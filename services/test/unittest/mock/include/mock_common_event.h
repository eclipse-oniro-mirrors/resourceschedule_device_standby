/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICES_TEST_MOCK_COMMON_EVENT_H
#define FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICES_TEST_MOCK_COMMON_EVENT_H
namespace OHOS {
namespace DevStandbyMgr {
extern int g_mockFunctionCallCount;
class MockCommonEvent {
public:
    MockCommonEvent();
    virtual ~MockCommonEvent();
    static void MockPublishCommonEvent(bool mockRet);
    static void MockSubscribeCommonEvent(bool mockRet);
};

class MockUtils {
public:
    MockUtils() {};
    ~MockUtils() {};
    static int32_t MockGetSingleExtConfigFunc(int32_t index, stad::string &config);
};
}  // namespace DevStandbyMgr
}  // namespace OHOS
#endif //FOUNDATION_RESOURCESCHEDULE_STANDBY_SERVICES_TEST_MOCK_COMMON_EVENT_H
