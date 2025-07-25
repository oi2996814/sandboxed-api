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

# Description: Sandboxed API reimplementation of zlib's zpipe.c example.

load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("//sandboxed_api/bazel:build_defs.bzl", "sapi_platform_copts")
load("//sandboxed_api/bazel:sapi.bzl", "sapi_library")

package(default_visibility = ["//sandboxed_api:__subpackages__"])

licenses(["notice"])

sapi_library(
    name = "zlib-sapi",
    srcs = [],
    hdrs = [],
    functions = [
        "deflateInit_",
        "deflate",
        "deflateEnd",
    ],
    generator_version = 1,
    lib = "@net_zlib//:zlib",
    lib_name = "Zlib",
    namespace = "sapi::zlib",
)

cc_binary(
    name = "main_zlib",
    srcs = ["main_zlib.cc"],
    copts = sapi_platform_copts(),
    deps = [
        ":zlib-sapi",
        "//sandboxed_api:vars",
        "@abseil-cpp//absl/base:core_headers",
        "@abseil-cpp//absl/base:log_severity",
        "@abseil-cpp//absl/flags:parse",
        "@abseil-cpp//absl/log",
        "@abseil-cpp//absl/log:globals",
        "@abseil-cpp//absl/log:initialize",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/status:statusor",
    ],
)

# Tests input/output conversion of zlib and compares with golden files
sh_test(
    name = "main_zlib_test",
    srcs = ["main_zlib_test.sh"],
    data = [
        ":main_zlib",
    ],
    tags = ["notsan"],
)
