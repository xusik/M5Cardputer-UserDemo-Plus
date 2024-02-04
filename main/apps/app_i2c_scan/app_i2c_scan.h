/**
 * @file i2c_scan.h
 * @author xusik
 * @brief
 * @version 0.6
 * @date 2024-02-04
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

#include "assets/i2c_big.h"
#include "assets/i2c_small.h"


namespace MOONCAKE
{
    namespace APPS
    {
        class AppI2C : public APP_BASE
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

        class AppI2C_Packer : public APP_PACKER_BASE
        {
            std::string getAppName() override { return "I2C Scan"; }
            void* getAppIcon() override { return (void*)(new AppIcon_t(image_data_i2c_big, image_data_i2c_small)); }
            void* newApp() override { return new AppI2C; }
            void deleteApp(void *app) override { delete (AppI2C*)app; }
        };
    }
}
