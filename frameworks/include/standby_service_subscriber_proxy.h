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

#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_FRAMEWORKS_INCLUDE_STANDBY_SERVICE_SUBSCRIBER_PROXY_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_FRAMEWORKS_INCLUDE_STANDBY_SERVICE_SUBSCRIBER_PROXY_H

#include <iremote_proxy.h>

#include "istandby_service_subscriber.h"

namespace OHOS {
namespace DevStandbyMgr {
class StandbyServiceSubscriberProxy : public IRemoteProxy<IStandbyServiceSubscriber> {
public:
    StandbyServiceSubscriberProxy() = delete;
    explicit StandbyServiceSubscriberProxy(const sptr<IRemoteObject>& impl);
    ~StandbyServiceSubscriberProxy() override;
    DISALLOW_COPY_AND_MOVE(StandbyServiceSubscriberProxy);

    /**
     * @brief Called back when the standby state changed.
     *
     * @param napped The device is in the nap mode.
     * @param sleeping The device is in the sleeping mode.
     */
    void OnDeviceIdleMode(bool napped, bool sleeping) override;

    /**
     * @brief report change of allow list to subscriber.
     *
     * @param uid uid which changed happens in.
     * @param name process name of uid.
     * @param allowType The change of the chang.
     * @param added add or removed.
     */
    void OnAllowListChanged(int32_t uid, const std::string& name, uint32_t allowType, bool added) override;
private:
    static inline BrokerDelegator<StandbyServiceSubscriberProxy> delegator_;
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICE_FRAMEWORKS_INCLUDE_STANDBY_SERVICE_SUBSCRIBER_PROXY_H