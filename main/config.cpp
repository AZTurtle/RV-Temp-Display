#include "config.h"

const char* CONFIG_PATH = "/config.json";

cJSON* getJSONFromFile(const char* path) {
    cJSON* root = NULL;

    // Get SD card data
    File file = SD.open(path);
    if(!file){
        Serial.printf("Couldn't find file: %s", path);
    } else {
        String data = getFileContents(file);
        root = cJSON_Parse(data.c_str());

        file.close();
    }

    return root;
}

cJSON* getJSONFromFile(File& file) {
    cJSON* root = NULL;

    // Get SD card data
    String data = getFileContents(file);
    root = cJSON_Parse(data.c_str());

    return root;
}

String getFileContents(File& file) {
    String data = "";
    while(file.available()) {
        data += file.readString();
    }

    return data;
}

void writeJSONToFile(cJSON* json, const char* path) {
    File file = SD.open(path, FILE_WRITE);

    if(!file) {
        Serial.printf("Couldn't find file: %s", path);
    } else {
        char* str = cJSON_Print(json);

        file.print(str);
        cJSON_free(str);

        file.close();
    }
}

void createDefaultConfig() {
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "ssid", "");
    cJSON_AddStringToObject(root, "password", "");
    cJSON_AddStringToObject(root, "sp_email", "");
    cJSON_AddStringToObject(root, "sp_password", "");

    writeJSONToFile(root, CONFIG_PATH);

    cJSON_Delete(root);
}