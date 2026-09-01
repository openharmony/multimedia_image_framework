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

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_CONVERT_RAW_TEXT_TO_EXIF_INFO_LEFTOVER_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_CONVERT_RAW_TEXT_TO_EXIF_INFO_LEFTOVER_H

#include <cstddef>
#include <cstdint>
#include <vector>

namespace OHOS {
namespace Media {
namespace ConvertRawTextToExifInfoLeftover {
constexpr size_t HEX_PAIR_SIZE = 2;
constexpr unsigned int HEX_BASE = 16;

/*
 * DataBuf::CData(offset) returns nullptr when offset == Size()
 * (data_buf.cpp). Exclusive one-past-end cannot be CData(Size()).
 * leftover used CData(Size() - 1), which points AT the last byte, so
 * StepOverNewLine / GetExifInfoLen / the 2*len check never treat that
 * byte as profile content (trailing hex digit lost).
 */
inline const char *ExclusiveEnd(const char *cdata, size_t size)
{
    return cdata == nullptr ? nullptr : cdata + size;
}

inline const char *LeftoverInclusiveLastByte(const char *cdata, size_t size)
{
    if (cdata == nullptr || size == 0) {
        return cdata;
    }
    return cdata + (size - 1);
}

inline const char *StepOverNewLine(const char *sourcePtr, const char *endPtr)
{
    while (*sourcePtr != '\n') {
        sourcePtr++;
        if (sourcePtr == endPtr) {
            return nullptr;
        }
    }
    sourcePtr++;
    if (sourcePtr == endPtr) {
        return nullptr;
    }
    return sourcePtr;
}

inline const char *GetExifInfoLen(const char *sourcePtr, size_t *lengthOut, const char *endPtr)
{
    while ((*sourcePtr == '\0') || (*sourcePtr == ' ') || (*sourcePtr == '\n')) {
        sourcePtr++;
        if (sourcePtr == endPtr) {
            return nullptr;
        }
    }

    size_t exifLength = 0;
    while (('0' <= *sourcePtr) && (*sourcePtr <= '9')) {
        size_t tmpLength = 0;
        if (__builtin_mul_overflow(exifLength, 10, &tmpLength) ||
            __builtin_add_overflow(tmpLength, static_cast<size_t>(*sourcePtr - '0'), &tmpLength)) {
            return nullptr;
        }
        exifLength = tmpLength;
        sourcePtr++;
        if (sourcePtr == endPtr) {
            return nullptr;
        }
    }
    sourcePtr++;
    if (sourcePtr == endPtr) {
        return nullptr;
    }
    *lengthOut = exifLength;
    return sourcePtr;
}

inline bool IsHexAscii(char value)
{
    return (value >= '0' && value <= '9') || (value >= 'a' && value <= 'f');
}

inline unsigned char HexAsciiValue(char hexChar)
{
    if (hexChar >= '0' && hexChar <= '9') {
        return static_cast<unsigned char>(hexChar - '0');
    }
    return static_cast<unsigned char>(hexChar - 'a' + 10);
}

inline bool HexPayloadFits(const char *sourcePtr, size_t exifInfoLength, const char *endPtr)
{
    return sourcePtr + HEX_PAIR_SIZE * exifInfoLength <= endPtr;
}

inline bool ConvertHexPairs(const char *sourcePtr, size_t exifInfoLength, uint8_t *destPtr)
{
    if (sourcePtr == nullptr || destPtr == nullptr) {
        return false;
    }
    for (size_t i = 0; i < exifInfoLength; ++i) {
        if (!IsHexAscii(sourcePtr[0]) || !IsHexAscii(sourcePtr[1])) {
            return false;
        }
        destPtr[i] = static_cast<uint8_t>(HEX_BASE * HexAsciiValue(sourcePtr[0]) + HexAsciiValue(sourcePtr[1]));
        sourcePtr += HEX_PAIR_SIZE;
    }
    return true;
}

/*
 * leftover ConvertRawTextToExifInfo walk: skip first byte (CData(1)),
 * then StepOverNewLine / GetExifInfoLen / 2*len with exclusive end.
 */
inline bool ConvertRawTextToBytesWithEnd(const char *cdata, size_t size, const char *endPtr,
    std::vector<uint8_t> &out)
{
    if (cdata == nullptr || size <= 1 || endPtr == nullptr) {
        return false;
    }
    const char *sourcePtr = cdata + 1;
    if (sourcePtr >= endPtr) {
        return false;
    }
    sourcePtr = StepOverNewLine(sourcePtr, endPtr);
    if (sourcePtr == nullptr) {
        return false;
    }
    size_t exifInfoLength = 0;
    sourcePtr = GetExifInfoLen(sourcePtr, &exifInfoLength, endPtr);
    if (sourcePtr == nullptr || exifInfoLength == 0 || exifInfoLength > size) {
        return false;
    }
    if (!HexPayloadFits(sourcePtr, exifInfoLength, endPtr)) {
        return false;
    }
    out.assign(exifInfoLength, 0);
    return ConvertHexPairs(sourcePtr, exifInfoLength, out.data());
}

inline bool ConvertRawTextToBytes(const char *cdata, size_t size, std::vector<uint8_t> &out)
{
    return ConvertRawTextToBytesWithEnd(cdata, size, ExclusiveEnd(cdata, size), out);
}
} // namespace ConvertRawTextToExifInfoLeftover
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_CONVERT_RAW_TEXT_TO_EXIF_INFO_LEFTOVER_H
