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

#ifndef FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_GET_FRACTION_FROM_STR_LEFTOVER_H
#define FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_GET_FRACTION_FROM_STR_LEFTOVER_H

#include <charconv>
#include <limits>
#include <string>
#include <system_error>

namespace OHOS {
namespace Media {
namespace GetFractionFromStrLeftover {
constexpr int DECIMAL_BASE = 10;

inline int Gcd(int a, int b)
{
    if (b == 0) {
        return a;
    }
    return Gcd(b, a % b);
}

/*
 * leftover GetFractionFromStr: scale fractional digits as an integer
 * (digit string / 10^n). Do not use (int)(decPart * pow(10, n)), which
 * truncates 0.29*100 → 28.999… → 28 and 0.57*100 → 56.
 */
inline bool ScaleDecimalDigits(const std::string &digits, int &numerator, int &denominator)
{
    long long num = 0;
    long long den = 1;
    const long long maxInt = static_cast<long long>(std::numeric_limits<int>::max());
    for (char raw : digits) {
        unsigned char c = static_cast<unsigned char>(raw);
        if (c < '0' || c > '9') {
            break;
        }
        const int digit = c - '0';
        if (num > (maxInt - digit) / DECIMAL_BASE) {
            return false;
        }
        if (den > maxInt / DECIMAL_BASE) {
            return false;
        }
        num = num * DECIMAL_BASE + digit;
        den *= DECIMAL_BASE;
    }
    numerator = static_cast<int>(num);
    denominator = static_cast<int>(den);
    return true;
}

inline std::string GetFractionFromStr(const std::string &decimal, bool &isOutRange)
{
    const auto dotPos = decimal.find('.');
    if (dotPos == std::string::npos) {
        isOutRange = true;
        return "";
    }

    const std::string intPartStr = decimal.substr(0, dotPos);
    int intPart = 0;
    auto [p, ec] = std::from_chars(intPartStr.data(), intPartStr.data() + intPartStr.size(), intPart);
    if (ec != std::errc()) {
        isOutRange = true;
        return "";
    }

    int numerator = 0;
    int denominator = 1;
    if (!ScaleDecimalDigits(decimal.substr(dotPos + 1), numerator, denominator)) {
        isOutRange = true;
        return "";
    }

    int gcdVal = Gcd(numerator, denominator);
    if (gcdVal == 0) {
        return std::to_string(numerator + intPart * denominator) + "/" + std::to_string(denominator);
    }
    numerator /= gcdVal;
    denominator /= gcdVal;
    numerator += intPart * denominator;
    return std::to_string(numerator) + "/" + std::to_string(denominator);
}
} // namespace GetFractionFromStrLeftover
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_ACCESSOR_INCLUDE_GET_FRACTION_FROM_STR_LEFTOVER_H
