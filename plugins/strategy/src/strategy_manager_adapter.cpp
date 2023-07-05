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

#include "strategy_manager_adapter.h"

#include <map>
#include <functional>
#include <string>
#include <memory>

#include "ibase_strategy.h"
#include "standby_service_log.h"
#include "network_strategy.h"
#include "timer_strategy.h"
#include "running_lock_strategy.h"
#include "work_scheduler_strategy.h"
#include "standby_config_manager.h"

namespace OHOS {
namespace DevStandbyMgr {

bool StrategyManagerAdapter::Init()
{
    if (StandbyConfigManager::GetInstance() == nullptr) {
        STANDBYSERVICE_LOGE("standby service ptr is nullptr, init failed");
        return false;
    }
    const auto& strategyConfigList = StandbyConfigManager::GetInstance()->GetStrategyConfigList();
    if (strategyConfigList.empty()) {
        STANDBYSERVICE_LOGI("strategies is disabled");
        return true;
    }
    RegisterPolicy(strategyConfigList);
    STANDBYSERVICE_LOGI("strategy manager plugin initialization succeed");
    return true;
}

void StrategyManagerAdapter::RegisterPolicy(const std::vector<std::string>& strategies)
{
    for (const auto& iter : strategies) {
        std::shared_ptr<IBaseStrategy> strategyPtr {nullptr};
        if (iter == "NET") {
            strategyPtr = std::make_shared<NetworkStrategy>();
        } else if (iter == "TIMER") {
            strategyPtr = std::make_shared<TimerStrategy>();
        } else if (iter == "RUNNING_LOCK") {
            strategyPtr = std::make_shared<RunningLockStrategy>();
        } else if(iter == "WORK_SCHEDULER") {
            strategyPtr = std::make_shared<WorkSchedulerStrategy>();
        } else {
            continue;
        }
        strategyList_.emplace_back(strategyPtr);
    }
}

void StrategyManagerAdapter::HandleEvent(const StandbyMessage& message)
{
    STANDBYSERVICE_LOGD("StrategyManagerAdapter revceive message %{public}u, action: %{public}s",
        message.eventId_, message.action_.c_str());
    for (const auto &strategy : strategyList_) {
        strategy->HandleEvent(message);
    }
}

void StrategyManagerAdapter::ShellDump(const std::vector<std::string>& argsInStr, std::string& result)
{
}
}  // namespace DevStandbyMgr
}  // namespace OHOS