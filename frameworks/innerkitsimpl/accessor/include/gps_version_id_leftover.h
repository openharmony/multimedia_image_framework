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

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_GPS_VERSION_ID_LEFTOVER_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_GPS_VERSION_ID_LEFTOVER_H

#include <regex>
#include <string>

namespace OHOS {
namespace Media {
namespace GpsVersionIdLeftover {
/*
 * leftover GPSVersionID: unescaped '.' was any char, so "9x9x9x9"
 * matched ValidRegexWithDot. Escape dots so only literal "9.9.9.9"
 * style values match. Distinct from PNG hex leftover (#1) and
 * ConvertToDouble leftover (#2).
 */
inline constexpr const char *TRIBLE_INT_WITH_DOT_REGEX = R"(^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$)";

inline bool MatchesGpsVersionId(const std::string &value)
{
    return std::regex_match(value, std::regex(TRIBLE_INT_WITH_DOT_REGEX));
}
} // namespace GpsVersionIdLeftover
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_GPS_VERSION_ID_LEFTOVER_H
