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
#include <WiFi.h>
#include "app_ha_connect.h"
#include "lgfx/v1/misc/enum.hpp"
#include "spdlog/spdlog.h"
#include <cstdint>
#include "../utils/theme/theme_define.h"

#include "lib/PubSubClient.h"

using namespace MOONCAKE::APPS;

#define _keyboard _data.hal->keyboard()
#define _canvas _data.hal->canvas()
#define _canvas_update _data.hal->canvas_update
#define _canvas_clear() _canvas->fillScreen(THEME_COLOR_BG)

TaskHandle_t loop_task_hander;

void AppHA::onCreate()
{
    spdlog::info("{} onCreate", getAppName());

    // Get hal
    _data.hal = mcAppGetDatabase()->Get("HAL")->value<HAL::Hal *>();
}

void AppHA::onResume()
{
    ANIM_APP_OPEN();

    _data.current_state = state_init;
}

void callback_bridge(char *topic, byte *payload, unsigned int length)
{
    AppHA::instance().callback(topic, payload, length);
}

void AppHA::callback(char *topic, byte *payload, unsigned int length)
{   
     char* payloadChar = reinterpret_cast<char*>(payload);
    _canvas_clear();
    _canvas->setBaseColor(THEME_COLOR_BG);
    _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
    _canvas->setFont(FONT_REPL);
    _canvas->setTextSize(FONT_SIZE_REPL);
    _canvas->setCursor(0, 0);
    _canvas->printf(payloadChar);
    _canvas_update();
}

static void loop_bridge(void*){
    spdlog::info("quit env");
}

void AppHA::onRunning()
{
    if (_data.current_state == state_init)
    {

        if (WiFi.status() == WL_CONNECTED)
        {

            _canvas_clear();
            _canvas->setBaseColor(THEME_COLOR_BG);
            _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
            _canvas->setFont(FONT_REPL);
            _canvas->setTextSize(FONT_SIZE_REPL);
            _canvas->setCursor(0, 0);
            _canvas->printf("WiFi connected");
            _canvas_update();

            const char *mqtt_server = "192.168.31.193";

            WiFiClient espClient;
            _data.client.setServer(mqtt_server, 1883);
            _data.client.setClient(espClient);
            _data.client.setCallback(callback_bridge);

            if (_data.client.connect("cardputer", "rr", "rr"))
            {
                _data.client.publish("cardputerOutTopic", "start MQTT");
                _data.client.subscribe("cardputerInTopic");
            }
            xTaskCreatePinnedToCore(loop_bridge, "decodeTask", 4096, &_data, 1, &loop_task_hander, APP_CPU_NUM);
            _data.current_state = state_connected;
        }
        else
        {

            _canvas_clear();
            _canvas->setBaseColor(THEME_COLOR_BG);
            _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
            _canvas->setFont(FONT_REPL);
            _canvas->setTextSize(FONT_SIZE_REPL);
            _canvas->setCursor(0, 0);
            _canvas->printf("WiFi not connected");
            _canvas_update();

            _data.current_state = state_init;
        }
    }
    else if (_data.current_state == state_connected)
    {
        
        if (millis() - _data._last_update > 20000)
        {

            if (_data.client.connected())
            {

                _canvas_clear();
                _canvas->setBaseColor(THEME_COLOR_BG);
                _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
                _canvas->setFont(FONT_REPL);
                _canvas->setTextSize(FONT_SIZE_REPL);
                _canvas->setCursor(0, 0);
                _canvas->printf("MQTT broker connected");
                _canvas_update();
                _data.client.publish("cardputerOutTopic", "Connected");
            }
            else
            {
                _canvas_clear();
                _canvas->setBaseColor(THEME_COLOR_BG);
                _canvas->setTextColor(THEME_COLOR_REPL_TEXT, THEME_COLOR_BG);
                _canvas->setFont(FONT_REPL);
                _canvas->setTextSize(FONT_SIZE_REPL);
                _canvas->setCursor(0, 0);
                _canvas->printf("MQTT broker not connected");
                _canvas_update();
                if (_data.client.connect("cardputer", "rr", "rr"))
                {
                    _data.client.publish("cardputerOutTopic", "Connected again");
                    _data.client.subscribe("cardputerInTopic");
                }
            }

            _data._last_update = millis();
        }
        // _data.client.loop();
    }

    if (_data.hal->homeButton()->pressed())
    {
        _data.hal->playNextSound();
        spdlog::info("quit env");
        destroyApp();
    }
}

void AppHA::onRunningBG()
{
    _data.client.loop();
}

void AppHA::onDestroy()
{
}
