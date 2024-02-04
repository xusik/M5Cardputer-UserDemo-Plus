/**
 * @file app_env.h
 * @author Wu23333
 * @brief
 * @version 0.1
 * @date 2024-01-09
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <mooncake.h>
#include "../../hal/hal.h"
#include "../utils/theme/theme_define.h"
#include "../utils/anim/anim_define.h"
#include "../utils/icon/icon_define.h"

#include "assets/ha_big.h"
#include "assets/ha_small.h"


namespace MOONCAKE
{
    namespace APPS
    {
        class AppHA : public APP_BASE
        {
            private:
                enum State_t
                {
                    state_init = 0,
                    state_connected,
                };

                struct Data_t
                {
                    HAL::Hal* hal = nullptr;
                    State_t current_state = state_init;
                    int64_t _last_update = 0;
                };
                Data_t _data;

            public:
                void onCreate() override;
                void onResume() override;
                void onRunning() override;
                void onDestroy() override;
        };

        class AppHA_Packer : public APP_PACKER_BASE
        {
            std::string getAppName() override { return "HA Connect"; }
            void* getAppIcon() override { return (void*)(new AppIcon_t(image_data_ha_big, image_data_ha_small)); }
            void* newApp() override { return new AppHA; }
            void deleteApp(void *app) override { delete (AppHA*)app; }
        };
    }
}
