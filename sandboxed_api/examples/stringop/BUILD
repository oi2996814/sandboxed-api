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

# Description: Example using dynamic length structures for Sandboxed API

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")
load("//sandboxed_api/bazel:build_defs.bzl", "sapi_platform_copts")
load("//sandboxed_api/bazel:proto.bzl", "sapi_proto_library")
load("//sandboxed_api/bazel:sapi.bzl", "sapi_library")

package(default_visibility = ["//sandboxed_api:__subpackages__"])

licenses(["notice"])

sapi_proto_library(
    name = "stringop_params_proto",
    srcs = ["stringop_params.proto"],
    visibility = ["//visibility:public"],
    alwayslink = True,
)

cc_library(
    name = "stringop",
    srcs = ["stringop.cc"],
    copts = sapi_platform_copts(),
    linkstatic = True,
    deps = [
        ":stringop_params_cc_proto",
        "//sandboxed_api:lenval_core",
        "@abseil-cpp//absl/base:core_headers",
    ],
    alwayslink = True,
)

STRINGOP_FUNCTIONS = [
    "duplicate_string",
    "reverse_string",
    "pb_duplicate_string",
    "pb_reverse_string",
    "nop",
    "violate",
    "get_raw_c_string",
]

sapi_library(
    name = "stringop-sapi",
    functions = STRINGOP_FUNCTIONS,
    generator_version = 2,
    input_files = ["stringop.cc"],
    lib = ":stringop",
    lib_name = "Stringop",
    namespace = "",
    deps = [":stringop_params_cc_proto"],
)

cc_test(
    name = "main_stringop",
    srcs = ["main_stringop.cc"],
    copts = sapi_platform_copts(),
    tags = ["local"],
    deps = [
        ":stringop-sapi",
        ":stringop_params_cc_proto",
        "//sandboxed_api:sapi",
        "//sandboxed_api:testing",
        "//sandboxed_api:vars",
        "//sandboxed_api/util:status",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/memory",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:status_matchers",
        "@abseil-cpp//absl/status:statusor",
        "@abseil-cpp//absl/strings:string_view",
        "@googletest//:gtest_main",
    ],
)
