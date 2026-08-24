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

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_GET_EXIF_INFO_LEN_LEFTOVER_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_GET_EXIF_INFO_LEN_LEFTOVER_H

#include <cstddef>

namespace OHOS {
namespace Media {
namespace GetExifInfoLenLeftover {
constexpr size_t DECIMAL_BASE = 10;

/*
 * leftover GetExifInfoLen: after decimal digits, require a newline before skip.
 * ImageMagick raw-profile uses "<len>\n<hex>". The leftover skip always advanced
 * one byte, so "6X457869" ate 'X' and hex-parsed the corrupted tail.
 * endPtr points at the last valid character (same convention as ConvertRawTextToExifInfo).
 */
inline const char *GetExifInfoLen(const char *sourcePtr, size_t *lengthOut, const char *endPtr)
{
    if (sourcePtr == nullptr || lengthOut == nullptr || endPtr == nullptr || sourcePtr >= endPtr) {
        return nullptr;
    }

    while ((*sourcePtr == '\0') || (*sourcePtr == ' ') || (*sourcePtr == '\n')) {
        sourcePtr++;
        if (sourcePtr == endPtr) {
            return nullptr;
        }
    }

    size_t exifLength = 0;
    while (('0' <= *sourcePtr) && (*sourcePtr <= '9')) {
        size_t tmpExifLength = 0;
        if (__builtin_mul_overflow(exifLength, DECIMAL_BASE, &tmpExifLength)) {
            return nullptr;
        }
        if (__builtin_add_overflow(tmpExifLength, static_cast<size_t>(*sourcePtr - '0'), &tmpExifLength)) {
            return nullptr;
        }
        exifLength = tmpExifLength;
        sourcePtr++;
        if (sourcePtr == endPtr) {
            return nullptr;
        }
    }
    if (*sourcePtr != '\n') {
        return nullptr;
    }
    sourcePtr++;
    if (sourcePtr == endPtr) {
        return nullptr;
    }
    *lengthOut = exifLength;
    return sourcePtr;
}
} // namespace GetExifInfoLenLeftover
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_GET_EXIF_INFO_LEN_LEFTOVER_H
