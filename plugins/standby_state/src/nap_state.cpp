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

#include "nap_state.h"

#include <cmath>

#include "itimer_info.h"
#include "time_service_client.h"

#include "standby_service_log.h"
#include "standby_config_manager.h"
#include "iconstraint_manager_adapter.h"
#include "standby_service_impl.h"
#include "istate_manager_adapter.h"
#include "time_provider.h"

namespace OHOS {
namespace DevStandbyMgr {
NapState::NapState(uint32_t curState, uint32_t curPhase, const std::shared_ptr<IStateManagerAdapter>&
    stateManager, std::shared_ptr<AppExecFwk::EventHandler>& handler): BaseState(curState, curPhase,
    stateManager, handler)
{
    maintInterval_ = StandbyConfigManager::GetInstance()->GetStandbyDurationList(NAP_MAINT_DURATION);
    nextState_ = StandbyState::SLEEP;
}

ErrCode NapState::BeginState()
{
    auto stateManagerPtr = stateManager_.lock();
    if (!stateManagerPtr) {
        STANDBYSERVICE_LOGE("state manager adapter is nullptr");
        return ERR_STATE_MANAGER_IS_NULLPTR;
    }

    if (stateManagerPtr->GetPreState() == StandbyState::MAINTENANCE) {
        nextState_ = StandbyState::MAINTENANCE;
        int64_t maintIntervalTimeOut = CalculateMaintTimeOut(stateManagerPtr, false);
        STANDBYSERVICE_LOGI("after %{public}ld ms, enter maintenance state", maintIntervalTimeOut);
        if (maintIntervalTimeOut != 0) {
            StartStateTransitionTimer(maintIntervalTimeOut);
        }
        return ERR_OK;
    }

    nextState_ = StandbyState::SLEEP;
    maintIntervalIndex_ = 0;
    curPhase_ = NapStatePhase::CONNECTION;
    int64_t napTimeOut = std::min(TimeConstant::MSEC_PER_SEC * StandbyConfigManager::GetInstance()->
        GetStandbyParam(NAP_TIMEOUT), TimeProvider::GetNapTimeOut());
    STANDBYSERVICE_LOGD("napTimeOut is %{public}ld ms", napTimeOut);
    StartStateTransitionTimer(napTimeOut);
    BaseState::GetPhaseRunningLock()->Lock();
    handler_->PostTask([napState = shared_from_this()]() {
        napState->TransitToPhase(napState->curPhase_, napState->curPhase_ + 1);
        }, TRANSIT_NEXT_PHASE_INSTANT_TASK);
    return ERR_OK;
}

ErrCode NapState::EndState()
{
    StopTimedTask(TRANSIT_NEXT_STATE_TIMED_TASK);
    handler_->RemoveTask(TRANSIT_NEXT_STATE_TIMED_TASK);
    handler_->RemoveTask(TRANSIT_NEXT_PHASE_INSTANT_TASK);
    return ERR_OK;
}

bool NapState::CheckTransitionValid(uint32_t nextState)
{
    return true;
}

void NapState::EndEvalCurrentState(bool evalResult)
{
    if (curPhase_ == NapStatePhase::END) {
        HandleEvalResToSleepState(evalResult);
        return;
    }
    SetPhaseTransitTask(evalResult);
}

void NapState::SetPhaseTransitTask(bool evalResult)
{
    if (evalResult) {
        NextPhaseImpl(curPhase_, curPhase_ + 1);
    }
    curPhase_ += 1;
    if (curPhase_ < NapStatePhase::END) {
        handler_->PostTask([napState = shared_from_this()]() {
            napState->TransitToPhase(napState->curPhase_, napState->curPhase_ + 1);
            }, TRANSIT_NEXT_PHASE_INSTANT_TASK);
    } else {
        BaseState::GetPhaseRunningLock()->UnLock();
    }
}

void NapState::HandleEvalResToSleepState(bool evalResult)
{
    auto stateManagerPtr = stateManager_.lock();
    if (!stateManagerPtr) {
        BaseState::GetStateRunningLock()->UnLock();
        return;
    }
    if (evalResult) {
        stateManagerPtr->NextStateImpl(StandbyState::SLEEP);
    } else {
        STANDBYSERVICE_LOGD("constraint evalution failed, block current state");
        stateManagerPtr->BlockCurrentState();
        int64_t maintIntervalTimeOut = CalculateMaintTimeOut(stateManagerPtr, true);
        if (maintIntervalTimeOut > 0) {
            nextState_ = StandbyState::MAINTENANCE;
            StartStateTransitionTimer(maintIntervalTimeOut);
        }
    }
    BaseState::GetStateRunningLock()->UnLock();
}

bool NapState::IsInFinalPhase()
{
    return curPhase_ == NapStatePhase::END;
}
}  // namespace DevStandbyMgr
}  // namespace OHOS