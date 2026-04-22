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

load("@rules_python//python:py_binary.bzl", "py_binary")
load("@rules_python//python:py_library.bzl", "py_library")
load("@rules_python//python:py_test.bzl", "py_test")

licenses(["notice"])

py_library(
    name = "sapi_code",
    srcs = ["sapi_code.py"],
)

py_test(
    name = "sapi_code_test",
    size = "small",
    srcs = [
        "sapi_code_test.py",
        "sapi_code_test_util.py",
    ],
    deps = [
        ":sapi_code",
        "@abseil-py//absl/testing:absltest",
        "@abseil-py//absl/testing:parameterized",
    ],
)

py_binary(
    name = "sapi_generator",
    srcs = ["sapi_generator.py"],
    visibility = ["//visibility:public"],
    deps = [
        ":sapi_code",
        "@abseil-py//absl:app",
        "@abseil-py//absl/flags",
        "@abseil-py//absl/logging",
    ],
)
