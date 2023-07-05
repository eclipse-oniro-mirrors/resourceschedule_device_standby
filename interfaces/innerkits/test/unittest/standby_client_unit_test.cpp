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
#include <functional>
#include <chrono>
#include <thread>
#include <message_parcel.h>

#include "gtest/gtest.h"
#include "singleton.h"

#include "allow_type.h"
#include "standby_service_client.h"
#include "standby_service_subscriber_stub.h"
using namespace testing::ext;

namespace OHOS {
namespace DevStandbyMgr {

class StandbyServiceClientUnitTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: StandbyServiceClientUnitTest_001
 * @tc.desc: test SubscribeStandbyCallback.
 * @tc.type: FUNC
 * @tc.require: AR000HQ6GA
 */
HWTEST_F(StandbyServiceClientUnitTest, StandbyServiceClientUnitTest_001, TestSize.Level1)
{
    sptr<IStandbyServiceSubscriber> nullSubscriber = nullptr;
    EXPECT_NE(StandbyServiceClient::GetInstance().SubscribeStandbyCallback(nullSubscriber), ERR_OK);
    EXPECT_NE(StandbyServiceClient::GetInstance().SubscribeStandbyCallback(nullSubscriber), ERR_OK);
    sptr<IStandbyServiceSubscriber> subscriber = new (std::nothrow) StandbyServiceSubscriberStub();
    EXPECT_EQ(StandbyServiceClient::GetInstance().SubscribeStandbyCallback(subscriber), ERR_OK);
    StandbyServiceClient::GetInstance().SubscribeStandbyCallback(subscriber);
    EXPECT_EQ(StandbyServiceClient::GetInstance().UnsubscribeStandbyCallback(subscriber), ERR_OK);
    EXPECT_NE(StandbyServiceClient::GetInstance().UnsubscribeStandbyCallback(subscriber), ERR_OK);
}

/**
 * @tc.name: StandbyServiceClientUnitTest_002
 * @tc.desc: test ApplyAllowResource.
 * @tc.type: FUNC
 * @tc.require: AR000HQ6GA
 */
HWTEST_F(StandbyServiceClientUnitTest, StandbyServiceClientUnitTest_002, TestSize.Level1)
{
    sptr<ResourceRequest> nullRequest = nullptr;
    EXPECT_NE(StandbyServiceClient::GetInstance().ApplyAllowResource(nullRequest), ERR_OK);
    EXPECT_NE(StandbyServiceClient::GetInstance().UnapplyAllowResource(nullRequest), ERR_OK);

    sptr<ResourceRequest> resouarceRequest = new (std::nothrow) ResourceRequest();
    EXPECT_NE(StandbyServiceClient::GetInstance().ApplyAllowResource(resouarceRequest), ERR_OK);
    EXPECT_NE(StandbyServiceClient::GetInstance().UnapplyAllowResource(resouarceRequest), ERR_OK);

    sptr<ResourceRequest> validResRequest = new (std::nothrow) ResourceRequest(AllowType::NET,
        0, "test_process", 100, "test", 1);
    EXPECT_EQ(StandbyServiceClient::GetInstance().ApplyAllowResource(validResRequest), ERR_OK);
    EXPECT_EQ(StandbyServiceClient::GetInstance().UnapplyAllowResource(validResRequest), ERR_OK);
}

/**
 * @tc.name: StandbyServiceClientUnitTest_003
 * @tc.desc: test GetAllowList.
 * @tc.type: FUNC
 * @tc.require: AR000HQ6GA
 */
HWTEST_F(StandbyServiceClientUnitTest, StandbyServiceClientUnitTest_003, TestSize.Level1)
{
    std::vector<AllowInfo> allowInfoList;
    sptr<ResourceRequest> nullRequest = nullptr;
    EXPECT_EQ(StandbyServiceClient::GetInstance().GetAllowList(AllowType::NET, allowInfoList, 0), ERR_OK);
    EXPECT_NE(StandbyServiceClient::GetInstance().GetAllowList(0, allowInfoList, 0), ERR_OK);
    EXPECT_EQ(StandbyServiceClient::GetInstance().GetAllowList((1 << 6), allowInfoList, 0), ERR_OK);
}

/**
 * @tc.name: StandbyServiceClientUnitTest_004
 * @tc.desc: test IsDeviceInStandby.
 * @tc.type: FUNC
 * @tc.require: AR000HQ6GA
 */
HWTEST_F(StandbyServiceClientUnitTest, StandbyServiceClientUnitTest_004, TestSize.Level1)
{
    bool isStandby {false};
    EXPECT_EQ(StandbyServiceClient::GetInstance().IsDeviceInStandby(isStandby), ERR_OK);
}
}  // namespace DevStandbyMgr
}  // namespace OHOS
