/**
 * @file app_env.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.6
 * @date 2023-09-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <Arduino.h>
#include "app_env.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/spdlog.h"
#include <cstdint>
#include "../utils/theme/theme_define.h"

#include "lib/Adafruit_BME280.h"
#include "lib/Adafruit_Sensor.h"


using namespace MOONCAKE::APPS;


#define _keyboard _data.hal->keyboard()
#define _canvas _data.hal->canvas()
#define _canvas_update _data.hal->canvas_update
#define _canvas_clear() _canvas->fillScreen(THEME_COLOR_BG)


void AppENV::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal*>();
}


void AppENV::onResume()
{
    ANIM_APP_OPEN();

    _data.current_state = state_init;
}


void AppENV::onRunning()
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

        if (_data.bmp.begin(0x76)) {
            _data.bmp.setSampling(Adafruit_BME280::MODE_FORCED,   // 模式：正常
                Adafruit_BME280::SAMPLING_X2,       // 温度采样率：2倍
                Adafruit_BME280::SAMPLING_X16,      // 压力采样率：16倍
                Adafruit_BME280::SAMPLING_X1, // humidity
                Adafruit_BME280::FILTER_X16,        // 滤波器：16倍
                Adafruit_BME280::STANDBY_MS_500);   // 等待时间：500毫秒

            spdlog::info("bmp begin");
            _data.current_state = state_connected;
            _data.hal->playNextSound();

        }

    } else if (_data.current_state == state_connected) {
        if (millis() - _data._last_update > 2000) {
            bool error = false;

            float temperature, humidity, pressure;
            temperature = _data.bmp.readTemperature();
            humidity = _data.bmp.readHumidity();
            pressure = _data.bmp.readPressure();


            _canvas_clear();
            _canvas->setBaseColor(THEME_COLOR_BG);
            _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
            _canvas->setFont(FONT_REPL);
            _canvas->setTextSize(FONT_SIZE_REPL);
            _canvas->setCursor(0, 0);
            _canvas->printf("connected\n\n");
            _canvas->setTextSize(2);
            _canvas->printf("%.3f °C\n", temperature);
            _canvas->setTextSize(1);
            _canvas->printf("Humidity: %.3f %%\n", humidity);
            _canvas->printf("Pressure: %.6f kPa\n", pressure / 1000);
            _canvas_update();

            if (error) {
                _data.current_state = state_init;
                _data.hal->playNextSound();
            }

            _data._last_update = millis();
        }
    }

    if (_data.hal->homeButton()->pressed())
    {
        _data.hal->playNextSound();
        spdlog::info("quit env");
        destroyApp();
    }
}


void AppENV::onDestroy()
{

}
