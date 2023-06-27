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
#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_IBASE_STRATEGY_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_IBASE_STRATEGY_H

#include "standby_service_errors.h"
#include "standby_messsage.h"

namespace OHOS {
namespace DevStandbyMgr {
class IBaseStrategy {
public:
    virtual void HandleEvent(const StandbyMessage& message) = 0;
    virtual ErrCode OnCreated() = 0;
    virtual ErrCode OnDestroy() = 0;
    virtual ~IBaseStrategy() = default;
protected:
    int32_t policyId_ {0};
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_IBASE_STRATEGY_H
