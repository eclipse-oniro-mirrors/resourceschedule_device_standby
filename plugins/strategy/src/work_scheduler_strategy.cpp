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

#include "work_scheduler_strategy.h"
#include "standby_service_log.h"

namespace OHOS {
namespace DevStandbyMgr {
void WorkSchedulerStrategy::HandleEvent(const StandbyMessage& message)
{
    STANDBYSERVICE_LOGD("NetworkStrategy revceived message %{public}u, action: %{public}s",
        message.eventId_, message.action_.c_str());
}

ErrCode WorkSchedulerStrategy::OnCreated()
{
    return ERR_OK;
}

ErrCode WorkSchedulerStrategy::OnDestroy()
{
    return ERR_OK;
}
}  // namespace DevStandbyMgr
}  // namespace OHOS