/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "allow_info.h"
#include "ipc_util.h"
#include "standby_service_log.h"

namespace OHOS {
namespace DevStandbyMgr {
bool AllowInfo::Marshalling(Parcel& out) const
{
    WRITE_PARCEL_WITH_RET(out, Uint32, allowType_, false);
    WRITE_PARCEL_WITH_RET(out, String, name_, false);
    WRITE_PARCEL_WITH_RET(out, Int32, duration_, false);
    return true;
}

sptr<AllowInfo> AllowInfo::Unmarshalling(Parcel& in)
{
    sptr<AllowInfo> info = new (std::nothrow) AllowInfo();
    if (info != nullptr && !info->ReadFromParcel(in)) {
        STANDBYSERVICE_LOGE("read from parcel failed");
        info = nullptr;
    }
    return info;
}

bool AllowInfo::ReadFromParcel(Parcel& in)
{
    READ_PARCEL_WITH_RET(in, Uint32, allowType_, false);
    READ_PARCEL_WITH_RET(in, String, name_, false);
    READ_PARCEL_WITH_RET(in, Int32, duration_, false);
    return true;
}
}  // namespace DevStandbyMgr
}  // namespace OHOS