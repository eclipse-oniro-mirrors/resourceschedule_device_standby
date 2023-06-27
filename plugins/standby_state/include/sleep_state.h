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
#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_SLEEP_STATE_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_SLEEP_STATE_H

#include "base_state.h"

namespace OHOS {
namespace DevStandbyMgr {
class IStateManagerAdapter;
class SleepState : public BaseState, public StateWithMaint, public std::enable_shared_from_this<SleepState> {
public:
    SleepState(uint32_t curState, uint32_t curPhase, const std::shared_ptr<IStateManagerAdapter>&
        stateManager, std::shared_ptr<AppExecFwk::EventHandler>& handler);
    ErrCode Init(const std::shared_ptr<BaseState>& statePtr) override;
    ErrCode UnInit() override;
    ErrCode BeginState() override;
    ErrCode EndState() override;
    bool CheckTransitionValid(uint32_t nextState) override;
    void EndEvalCurrentState(bool evalResult) override;
    void ShellDump(const std::vector<std::string>& argsInStr, std::string& result) override;
protected:
    bool IsInFinalPhase() override;
    virtual void CheckScrenOffHalfHour();
private:
    void StartPeriodlyMotionDetection();
    void TryToEnterNextPhase(const std::shared_ptr<IStateManagerAdapter>& stateManagerPtr,
        int32_t retryTimeOut);
    void SetPhaseTransitOrRepeatedTask();
private:
    bool isRepeatedDetection_ {false};
    uint64_t repeatedDetectionTimerId_ {0};
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_SERVICES_CORE_INCLUDE_SLEEP_STATE_H