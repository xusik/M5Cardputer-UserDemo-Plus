/**
 * @file i2c_scan.cpp
 * @author xusik
 * @brief
 * @version 0.6
 * @date 2024-02-04
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <Arduino.h>
#include <Wire.h>
#include "app_i2c_scan.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/spdlog.h"
#include <cstdint>
#include "../utils/theme/theme_define.h"

using namespace MOONCAKE::APPS;


#define _keyboard _data.hal->keyboard()
#define _canvas _data.hal->canvas()
#define _canvas_update _data.hal->canvas_update
#define _canvas_clear() _canvas->fillScreen(THEME_COLOR_BG)


void AppI2C::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal*>();
}


void AppI2C::onResume()
{
    ANIM_APP_OPEN();

    _data.current_state = state_init;
}


void AppI2C::onRunning()
{
    if (_data.current_state == state_init) {
        _canvas_clear();
        _canvas->setBaseColor(THEME_COLOR_BG);
        _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
        _canvas->setFont(FONT_REPL);
        _canvas->setTextSize(FONT_SIZE_REPL);
        _canvas->setCursor(0, 0);
        _canvas->printf("not connected");
        _canvas_update();

        Wire.begin(2, 1);
        _data.current_state = state_connected;
        _data.hal->playNextSound();


    } else if (_data.current_state == state_connected) {

        byte error, address;
        int nDevices;

        nDevices = 0;
        _canvas_clear();
        _canvas->setBaseColor(THEME_COLOR_BG);
        _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
        _canvas->setFont(FONT_REPL);
        _canvas->setTextSize(FONT_SIZE_REPL);
        _canvas->setCursor(0, 0);
        _canvas->printf("dev found: "); 
        _canvas->setTextSize(1);
        for (address = 1; address < 127; address++ )
        {
            // The i2c_scanner uses the return value of
            // the Write.endTransmisstion to see if
            // a device did acknowledge to the address.
            Wire.beginTransmission(address);
            error = Wire.endTransmission();

            if (error == 0)
            {

            if (address < 16) {
                spdlog::info("0");
                spdlog::info("0");
            }
            spdlog::info(address);


            _canvas->printf("%x\n", address);
            _canvas_update();

            nDevices++;
            }
            else if (error == 4)
            {
            spdlog::info("Unknown error at address 0x");
            if (address < 16) {
                spdlog::info("0");
            }
            spdlog::info(address);
            }
        }
        if (nDevices == 0) {
            _canvas_clear();
            _canvas->setBaseColor(THEME_COLOR_BG);
            _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
            _canvas->setFont(FONT_REPL);
            _canvas->setTextSize(FONT_SIZE_REPL);
            _canvas->setCursor(0, 0);
            _canvas->printf("No i2c device found \n");
            _canvas_update();
        }
    }

    if (_data.hal->homeButton()->pressed())
    {
        _data.hal->playNextSound();
        spdlog::info("quit env");
        destroyApp();
    }
}


void AppI2C::onDestroy()
{

}
