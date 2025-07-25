// Copyright 2024 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "sandboxed_api/sandbox2/util/deadline_manager.h"

#include <sys/syscall.h>

#include <algorithm>
#include <csignal>
#include <cstdint>
#include <memory>
#include <string>

#include "absl/base/call_once.h"
#include "absl/flags/flag.h"
#include "absl/functional/function_ref.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/mutex.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "sandboxed_api/sandbox2/flags.h"
#include "sandboxed_api/sandbox2/util.h"
#include "sandboxed_api/util/thread.h"

namespace sandbox2 {
namespace {

constexpr int kFailedNotificationsThreshold = 32;

absl::Time RoundUpTo(absl::Time time, absl::Duration resolution) {
  return time == absl::InfiniteFuture()
             ? absl::InfiniteFuture()
             : absl::UnixEpoch() +
                   absl::Ceil(time - absl::UnixEpoch(), resolution);
}

}  // namespace

int DeadlineManager::signal_nr_ = -1;

DeadlineRegistration::DeadlineRegistration(DeadlineManager& manager)
    : manager_(manager) {
  manager_.Register(*this);
}

DeadlineRegistration::~DeadlineRegistration() { manager_.Unregister(*this); }

void DeadlineRegistration::SetDeadline(absl::Time deadline) {
  if (deadline != last_deadline_) {
    manager_.AdjustDeadline(*this, deadline);
    last_deadline_ = deadline;
  }
}

void DeadlineRegistration::ExecuteBlockingSyscall(
    absl::FunctionRef<void()> blocking_fn) {
  {
    absl::MutexLock lock(&data_->mutex);
    data_->tid = util::Syscall(__NR_gettid);
    if (data_->notification_attempt > 0 || data_->deadline <= absl::Now()) {
      return;
    }
    data_->in_blocking_fn = true;
  }
  blocking_fn();
  {
    absl::MutexLock lock(&data_->mutex);
    data_->in_blocking_fn = false;
  }
}

DeadlineManager::DeadlineManager(absl::string_view name) {
  RegisterSignalHandler();
  thread_ = sapi::Thread(this, &DeadlineManager::Run, name);
}

DeadlineManager::~DeadlineManager() {
  {
    absl::MutexLock lock(&registration_mutex_);
    CHECK_EQ(registered_deadlines_, 0);
  }
  {
    absl::MutexLock lock(&queue_mutex_);
    cancelled_ = true;
  }
  if (thread_.IsJoinable()) {
    thread_.Join();
  }
}

void DeadlineManager::SignalHandler(int signal) {}

void DeadlineManager::AdjustDeadline(DeadlineRegistration& registration,
                                     absl::Time deadline) {
  absl::MutexLock lock(&queue_mutex_);
  queue_.erase(registration.data_.get());
  absl::MutexLock data_lock(&registration.data_->mutex);
  registration.data_->notification_attempt = 0;
  registration.data_->deadline = RoundUpTo(deadline, kResolution);
  if (deadline != absl::InfiniteFuture()) {
    queue_.insert(registration.data_.get());
  }
}

void DeadlineManager::RegisterSignalHandler() {
  static absl::once_flag once;
  absl::call_once(once, [] {
    signal_nr_ = absl::GetFlag(FLAGS_sandbox2_deadline_manager_signal);
    struct sigaction sa = {};
    sa.sa_flags = 0;
    sa.sa_handler = DeadlineManager::SignalHandler;
    struct sigaction old = {};
    PCHECK(sigaction(signal_nr_, &sa, &old) == 0);
    // Verify that previously there was no handler set.
    if (old.sa_flags & SA_SIGINFO) {
      LOG_IF(WARNING, old.sa_sigaction != nullptr)
          << "Signal handler was already registered";
    } else {
      LOG_IF(WARNING, old.sa_handler != nullptr)
          << "Signal handler was already registered";
    }
    return true;
  });
}

void DeadlineManager::VerifySignalHandler() {
  struct sigaction old = {};
  PCHECK(sigaction(signal_nr_, nullptr, &old) == 0);
  auto describe = [](uintptr_t handler) -> std::string {
    return absl::StrCat(absl::Hex(handler));
  };
  uintptr_t old_handler = (old.sa_flags & SA_SIGINFO)
                              ? reinterpret_cast<uintptr_t>(old.sa_sigaction)
                              : reinterpret_cast<uintptr_t>(old.sa_handler);
  CHECK(old_handler ==
        reinterpret_cast<uintptr_t>(DeadlineManager::SignalHandler))
      << "Signal handler was overriden " << describe(old_handler);
  CHECK((old.sa_flags & SA_RESTART) == 0)
      << "SA_RESTART signal handler flag was overriden: "
      << absl::Hex(old.sa_flags);
}

void DeadlineManager::Run() {
  absl::Time next_deadline;
  auto next_deadline_changed_or_cancelled = [this, &next_deadline]() -> bool {
    queue_mutex_.AssertReaderHeld();
    return (!queue_.empty() && next_deadline != (*queue_.begin())->deadline) ||
           cancelled_;
  };
  absl::MutexLock lock(&queue_mutex_);
  while (!cancelled_) {
    next_deadline = absl::InfiniteFuture();
    if (!queue_.empty()) {
      next_deadline = (*queue_.begin())->deadline;
    }
    if (queue_mutex_.AwaitWithDeadline(
            absl::Condition(&next_deadline_changed_or_cancelled),
            next_deadline)) {
      continue;
    }
    while (!queue_.empty() && (*queue_.begin())->deadline <= next_deadline) {
      DeadlineRegistration::Data* entry = *queue_.begin();
      queue_.erase(queue_.begin());
      absl::MutexLock lock(&entry->mutex);
      if (++entry->notification_attempt > kFailedNotificationsThreshold) {
        VerifySignalHandler();
      }
      if (entry->in_blocking_fn) {
        constexpr int kExponentialBackoffRate = 8;
        constexpr int kMaxExponentialBackoff = 10;
        int exp_backoff =
            1 << std::min(entry->notification_attempt / kExponentialBackoffRate,
                          kMaxExponentialBackoff);
        absl::Time next_notification_time =
            RoundUpTo(absl::Now() + kResolution * exp_backoff, kResolution);
        util::Syscall(__NR_tgkill, getpid(), entry->tid, signal_nr_);
        entry->deadline = next_notification_time;
        queue_.insert(entry);
      }
    }
  }
}

}  // namespace sandbox2
