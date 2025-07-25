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

load("@bazel_skylib//:bzl_library.bzl", "bzl_library")

licenses(["notice"])

exports_files([
    "proto.bzl",
    "embed_data.bzl",
    "sapi.bzl",
])

bzl_library(
    name = "build_defs_bzl",
    srcs = ["build_defs.bzl"],
    visibility = ["//visibility:private"],
)

bzl_library(
    name = "embed_data_bzl",
    srcs = ["embed_data.bzl"],
    visibility = ["//visibility:private"],
    deps = [
        "@rules_cc//cc:core_rules",
    ],
)

bzl_library(
    name = "proto_bzl",
    srcs = ["proto.bzl"],
    visibility = ["//visibility:private"],
    deps = [
        "@rules_proto//proto:defs",
    ],
)

bzl_library(
    name = "repositories_bzl",
    srcs = ["repositories.bzl"],
    visibility = ["//visibility:private"],
)

bzl_library(
    name = "sapi",
    srcs = ["sapi.bzl"],
    visibility = ["//visibility:private"],
    deps = [
        ":build_defs_bzl",
        ":embed_data_bzl",
        ":proto_bzl",
        "@bazel_tools//tools/cpp:toolchain_utils.bzl",
        "@rules_cc//cc:core_rules",
    ],
)

bzl_library(
    name = "llvm_config_bzl",
    srcs = ["llvm_config.bzl"],
    visibility = ["//visibility:private"],
)
