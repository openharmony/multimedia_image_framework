/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_CONVERT_TO_DOUBLE_LEFTOVER_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_CONVERT_TO_DOUBLE_LEFTOVER_H

#include <cerrno>
#include <cstdlib>
#include <string>

namespace OHOS {
namespace Media {
namespace ConvertToDoubleLeftover {
/*
 * leftover ConvertToDouble: reject empty, trailing junk, and ERANGE.
 * Clean values such as "2.5" and ".5" succeed.
 * The previous polarity (errno == ERANGE && *endPtr != '\0') accepted
 * 1e99999 as +inf, "1.5abc" as 1.5, and "" as 0.
 */
inline bool ConvertToDouble(const std::string &str, double &value)
{
    errno = 0;
    char *endPtr = nullptr;
    value = strtod(str.c_str(), &endPtr);
    if (endPtr == str.c_str() || *endPtr != '\0' || errno == ERANGE) {
        return false;
    }
    return true;
}
} // namespace ConvertToDoubleLeftover
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_CONVERT_TO_DOUBLE_LEFTOVER_H
