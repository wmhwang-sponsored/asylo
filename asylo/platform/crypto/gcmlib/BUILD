#
# Copyright 2018 Asylo authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

licenses(["notice"])  # Apache v2.0

# GCM library for secure storage.

load("//asylo/bazel:asylo.bzl", "ASYLO_ALL_BACKENDS", "cc_enclave_test")
load("//asylo/bazel:copts.bzl", "ASYLO_DEFAULT_COPTS")

package(
    default_visibility = ["//asylo:implementation"],
)

cc_library(
    name = "trusted_gcmlib",
    copts = ASYLO_DEFAULT_COPTS,
    deps = select({
        "@com_google_asylo//asylo": ["gcm_cryptor"],
        "//conditions:default": [],
    }),
)

cc_library(
    name = "gcm_cryptor",
    srcs = ["gcm_cryptor.cc"],
    hdrs = ["gcm_cryptor.h"],
    copts = ASYLO_DEFAULT_COPTS,
    tags = ASYLO_ALL_BACKENDS,
    deps = [
        "//asylo/crypto/util:bssl_util",
        "//asylo/crypto/util:byte_container_view",
        "//asylo/crypto/util:bytes",
        "//asylo/util:logging",
        "@boringssl//:crypto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
    ],
)

# GCM cryptor test in enclave.
cc_enclave_test(
    name = "gcm_cryptor_test",
    srcs = ["gcm_cryptor_test.cc"],
    copts = ASYLO_DEFAULT_COPTS,
    deps = [
        "@com_google_googletest//:gtest",
    ],
)
