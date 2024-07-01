/**
 * @file      gui.h
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-07-14
 *
 */

#pragma once

#include <lvgl.h>
#include <time.h>
#include <Arduino.h>
#include "log.h"
#include <WiFi.h>
#include <cJSON.h>
#include <Adafruit_NeoPixel.h>
#include <LilyGo_AMOLED.h>
#include "config.h"

void drawGUI(void);

#define COIN_MSG_ID             0x1000
#define WIFI_MSG_ID             0x1001
#define TEMPERATURE_MSG_ID      0x1002
#define WEATHER_MSG_ID          0x1003
#define SENSORPUSH_MSG_ID       0x1004
#define WIFI_SCAN_DONE          0x1006
#define HIDE_KEYBOARD           0x1007
#define WIFI_CONNECTED          0x1008
#define WIFI_FAILED             0x1009
#define WIFI_TIMEOUT            0x1010
#define OPEN_GRAPH              0x1011
#define UPDATE_GRAPH            0x1012

#define SENSOR_NUM 5
#define GRAPH_RES 50
#define GRAPH_W 325
#define GRAPH_H 325
#define GRAPH_PADDING 10

typedef struct __CoinMarketCapApiSubsribe {
    int id;
    const lv_img_dsc_t *src_img;
    const char *name;
} CoinMarketCapApiSubsribe;

typedef struct __CoinMarketCapApiDataStream {
    int id;
    double price;
    double percent_change_1h;
    double percent_change_24h;
    double percent_change_7d;
} CoinMarketCapApiDataStream;

typedef struct  __OpenWeatherMapApi {
    String city;
    String description;
    double temperature;
    double temp_min;
    double temp_max;
    double pressure;
    double humidity;
} OpenWeatherMapApi;

typedef struct __Sample {
    double temp;
    double humidity;
} Sample;

typedef struct __SensorData {
    const char* name;
    String id;
    double temp;
    double batt;
    Sample sample[GRAPH_RES];
} SensorData;


typedef struct __SensorPushApi {
    String last_time;
    SensorData sensors[SENSOR_NUM] = {
        SensorData{"RV Outdoor"},
        SensorData{"RV Refrigerator"},
        SensorData{"RV Tank Bay"},
        SensorData{"RV Freezer"},
        SensorData{"RV Interior"}
    };
} SensorPushApi;

void selectNextItem();
void showCertification(uint32_t delay_ms);

extern String WiFi_SSID;
extern String sensorpushEmail;
extern String sensorpushPsswd;