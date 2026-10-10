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

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_PNG_ITXT_TEXTLEN_LEFTOVER_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_PNG_ITXT_TEXTLEN_LEFTOVER_H

#include <cstddef>
#include <limits>

namespace OHOS {
namespace Media {
namespace PngItxtTextLenLeftover {
/*
 * leftover iTXt textLen: Size - (keySize + 3 + languageTextLen + 1 +
 * translatedKeyTextLen + 1) with no Size >= prefix guard. When the
 * prefix exceeds Size, size_t underflows and DataBuf/DecompressText
 * sees a huge length. Distinct from leftover #1-#4.
 *
 * leftover nullCount used half-open [keySize+3, Size-1), dropping a
 * last-byte NUL. Exclusive end is Size so the last byte is counted.
 */

inline bool AddToPrefix(size_t &prefix, size_t addend)
{
    if (addend > (std::numeric_limits<size_t>::max() - prefix)) {
        return false;
    }
    prefix += addend;
    return true;
}

inline bool GetItxtPrefixLen(size_t keySize, size_t languageTextLen, size_t translatedKeyTextLen,
    size_t &prefixLen)
{
    prefixLen = keySize;
    /* compression flag, method, and language-tag start: +3 */
    if (!AddToPrefix(prefixLen, 3)) {
        return false;
    }
    if (!AddToPrefix(prefixLen, languageTextLen) || !AddToPrefix(prefixLen, 1)) {
        return false;
    }
    if (!AddToPrefix(prefixLen, translatedKeyTextLen) || !AddToPrefix(prefixLen, 1)) {
        return false;
    }
    return true;
}

inline bool GetItxtTextLen(size_t chunkSize, size_t keySize, size_t languageTextLen,
    size_t translatedKeyTextLen, size_t &textLen)
{
    size_t prefixLen = 0;
    if (!GetItxtPrefixLen(keySize, languageTextLen, translatedKeyTextLen, prefixLen)) {
        return false;
    }
    if (chunkSize < prefixLen) {
        return false;
    }
    textLen = chunkSize - prefixLen;
    return true;
}

/* leftover subtract: no Size >= prefix guard. */
inline size_t LeftoverItxtTextLen(size_t chunkSize, size_t keySize, size_t languageTextLen,
    size_t translatedKeyTextLen)
{
    return chunkSize - (keySize + 3 + languageTextLen + 1 + translatedKeyTextLen + 1);
}

/* leftover exclusive end dropped the last byte (Size - 1). */
inline size_t LeftoverItxtNullCountEnd(size_t chunkSize)
{
    return (chunkSize == 0) ? 0 : (chunkSize - 1);
}

inline size_t ItxtNullCountEnd(size_t chunkSize)
{
    return chunkSize;
}
} // namespace PngItxtTextLenLeftover
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_PNG_ITXT_TEXTLEN_LEFTOVER_H
