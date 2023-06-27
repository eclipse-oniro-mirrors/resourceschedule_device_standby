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

#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_IMESSAGE_LISTENER_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_IMESSAGE_LISTENER_H

#include "standby_service_errors.h"

namespace OHOS {
namespace DevStandbyMgr {
class IStateManagerAdapter;
class IMesssageListener {
public:
    virtual ErrCode StartListener() = 0;
    virtual ErrCode StopListener() = 0;
    virtual ~IMesssageListener() = default;
protected:
    std::shared_ptr<IStateManagerAdapter> standbyStateManager_ {nullptr};
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_IMESSAGE_LISTENER_H
