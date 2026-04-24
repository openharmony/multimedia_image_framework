/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "bandjpeg/fast_manager.h"
#include "image_log.h"
#include <sys/stat.h>
#include <iostream>

namespace OHOS {
namespace ImagePlugin {
FASTManager& FastManager::GetInstance()
{
    static FastManager instance;
    return instance;
}

FastManager::FastManager()
{
    FastHandle_ = dlopen(FASTLib_, RTLD_NOW);
    CHECK_ERROR_RETURN_LOG(FastHandle_ == nullptr, "FASTManager dlopen FASTlib failed: %{public}s", dlerror());

    DecodeImage = reinterpret_cast<fast::image::FastErrCode (*)(const uint8_t*, size_t, uint8_t*, uint32_t, uint32_t,
        size_t, fast::image::RGBFormat)>(dlsym(FastHandle_, "_"));
}
}
}
