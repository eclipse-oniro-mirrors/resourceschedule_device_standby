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

#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_ICONSTRAINT_MANAGER_ADAPTER_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_ICONSTRAINT_MANAGER_ADAPTER_H

#include <vector>
#include <map>
#include <memory>
#include <utility>

#include "standby_messsage.h"
#include "standby_service_errors.h"
#include "iconstraint_monitor.h"
#include "errors.h"

namespace OHOS {
namespace DevStandbyMgr {
struct ConstraintEvalParam;
class IStateManagerAdapter;
class IConstraintManagerAdapter {
public:
    virtual bool Init() = 0;
    virtual bool UnInit() = 0;
    virtual ErrCode StartEvalution(const ConstraintEvalParam& params) = 0;
    virtual ErrCode StopEvalution() = 0;
    virtual void RegisterConstraintCallback(const ConstraintEvalParam& params,
        const std::shared_ptr<IConstraintMonitor>& monitor) = 0;
    virtual void ShellDump(const std::vector<std::string>& argsInStr, std::string& result) = 0;
    virtual ~IConstraintManagerAdapter() = default;
protected:
    std::vector<std::shared_ptr<IConstraintMonitor>> constraintMonitorList_ {};
    std::map<uint32_t, std::shared_ptr<IConstraintMonitor>> constraintMap_ {};
    std::weak_ptr<IStateManagerAdapter> stateManager_ {};
    std::shared_ptr<IConstraintMonitor> curMonitor_ {nullptr};
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_EXT_INCLUDE_ICONSTRAINT_MANAGER_ADAPTER_H
