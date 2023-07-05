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

#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_ABILITY_MGR_HELPER_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_ABILITY_MGR_HELPER_H

#ifdef STANDBY_SERVICE_UNIT_TEST
#define WEAK_FUNC __attribute__((weak))
#else
#define WEAK_FUNC
#endif

#include "if_system_ability_manager.h"
#include "system_ability_manager_proxy.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "single_instance.h"

namespace OHOS {
namespace DevStandbyMgr {
class AbilityManagerHelper {
DECLARE_SINGLE_INSTANCE(AbilityManagerHelper);
public:
    bool GetRunningSystemProcess(std::list<SystemProcessInfo>& systemProcessInfos);
private:
    sptr<ISystemAbilityManager> abilityManagerProxy_ {nullptr};
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_ABILITY_MGR_HELPER_H