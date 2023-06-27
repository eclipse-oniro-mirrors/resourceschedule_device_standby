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
#include "gtest/gtest.h"
#include "gtest/hwext/gtest-multithread.h"

#include "standby_service_log.h"
#include "json_utils.h"
#include "standby_config_manager.h"
#include "nlohmann/json.hpp"

#include "bundle_manager_helper.h"
#include "app_mgr_helper.h"
#include "ability_manager_helper.h"

using namespace testing::ext;
using namespace testing::mt;

namespace OHOS {
namespace DevStandbyMgr {
namespace {
    const std::string JSON_KEY = "key";
    const std::string JSON_ERROR_KEY = "error_key";
}
class StandbyUtilsUnitTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: StandbyUtilsUnitTest_001
 * @tc.desc: test AppMgrHelper.
 * @tc.type: FUNC
 * @tc.require: AR000HQ76V
 */
HWTEST_F(StandbyUtilsUnitTest, StandbyUtilsUnitTest_001, TestSize.Level1)
{
    std::vector<AppExecFwk::RunningProcessInfo> allAppProcessInfos;
    AppMgrHelper::GetInstance()->GetAllRunningProcesses(allAppProcessInfos);
    AppMgrHelper::GetInstance()->Connect();
    AppMgrHelper::GetInstance()->Connect();
    AppMgrHelper::GetInstance()->GetAllRunningProcesses(allAppProcessInfos);
    EXPECT_TRUE(allAppProcessInfos.empty());
}

/**
 * @tc.name: StandbyUtilsUnitTest_002
 * @tc.desc: test BundleManagerHelper.
 * @tc.type: FUNC
 * @tc.require: AR000HQ76V
 */
HWTEST_F(StandbyUtilsUnitTest, StandbyUtilsUnitTest_002, TestSize.Level1)
{
    int32_t uid {0};
    BundleManagerHelper::GetInstance()->GetClientBundleName(uid);
    AppExecFwk::ApplicationInfo applicationInfo {};
    BundleManagerHelper::GetInstance()->GetApplicationInfo("test",
        AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO, 0, applicationInfo);
    EXPECT_FALSE(applicationInfo.uid > 0);
}
}  // namespace DevStandbyMgr
}  // namespace OHOS