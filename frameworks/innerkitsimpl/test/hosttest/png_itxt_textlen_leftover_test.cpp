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

/*
 * Host leftover test for iTXt textLen size_t underflow (no Harmony device).
 *
 * g++ -std=c++17 -I frameworks/innerkitsimpl/accessor/include \
 *   frameworks/innerkitsimpl/test/hosttest/png_itxt_textlen_leftover_test.cpp \
 *   -o /tmp/png_itxt_textlen_leftover_test && /tmp/png_itxt_textlen_leftover_test
 */

#include "png_itxt_textlen_leftover.h"

#include <algorithm>
#include <cstdio>
#include <limits>
#include <vector>

using OHOS::Media::PngItxtTextLenLeftover::GetItxtTextLen;
using OHOS::Media::PngItxtTextLenLeftover::ItxtNullCountEnd;
using OHOS::Media::PngItxtTextLenLeftover::LeftoverItxtNullCountEnd;
using OHOS::Media::PngItxtTextLenLeftover::LeftoverItxtTextLen;

static int gFails = 0;

#define EXPECT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++gFails; \
        } \
    } while (0)

static void TestLeftoverUnderflowOnCraftedSizes()
{
    /* prefix = 0 + 3 + 3 + 1 + 3 + 1 = 11 > Size 8 */
    const size_t chunkSize = 8;
    const size_t leftover = LeftoverItxtTextLen(chunkSize, 0, 3, 3);
    EXPECT_TRUE(leftover > chunkSize);
    EXPECT_TRUE(leftover == (std::numeric_limits<size_t>::max() - 2));

    size_t textLen = 0xdeadbeef;
    EXPECT_TRUE(!GetItxtTextLen(chunkSize, 0, 3, 3, textLen));
}

static void TestPrefixEqualSizeIsEmptyText()
{
    /* prefix = 0 + 3 + 0 + 1 + 0 + 1 = 5, Size 5 → textLen 0 */
    size_t textLen = 0xdeadbeef;
    EXPECT_TRUE(GetItxtTextLen(5, 0, 0, 0, textLen));
    EXPECT_TRUE(textLen == 0);
}

static void TestValidRemainder()
{
    /* prefix = 2 + 3 + 1 + 1 + 2 + 1 = 10, Size 14 → textLen 4 */
    size_t textLen = 0;
    EXPECT_TRUE(GetItxtTextLen(14, 2, 1, 2, textLen));
    EXPECT_TRUE(textLen == 4);
}

static void TestKeySizeNearMaxDoesNotWrapPrefix()
{
    const size_t hugeKey = std::numeric_limits<size_t>::max() - 2;
    size_t textLen = 0xdeadbeef;
    EXPECT_TRUE(!GetItxtTextLen(16, hugeKey, 0, 0, textLen));
}

static void TestNullCountIncludesLastByte()
{
    /* crafted: NULs at offset 3 and last byte (index 5). Size 6. */
    const std::vector<unsigned char> chunk = { 'K', 0, 0, 0, 'x', 0 };
    const size_t keySize = 0;
    const size_t begin = keySize + 3;

    const size_t leftoverEnd = LeftoverItxtNullCountEnd(chunk.size());
    const size_t leftoverCount = static_cast<size_t>(std::count(
        chunk.data() + begin, chunk.data() + leftoverEnd, '\0'));
    EXPECT_TRUE(leftoverEnd == chunk.size() - 1);
    EXPECT_TRUE(leftoverCount == 1);

    const size_t fixedEnd = ItxtNullCountEnd(chunk.size());
    const size_t fixedCount = static_cast<size_t>(std::count(
        chunk.data() + begin, chunk.data() + fixedEnd, '\0'));
    EXPECT_TRUE(fixedEnd == chunk.size());
    EXPECT_TRUE(fixedCount == 2);
}

int main()
{
    TestLeftoverUnderflowOnCraftedSizes();
    TestPrefixEqualSizeIsEmptyText();
    TestValidRemainder();
    TestKeySizeNearMaxDoesNotWrapPrefix();
    TestNullCountIncludesLastByte();

    if (gFails != 0) {
        std::fprintf(stderr, "%d leftover test(s) failed\n", gFails);
        return 1;
    }
    std::printf("iTXt textLen leftover host tests passed\n");
    return 0;
}
