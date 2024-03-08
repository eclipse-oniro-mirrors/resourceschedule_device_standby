/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "securec.h"
#include "devicestandby_fuzzer.h"

#include "standby_service.h"

namespace OHOS {
namespace DevStandbyMgr {
    constexpr size_t U32_AT_SIZE = 4;
    constexpr uint32_t MAX_CODE = 12;
    constexpr uint8_t TWENTYFOUR = 24;
    constexpr uint8_t SIXTEEN = 16;
    constexpr uint8_t EIGHT = 8;
    constexpr uint8_t TWO = 2;
    constexpr int32_t THREE = 3;
    const std::u16string DEVICE_STANDBY_TOKEN = u"ohos.resourceschedule.IStandbyService";

    uint32_t GetU32Data(const char* ptr)
    {
        return (ptr[0] << TWENTYFOUR) | (ptr[1] << SIXTEEN) | (ptr[TWO] << EIGHT) | (ptr[THREE]);
    }

    bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
    {
        uint32_t code = GetU32Data(data);
        MessageParcel datas;
        datas.WriteInterfaceToken(DEVICE_STANDBY_TOKEN);
        datas.WriteBuffer(data, size);
        datas.RewindRead(0);
        MessageParcel reply;
        MessageOption option;
        uint32_t inputCode = code % MAX_CODE;
        if (inputCode > 0) {
            DelayedSingleton<StandbyService>::GetInstance()->OnRemoteRequest(inputCode,
                datas, reply, option);
        }
        return true;
    }
} // namespace DevStandbyMgr
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    if (size < OHOS::DevStandbyMgr::U32_AT_SIZE) {
        return 0;
    }

    char* ch = (char *)malloc(size + 1);
    if (ch == nullptr) {
        return 0;
    }

    (void)memset_s(ch, size + 1, 0x00, size + 1);
    if (memcpy_s(ch, size, data, size) != EOK) {
        free(ch);
        ch = nullptr;
        return 0;
    }

    OHOS::DevStandbyMgr::DoSomethingInterestingWithMyAPI(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}
