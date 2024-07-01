#pragma once

#include <lvgl.h>
#include <time.h>
#include <Arduino.h>

#define LOG_SIZE 256

#define LOG_MSG 0x1005

class Log {

String data[LOG_SIZE];
int dataNum = 0;

public:
    static Log& get() {
        static Log instance;

        return instance;
    }

    void createView(lv_obj_t* parent);

    void print(String msg);
    inline void print(double msg) {print(String(msg));}
    inline void println(String msg) {print(msg + "\n");}
    inline void println(double msg) {print(String(msg) + "\n");}
private:
    Log();

    void shift();

    String getDataString();
    static void updateLog(lv_event_t* e) {
        lv_obj_t *label = (lv_obj_t *)lv_event_get_target(e);
        Log* log = (Log*)lv_event_get_user_data(e);
        lv_label_set_text(label, log->getDataString().c_str());
        lv_obj_scroll_to_y(label, LV_COORD_MAX, LV_ANIM_OFF);
    }
public:
    Log(Log const&) = delete;
    void operator=(Log const&) = delete;
};