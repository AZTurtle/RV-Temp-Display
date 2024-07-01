#pragma once

#include <cJSON.h>
#include <Arduino.h>
#include <lvgl.h>
#include <LilyGo_AMOLED.h>

extern const char* CONFIG_PATH;

cJSON* getJSONFromFile(const char* path);
cJSON* getJSONFromFile(File& file);
String getFileContents(File& file);
void writeJSONToFile(cJSON* json, const char* path);

void createDefaultConfig();