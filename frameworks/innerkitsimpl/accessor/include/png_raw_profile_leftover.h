/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_PNG_RAW_PROFILE_LEFTOVER_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_PNG_RAW_PROFILE_LEFTOVER_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace OHOS {
namespace Media {
namespace PngRawProfileLeftover {
constexpr size_t ASCII_TO_HEX_MAP_SIZE = 103;
constexpr size_t EXIF_HEADER_SIZE = 6;
constexpr unsigned int HEX_BASE = 16;
constexpr size_t HEX_STRING_UNIT_SIZE = 2;

inline bool IsHexAscii(char value)
{
    return (value >= '0' && value <= '9') || (value >= 'a' && value <= 'f') || (value >= 'A' && value <= 'F');
}

/* Indices 48-57: '0'-'9'; 65-70: 'A'-'F'; 97-102: 'a'-'f'. */
inline constexpr unsigned char HEX_ASCII_TO_INT[ASCII_TO_HEX_MAP_SIZE] = {
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 1,    2, 3, 4, 5, 6,    7, 8, 9, 0, 0,
    0, 0, 0, 0, 0,    10, 11, 12, 13, 14,    15, 0, 0, 0, 0,    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 0, 0, 0,    0, 0, 10, 11, 12,
    13, 14, 15,
};

inline unsigned char HexAsciiValue(char hexChar)
{
    return HEX_ASCII_TO_INT[static_cast<unsigned char>(hexChar)];
}

/*
 * leftover hex-ascii decoder: consume contiguous hex pairs.
 * "45786966" -> {0x45,0x78,0x69,0x66}; "ABCDEF" -> {0xAB,0xCD,0xEF}.
 */
inline bool DecodeHexAscii(const char *sourcePtr, size_t hexCharCount, uint8_t *destPtr)
{
    if (sourcePtr == nullptr || destPtr == nullptr || (hexCharCount % HEX_STRING_UNIT_SIZE) != 0) {
        return false;
    }
    for (size_t i = 0; i < hexCharCount; i += HEX_STRING_UNIT_SIZE) {
        if (!IsHexAscii(sourcePtr[i]) || !IsHexAscii(sourcePtr[i + 1])) {
            return false;
        }
        destPtr[i / HEX_STRING_UNIT_SIZE] = static_cast<uint8_t>(
            HEX_BASE * HexAsciiValue(sourcePtr[i]) + HexAsciiValue(sourcePtr[i + 1]));
    }
    return true;
}

inline bool DecodeHexAscii(const char *sourcePtr, size_t hexCharCount, std::vector<uint8_t> &dest)
{
    if (sourcePtr == nullptr || (hexCharCount % HEX_STRING_UNIT_SIZE) != 0) {
        return false;
    }
    dest.assign(hexCharCount / HEX_STRING_UNIT_SIZE, 0);
    return DecodeHexAscii(sourcePtr, hexCharCount, dest.data());
}

/*
 * leftover TIFF slice: payload after Exif\0\0 starts at exifHeadPos + EXIF_HEADER_SIZE,
 * not a hardcoded offset of 6.
 */
inline bool GetTiffSlice(size_t exifHeadPos, size_t exifInfoLength, size_t &tiffOffset, size_t &tiffLength)
{
    if (exifInfoLength < EXIF_HEADER_SIZE) {
        return false;
    }
    if (exifHeadPos > exifInfoLength - EXIF_HEADER_SIZE) {
        return false;
    }
    tiffOffset = exifHeadPos + EXIF_HEADER_SIZE;
    tiffLength = exifInfoLength - tiffOffset;
    return tiffLength > 0;
}

inline bool FindExifHeadPos(const uint8_t *exifInfo, size_t exifInfoLength, size_t &exifHeadPos)
{
    static const uint8_t EXIF_ID_CODE[EXIF_HEADER_SIZE] = { 0x45, 0x78, 0x69, 0x66, 0x00, 0x00 };
    if (exifInfo == nullptr || exifInfoLength < EXIF_HEADER_SIZE) {
        return false;
    }
    const size_t lastPos = exifInfoLength - EXIF_HEADER_SIZE;
    for (size_t i = 0; i <= lastPos; ++i) {
        if (memcmp(exifInfo + i, EXIF_ID_CODE, EXIF_HEADER_SIZE) == 0) {
            exifHeadPos = i;
            return true;
        }
    }
    return false;
}

inline bool SliceTiffAfterExifHeader(const uint8_t *exifInfo, size_t exifInfoLength,
    size_t &tiffOffset, size_t &tiffLength)
{
    size_t exifHeadPos = 0;
    if (!FindExifHeadPos(exifInfo, exifInfoLength, exifHeadPos)) {
        return false;
    }
    return GetTiffSlice(exifHeadPos, exifInfoLength, tiffOffset, tiffLength);
}
} // namespace PngRawProfileLeftover
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_PNG_RAW_PROFILE_LEFTOVER_H
