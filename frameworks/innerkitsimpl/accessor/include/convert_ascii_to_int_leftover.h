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

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_CONVERT_ASCII_TO_INT_LEFTOVER_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_CONVERT_ASCII_TO_INT_LEFTOVER_H

#include <cstddef>

namespace OHOS {
namespace Media {
namespace ConvertAsciiToIntLeftover {
constexpr unsigned int HEX_BASE = 16;
constexpr size_t HEX_STRING_UNIT_SIZE = 2;
constexpr int SUCCESS = 0;
constexpr int ERR_IMAGE_SOURCE_DATA_INCOMPLETE = 1;

/*
 * leftover vs #1: this helper does not widen hex charset (A-F is #1).
 * leftover vs #8: does not retouch ConvertRawTextToExifInfo endPtr construction.
 *
 * Production ConvertAsciiToInt counted skipped newlines against
 * sourceLength = 2 * exifInfoLength. ImageMagick wraps hex every 72 chars,
 * so an 80-hex (40-byte) payload + one wrap returned SUCCESS with the last
 * nibble dropped (dest[39] == 0x20 instead of 0x28).
 *
 * endPtr is the last valid character (same convention as ConvertRawTextToExifInfo).
 * When endPtr is null, walk the leftover exclusive 2*L window only.
 */
inline bool IsHexAscii(char value)
{
    return (value >= '0' && value <= '9') || (value >= 'a' && value <= 'f');
}

inline unsigned char HexAsciiValue(char hexChar)
{
    if (hexChar >= '0' && hexChar <= '9') {
        return static_cast<unsigned char>(hexChar - '0');
    }
    if (hexChar >= 'a' && hexChar <= 'f') {
        return static_cast<unsigned char>(hexChar - 'a' + 10);
    }
    return 0;
}

inline int ConvertAsciiToInt(const char *sourcePtr, size_t exifInfoLength, unsigned char *destPtr,
    const char *endPtr)
{
    if (sourcePtr == nullptr || destPtr == nullptr) {
        return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
    }
    if (exifInfoLength == 0) {
        return SUCCESS;
    }

    const size_t needed = exifInfoLength * HEX_STRING_UNIT_SIZE;
    const char *last = (endPtr != nullptr) ? endPtr : (sourcePtr + needed - 1);
    size_t hexGot = 0;
    const char *cursor = sourcePtr;
    while (hexGot < needed) {
        if (cursor > last) {
            return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
        }
        if (*cursor == '\0') {
            return ERR_IMAGE_SOURCE_DATA_INCOMPLETE;
        }
        if (!IsHexAscii(*cursor)) {
            ++cursor;
            continue;
        }
        const unsigned char hexValue = HexAsciiValue(*cursor);
        if ((hexGot % HEX_STRING_UNIT_SIZE) == 0) {
            destPtr[hexGot / HEX_STRING_UNIT_SIZE] =
                static_cast<unsigned char>(HEX_BASE * hexValue);
        } else {
            destPtr[hexGot / HEX_STRING_UNIT_SIZE] =
                static_cast<unsigned char>(destPtr[hexGot / HEX_STRING_UNIT_SIZE] + hexValue);
        }
        ++hexGot;
        ++cursor;
    }
    return SUCCESS;
}
} // namespace ConvertAsciiToIntLeftover
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_CONVERT_ASCII_TO_INT_LEFTOVER_H
