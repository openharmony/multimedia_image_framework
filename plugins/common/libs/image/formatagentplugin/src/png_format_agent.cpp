/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "png_format_agent.h"

#include "image_log.h"
#include "plugin_service.h"
#include "sched.h"
#include "string"

#undef LOG_DOMAIN
#define LOG_DOMAIN LOG_TAG_DOMAIN_ID_PLUGIN

#undef LOG_TAG
#define LOG_TAG "PngFormatAgent"

namespace OHOS {
namespace ImagePlugin {
using namespace MultimediaPlugin;

static const std::string FORMAT_TYPE = "image/png";
static constexpr uint8_t PNG_HEADER[] = { 137, 80, 78, 71, 13, 10, 26, 10 };

std::string PngFormatAgent::GetFormatType()
{
    return FORMAT_TYPE;
}

uint32_t PngFormatAgent::GetHeaderSize()
{
    return (sizeof(PNG_HEADER));
}

bool PngFormatAgent::CheckFormat(const void *headerData, uint32_t dataSize)
{
    if (headerData == nullptr || dataSize == 0) {
        IMAGE_LOGE("check format input parameter abnormal.");
        return false;
    }
    uint32_t headerSize = sizeof(PNG_HEADER);
    if (dataSize < headerSize) {
        IMAGE_LOGE("read head size:[%{public}u] less than header size:[%{public}u].", dataSize, headerSize);
        return false;
    }
    return !memcmp(headerData, PNG_HEADER, headerSize);
}
} // namespace ImagePlugin
} // namespace OHOS
