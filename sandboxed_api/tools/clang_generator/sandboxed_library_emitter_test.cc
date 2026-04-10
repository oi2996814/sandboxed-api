// Copyright 2026 Google LLC
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

#include "sandboxed_api/tools/clang_generator/sandboxed_library_emitter.h"

#include <memory>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/status_matchers.h"
#include "absl/status/statusor.h"
#include "sandboxed_api/tools/clang_generator/frontend_action_test_util.h"
#include "sandboxed_api/tools/clang_generator/generator.h"

namespace sapi {
namespace {

using ::absl_testing::IsOk;
using ::testing::HasSubstr;
using ::testing::Not;

class SandboxedLibraryEmitterTest : public FrontendActionTest {};

TEST_F(SandboxedLibraryEmitterTest, SandboxeeThunkNotUsed) {
  GeneratorOptions options;
  options.name = "MyLib";
  SandboxedLibraryEmitter emitter;
  ASSERT_THAT(
      RunFrontendAction(R"(
        extern "C" int func_with_thunk(int a);

        [[clang::annotate("sandbox", "sandboxee_thunk", "func_with_thunk")]]
        int func_with_thunk_thunk(int a) {
          return func_with_thunk(a) + 1;
        }
      )",
                        std::make_unique<GeneratorAction>(&emitter, &options)),
      IsOk());

  ASSERT_EQ(emitter.PostParseAllFiles(), absl::OkStatus());
  absl::StatusOr<std::string> host_src = emitter.EmitHostSrc(options);
  ASSERT_THAT(host_src, IsOk());
  EXPECT_THAT(*host_src, HasSubstr("api.sapi_wrapper_func_with_thunk("));

  absl::StatusOr<std::string> sandboxee_src = emitter.EmitSandboxeeSrc(options);
  ASSERT_THAT(sandboxee_src, IsOk());
  // Problem: wrapper for func_with_thunk is generated, and thunk is not used.
  EXPECT_THAT(*sandboxee_src, HasSubstr("sapi_wrapper_func_with_thunk"));
  // The wrapper should call func_with_thunk, not func_with_thunk_thunk
  EXPECT_THAT(*sandboxee_src,
              HasSubstr("int sapi_ret_val = func_with_thunk(a);"));
  EXPECT_THAT(*sandboxee_src, Not(HasSubstr("func_with_thunk_thunk")));
}

}  // namespace
}  // namespace sapi
