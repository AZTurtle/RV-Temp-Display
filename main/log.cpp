#include "log.h"

Log::Log(){}

void Log::print(String msg) {
    shift();

    data[0] = msg;
    Serial.print(msg);

    lv_msg_send(LOG_MSG, NULL);
}

void Log::shift() {
    dataNum++;
    if(dataNum > LOG_SIZE) {
        dataNum = LOG_SIZE - 1;
    }
    for (int i = dataNum; i > 0; i--){
        data[i] = data[i - 1];
    }
}

String Log::getDataString() {
    String res = "";
    for (int i = dataNum - 1; i >= 0; i--) {
        res += data[i];
    }

    return res;
}

void Log::createView(lv_obj_t* parent) {
    lv_obj_t * txt = lv_label_create(parent);
    lv_label_set_text(txt, getDataString().c_str());
    lv_obj_set_width(txt, LV_PCT(100));
    lv_obj_scroll_to_y(txt, LV_COORD_MAX, LV_ANIM_OFF);
    lv_msg_subsribe_obj(LOG_MSG, txt, NULL);
    lv_obj_add_event_cb(txt, updateLog, LV_EVENT_MSG_RECEIVED, this);
}