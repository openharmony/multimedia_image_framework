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

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_VERIFY_EXIF_ID_CODE_LEFTOVER_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_VERIFY_EXIF_ID_CODE_LEFTOVER_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

namespace OHOS {
namespace Media {
namespace VerifyExifIdCodeLeftover {
/*
 * leftover VerifyExifIdCode: "i < length - size" never checks the last
 * valid start. Exact "Exif\0\0" (length==6) and header at end-1 both miss.
 * Use i + size <= length; length < size returns max. Distinct from leftover
 * #1 (PNG hex / TIFF offset), #2 ConvertToDouble, #3 GPSVersionID,
 * #4 GetFractionFromStr.
 */
inline constexpr size_t EXIF_ID_CODE_SIZE = 6;
inline constexpr uint8_t EXIF_ID_CODE[EXIF_ID_CODE_SIZE] = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };

inline size_t VerifyExifIdCode(const uint8_t *data, size_t length)
{
    if (data == nullptr || length < EXIF_ID_CODE_SIZE) {
        return std::numeric_limits<size_t>::max();
    }
    for (size_t i = 0; i + EXIF_ID_CODE_SIZE <= length; ++i) {
        if (std::memcmp(data + i, EXIF_ID_CODE, EXIF_ID_CODE_SIZE) == 0) {
            return i;
        }
    }
    return std::numeric_limits<size_t>::max();
}
} // namespace VerifyExifIdCodeLeftover
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_VERIFY_EXIF_ID_CODE_LEFTOVER_H
