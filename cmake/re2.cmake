# Copyright 2026 Google LLC
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

FetchContent_Declare(re2
  URL https://github.com/google/re2/releases/download/2025-11-05/re2-2025-11-05.tar.gz
  URL_HASH SHA256=87f6029d2f6de8aa023654240a03ada90e876ce9a4676e258dd01ea4c26ffd67
)
set(RE2_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(re2)
