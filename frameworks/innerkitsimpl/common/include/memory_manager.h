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

#ifndef FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_MEMORY_MANAGER_H_
#define FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_MEMORY_MANAGER_H_

#include <cstdint>
#include <cstddef>
#include <memory>
#include <string>
#include "image_type.h"

namespace OHOS {
namespace Media {
struct MemoryData {
    void* data;
    size_t size;
    const char* tag;
    // std::string tag;
};

class AbsMemory {
public:
    virtual ~AbsMemory() {};
    virtual uint32_t Create();
    virtual uint32_t Release();
    virtual AllocatorType GetType();
    MemoryData data;
    MemoryData extend;
};
class HeapMemory : public AbsMemory {
public:
    ~HeapMemory() = default;
    uint32_t Create() override;
    uint32_t Release() override;
    AllocatorType GetType() override
    {
        return AllocatorType::HEAP_ALLOC;
    }
};
class SharedMemory : public AbsMemory {
public:
    ~SharedMemory() = default;
    uint32_t Create() override;
    uint32_t Release() override;
    AllocatorType GetType() override
    {
        return AllocatorType::SHARE_MEM_ALLOC;
    }
};
class MemoryManager {
public:
    static std::unique_ptr<AbsMemory> CreateMemory(AllocatorType type, MemoryData &data);
    static std::unique_ptr<AbsMemory> CreateMemory(AllocatorType type, MemoryData &data, MemoryData &extend);
    static std::unique_ptr<AbsMemory> TransMemoryType(const AbsMemory &source, AllocatorType target,
        std::string tag);
};
} // namespace Media
} // namespace OHOS

#endif // FRAMEWORKS_INNERKITSIMPL_COMMON_INCLUDE_MEMORY_MANAGER_H_