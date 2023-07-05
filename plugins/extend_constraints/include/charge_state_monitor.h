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

#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_CONSTRAINTS_INCLUDE_CHARGE_STATE_CONSTRAINT_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_CONSTRAINTS_INCLUDE_CHARGE_STATE_CONSTRAINT_H

#include <memory>
#include "iconstraint_monitor.h"

namespace OHOS {
namespace DevStandbyMgr {
class ChargeStateMonitor : public IConstraintMonitor,
    public std::enable_shared_from_this<ChargeStateMonitor> {
public:
    bool Init() override;
    void StartMonitoring() override;
    void StopMonitoring() override;

private:
    std::shared_ptr<AppExecFwk::EventHandler> handler_ {nullptr};
};
} // DevStandbyMgr
} // OHOS
#endif // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_CONSTRAINTS_INCLUDE_CHARGE_STATE_CONSTRAINT_H
