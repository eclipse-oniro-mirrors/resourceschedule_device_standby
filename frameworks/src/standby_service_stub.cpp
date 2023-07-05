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

#include "standby_service_stub.h"

#include <ipc_skeleton.h>
#include <string_ex.h>

#include "standby_service_subscriber_proxy.h"
#include "standby_service_errors.h"
#include "standby_service_log.h"

namespace OHOS {
namespace DevStandbyMgr {
const std::map<uint32_t, std::function<ErrCode(StandbyServiceStub *, MessageParcel& , MessageParcel& )>>
    StandbyServiceStub::interfaces_ = {
        {StandbyServiceStub::SUBSCRIBE_STANDBY_CALLBACK,
            std::bind(&StandbyServiceStub::HandleSubscribeStandbyCallback,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {StandbyServiceStub::UNSUBSCRIBE_STANDBY_CALLBACK,
            std::bind(&StandbyServiceStub::HandleUnsubscribeStandbyCallback,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {StandbyServiceStub::APPLY_ALLOW_RESOURCE,
            std::bind(&StandbyServiceStub::HandleApplyAllowResource,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {StandbyServiceStub::UNAPPLY_ALLOW_RESOURCE,
            std::bind(&StandbyServiceStub::HandleUnapplyAllowResource,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {StandbyServiceStub::GET_ALLOW_LIST,
            std::bind(&StandbyServiceStub::HandleGetAllowList,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
        {StandbyServiceStub::IS_DEVICE_IN_STANDBY,
            std::bind(&StandbyServiceStub::HandleIsDeviceInStandby,
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)},
};

ErrCode StandbyServiceStub::OnRemoteRequest(uint32_t code,
    MessageParcel& data, MessageParcel& reply, MessageOption& option)
{
    std::u16string descriptor = StandbyServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        STANDBYSERVICE_LOGE("StandbyServiceStub: Local descriptor not match remote.");
        return ERR_TRANSACTION_FAILED;
    }
    auto it = interfaces_.find(code);
    if (it == interfaces_.end()) {
        return IRemoteStub<IStandbyService>::OnRemoteRequest(code, data, reply, option);
    }

    auto fun = it->second;
    if (fun == nullptr) {
        return IRemoteStub<IStandbyService>::OnRemoteRequest(code, data, reply, option);
    }

    ErrCode result = fun(this, data, reply);
    if (SUCCEEDED(result)) {
        return ERR_OK;
    }

    STANDBYSERVICE_LOGW("StandbyServiceStub: Failed to call interface %{public}u, err:%{public}d", code, result);
    return result;
}

ErrCode StandbyServiceStub::HandleSubscribeStandbyCallback(MessageParcel& data, MessageParcel& reply)
{
    sptr<IRemoteObject> subscriber = data.ReadRemoteObject();
    if (subscriber == nullptr) {
        STANDBYSERVICE_LOGW("HandleSubscribeStandbyCallback Read callback fail.");
        return ERR_STANDBY_PARCELABLE_FAILED;
    }

    ErrCode result = SubscribeStandbyCallback(iface_cast<IStandbyServiceSubscriber>(subscriber));
    if (!reply.WriteInt32(result)) {
        STANDBYSERVICE_LOGW("HandleSubscribeStandbyCallback Write result failed, ErrCode=%{public}d", result);
        return ERR_STANDBY_PARCELABLE_FAILED;
    }
    return ERR_OK;
}

ErrCode StandbyServiceStub::HandleUnsubscribeStandbyCallback(MessageParcel& data, MessageParcel& reply)
{
    sptr<IRemoteObject> subscriber = data.ReadRemoteObject();
    if (subscriber == nullptr) {
        STANDBYSERVICE_LOGW("HandleUnsubscribeStandbyCallback Read callback fail.");
        return ERR_STANDBY_PARCELABLE_FAILED;
    }

    ErrCode result = UnsubscribeStandbyCallback(iface_cast<IStandbyServiceSubscriber>(subscriber));
    if (!reply.WriteInt32(result)) {
        STANDBYSERVICE_LOGW("HandleUnsubscribeStandbyCallback Write result failed, ErrCode=%{public}d", result);
        return ERR_STANDBY_PARCELABLE_FAILED;
    }
    return ERR_OK;
}

ErrCode StandbyServiceStub::HandleApplyAllowResource(MessageParcel& data, MessageParcel& reply)
{
    auto resourceRequest = ResourceRequest::Unmarshalling(data);
    ErrCode result = ApplyAllowResource(resourceRequest);
    if (!reply.WriteInt32(result)) {
        STANDBYSERVICE_LOGW("HandleApplyAllowResource Write result failed, ErrCode=%{public}d", result);
        return ERR_STANDBY_PARCELABLE_FAILED;
    }
    return ERR_OK;
}

ErrCode StandbyServiceStub::HandleUnapplyAllowResource(MessageParcel& data, MessageParcel& reply)
{
    auto resourceRequest = ResourceRequest::Unmarshalling(data);
    ErrCode result = UnapplyAllowResource(resourceRequest);
    if (!reply.WriteInt32(result)) {
        STANDBYSERVICE_LOGW("HandleUnapplyAllowResource Write result failed, ErrCode=%{public}d", result);
        return ERR_STANDBY_PARCELABLE_FAILED;
    }
    return ERR_OK;
}

ErrCode StandbyServiceStub::HandleGetAllowList(MessageParcel& data, MessageParcel& reply)
{
    uint32_t allowType = data.ReadUint32();
    uint32_t reasonCode = data.ReadUint32();
    std::vector<AllowInfo> allowInfoList {};
    ErrCode result = GetAllowList(allowType, allowInfoList, reasonCode);
    if (!reply.WriteInt32(result)) {
        STANDBYSERVICE_LOGW("HandleGetAllowList Write result failed, ErrCode=%{public}d", result);
        return ERR_STANDBY_PARCELABLE_FAILED;
    }
    if (!reply.WriteUint32(allowInfoList.size())) {
        STANDBYSERVICE_LOGW("HandleGetAllowList Write result size failed");
        return ERR_STANDBY_PARCELABLE_FAILED;
    }
    for (auto& info : allowInfoList) {
        if (!info.Marshalling(reply)) {
            return ERR_STANDBY_PARCELABLE_FAILED;
        }
    }
    return ERR_OK;
}

ErrCode StandbyServiceStub::HandleIsDeviceInStandby(MessageParcel& data, MessageParcel& reply)
{
    bool isStandby {false};
    ErrCode result = IsDeviceInStandby(isStandby);
    if (!reply.WriteInt32(result)) {
        STANDBYSERVICE_LOGW("HandleIsDeviceInStandby Write result failed, ErrCode=%{public}d", result);
        return ERR_STANDBY_PARCELABLE_FAILED;
    }
    if (!reply.WriteBool(isStandby)) {
        STANDBYSERVICE_LOGW("HandleIsDeviceInStandby Write isStandby failed");
        return ERR_STANDBY_PARCELABLE_FAILED;
    }
    return ERR_OK;
}
}  // namespace DevStandbyMgr
}  // namespace OHOS