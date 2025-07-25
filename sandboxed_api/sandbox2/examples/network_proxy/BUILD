# Copyright 2019 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# The 'network proxy' example demonstrates how to use network proxy server.

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("//sandboxed_api/bazel:build_defs.bzl", "sapi_platform_copts")

licenses(["notice"])

# Executor
cc_binary(
    name = "networkproxy_sandbox",
    srcs = ["networkproxy_sandbox.cc"],
    copts = sapi_platform_copts(),
    data = [":networkproxy_bin"],
    deps = [
        "//sandboxed_api:config",
        "//sandboxed_api/sandbox2",
        "//sandboxed_api/sandbox2:comms",
        "//sandboxed_api/sandbox2/network_proxy:testing",
        "//sandboxed_api/util:runfiles",
        "@abseil-cpp//absl/base:core_headers",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:globals",
        "@abseil-cpp//absl/log:initialize",
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/strings:string_view",
        "@abseil-cpp//absl/time",
    ],
)

# Sandboxee
cc_binary(
    name = "networkproxy_bin",
    srcs = ["networkproxy_bin.cc"],
    copts = sapi_platform_copts(),
    deps = [
        "//sandboxed_api/sandbox2:client",
        "//sandboxed_api/sandbox2:comms",
        "//sandboxed_api/sandbox2/network_proxy:client",
        "//sandboxed_api/util:fileops",
        "//sandboxed_api/util:status",
        "@abseil-cpp//absl/base:log_severity",
        "@abseil-cpp//absl/flags:flag",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:globals",
        "@abseil-cpp//absl/log:initialize",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/strings:str_format",
        "@abseil-cpp//absl/strings:string_view",
    ],
)

# Test
sh_test(
    name = "networkproxy_sandbox_test",
    srcs = ["networkproxy_sandbox_test.sh"],
    data = [":networkproxy_sandbox"],
    tags = ["no_qemu_user_mode"],
)
