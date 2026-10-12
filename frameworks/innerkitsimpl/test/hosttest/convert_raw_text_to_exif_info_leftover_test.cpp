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
 * Host leftover test for ConvertRawTextToExifInfo exclusive endPtr (no Harmony device).
 *
 * g++ -std=c++17 -I frameworks/innerkitsimpl/accessor/include \
 *   frameworks/innerkitsimpl/test/hosttest/convert_raw_text_to_exif_info_leftover_test.cpp \
 *   -o /tmp/convert_raw_text_to_exif_info_leftover_test && \
 *   /tmp/convert_raw_text_to_exif_info_leftover_test
 */

#include "convert_raw_text_to_exif_info_leftover.h"

#include <cstdio>
#include <cstring>
#include <vector>

using OHOS::Media::ConvertRawTextToExifInfoLeftover::ConvertRawTextToBytes;
using OHOS::Media::ConvertRawTextToExifInfoLeftover::ConvertRawTextToBytesWithEnd;
using OHOS::Media::ConvertRawTextToExifInfoLeftover::ExclusiveEnd;
using OHOS::Media::ConvertRawTextToExifInfoLeftover::HexPayloadFits;
using OHOS::Media::ConvertRawTextToExifInfoLeftover::LeftoverInclusiveLastByte;

static int gFails = 0;

#define EXPECT_TRUE(cond) \
    do { \
        if (!(cond)) { \
            std::fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            ++gFails; \
        } \
    } while (0)

static void TestExclusiveEndIsOnePastLast()
{
    const char buf[] = { 'a', 'b', 'c' };
    const size_t size = sizeof(buf);
    EXPECT_TRUE(ExclusiveEnd(buf, size) == buf + size);
    EXPECT_TRUE(LeftoverInclusiveLastByte(buf, size) == buf + size - 1);
    EXPECT_TRUE(ExclusiveEnd(buf, size) != LeftoverInclusiveLastByte(buf, size));
}

static void TestLastHexDigitFitsOnlyWithExclusiveEnd()
{
    /* skip first byte, then \n, length 1, hex "4a" ending at last byte. */
    const char raw[] = { '\0', '\n', '1', '\n', '4', 'a' };
    const size_t size = sizeof(raw);
    const char *hexStart = raw + 4;
    EXPECT_TRUE(*hexStart == '4');
    EXPECT_TRUE(raw[size - 1] == 'a');
    EXPECT_TRUE(HexPayloadFits(hexStart, 1, ExclusiveEnd(raw, size)));
    EXPECT_TRUE(!HexPayloadFits(hexStart, 1, LeftoverInclusiveLastByte(raw, size)));
}

static void TestLastByteHexDigitIsKept()
{
    const char raw[] = { '\0', '\n', '1', '\n', '4', 'a' };
    std::vector<uint8_t> dest;
    EXPECT_TRUE(ConvertRawTextToBytes(raw, sizeof(raw), dest));
    EXPECT_TRUE(dest.size() == 1);
    EXPECT_TRUE(dest[0] == 0x4a);

    dest.clear();
    EXPECT_TRUE(!ConvertRawTextToBytesWithEnd(raw, sizeof(raw),
        LeftoverInclusiveLastByte(raw, sizeof(raw)), dest));
}

static void TestTrailingHexNibbleOnLongerProfile()
{
    const char raw[] = { '\0', '\n', '3', '\n', '0', '1', '0', '2', 'f', 'f' };
    std::vector<uint8_t> dest;
    EXPECT_TRUE(ConvertRawTextToBytes(raw, sizeof(raw), dest));
    EXPECT_TRUE(dest.size() == 3);
    EXPECT_TRUE(dest[0] == 0x01 && dest[1] == 0x02 && dest[2] == 0xff);

    dest.clear();
    EXPECT_TRUE(!ConvertRawTextToBytesWithEnd(raw, sizeof(raw),
        LeftoverInclusiveLastByte(raw, sizeof(raw)), dest));
}

int main()
{
    TestExclusiveEndIsOnePastLast();
    TestLastHexDigitFitsOnlyWithExclusiveEnd();
    TestLastByteHexDigitIsKept();
    TestTrailingHexNibbleOnLongerProfile();
    if (gFails != 0) {
        std::fprintf(stderr, "ConvertRawTextToExifInfo leftover host tests failed: %d\n", gFails);
        return 1;
    }
    std::printf("ConvertRawTextToExifInfo leftover host tests passed\n");
    return 0;
}
