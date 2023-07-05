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

#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_CONSTRAINTS_INCLUDE_DEFAULT_CONSTRAINT_ADAPTER_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_CONSTRAINTS_INCLUDE_DEFAULT_CONSTRAINT_ADAPTER_H

#include "iconstraint_manager_adapter.h"
#include "standby_service_errors.h"
#include "standby_messsage.h"

namespace OHOS {
namespace DevStandbyMgr {
struct ConstraintEvalParam;
class IStateManagerAdapter;
class ConstraintManagerAdapter : public IConstraintManagerAdapter {
public:
    ConstraintManagerAdapter() = default;
    ~ConstraintManagerAdapter() override = default;
    bool Init() override;
    bool UnInit() override;
    ErrCode StartEvalution(const ConstraintEvalParam& params) override;
    ErrCode StopEvalution() override;
    void RegisterConstraintCallback(const ConstraintEvalParam& params, const std::shared_ptr<
        IConstraintMonitor>& monitor) override;
    void ShellDump(const std::vector<std::string>& argsInStr, std::string& result) override;

private:
    bool isEvaluation_ {false};
    std::shared_ptr<IConstraintMonitor> motionConstraint_ {nullptr};
    std::shared_ptr<IConstraintMonitor> repeatedMotionConstraint_ {nullptr};
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_PLUGINS_CONSTRAINTS_INCLUDE_DEFAULT_CONSTRAINT_ADAPTER_H
