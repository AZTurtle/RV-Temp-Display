/**
 * @file      gui.cpp
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-07-13
 *
 */

#include "gui.h"

extern LilyGo_Class amoled;

LV_FONT_DECLARE(alibaba_font_48);
LV_FONT_DECLARE(alibaba_font_18);

LV_FONT_DECLARE(font_ali_70);

LV_IMG_DECLARE(icon_cloudy_sun);
LV_IMG_DECLARE(icon_sun);
LV_IMG_DECLARE(icon_cloudy);
LV_IMG_DECLARE(icon_thunderstorm);
LV_IMG_DECLARE(gif_rabbit);

LV_IMG_DECLARE(icon_battery);
LV_IMG_DECLARE(icon_cpu);
LV_IMG_DECLARE(icon_flash);
LV_IMG_DECLARE(icon_ram);
LV_IMG_DECLARE(icon_light_sensor);
LV_IMG_DECLARE(icon_usb);
LV_IMG_DECLARE(icon_micro_sd);

LV_IMG_DECLARE(ico_ethereum);
LV_IMG_DECLARE(icon_tether);
LV_IMG_DECLARE(icon_xrp);
LV_IMG_DECLARE(icon_bitcoin);

LV_IMG_DECLARE(icon_humidity);
LV_IMG_DECLARE(img_certification_amoled_191_tp);
LV_IMG_DECLARE(img_certification_t4_s3_241_tp);

LV_IMG_DECLARE(img_rv);
LV_IMG_DECLARE(img_refresh);

static lv_color_t green = LV_COLOR_MAKE(0, 255, 0);
static lv_color_t dim_green = LV_COLOR_MAKE(20, 170, 20);

String WiFi_SSID;
String sensorpushEmail;
String sensorpushPsswd;

void createColoredSquare(lv_obj_t* parent, lv_coord_t x, lv_coord_t y, lv_color_t col) {
    lv_obj_t* square;
    square = lv_obj_create(parent);
    lv_obj_set_pos(square, x, y);
    lv_obj_set_style_bg_color(square, col, 0);
    lv_obj_set_size(square, 20, 20);
    lv_obj_set_style_border_width(square, 0, 0);
    lv_obj_clear_flag(square, LV_OBJ_FLAG_SCROLLABLE);
}

static lv_color_t cols[SENSOR_NUM] = {LV_COLOR_MAKE(0, 200, 100), LV_COLOR_MAKE(255, 140, 0), LV_COLOR_MAKE(50, 50, 200), LV_COLOR_MAKE(0, 200, 220), LV_COLOR_MAKE(100, 0, 160)};

void draw_rv(lv_obj_t* parent)
{
    lv_color_t border = LV_COLOR_MAKE(60, 60, 60);

    lv_obj_t *img = lv_img_create(parent);

    lv_obj_set_style_img_recolor(img, border, LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(img, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_size(img, LV_SIZE_CONTENT, LV_PART_MAIN);

    lv_img_set_src(img, &img_rv);

    lv_obj_align(img, LV_ALIGN_CENTER, LV_PCT(20), 0);

    createColoredSquare(parent, 400, 30, cols[0]);
    createColoredSquare(parent, 410, 220, cols[1]);
    createColoredSquare(parent, 420, 300, cols[2]);
    createColoredSquare(parent, 410, 240, cols[3]);
    createColoredSquare(parent, 365, 215, cols[4]);

}

static void temperature_event_cb(lv_event_t *e)
{
    lv_obj_t *label = (lv_obj_t *)lv_event_get_target(e);
    uint8_t *index = (uint8_t *)lv_event_get_user_data(e);
    lv_msg_t *msg = lv_event_get_msg(e);
    SensorPushApi *api =  (SensorPushApi *)lv_msg_get_payload(msg);
    if (!index) {
        Serial.println("Empty index point");
        return;
    }
    if (!api ) {
        Serial.println("Empty api point");
        return;
    }
    lv_label_set_text_fmt(label, "%.2f°F", api->sensors[*index].sample[0].temp);
}

static void humidity_event_cb(lv_event_t *e)
{
    lv_obj_t *label = (lv_obj_t *)lv_event_get_target(e);
    uint8_t *index = (uint8_t *)lv_event_get_user_data(e);
    lv_msg_t *msg = lv_event_get_msg(e);
    SensorPushApi *api =  (SensorPushApi *)lv_msg_get_payload(msg);
    if (!index) {
        Serial.println("Empty index point");
        return;
    }
    if (!api ) {
        Serial.println("Empty api point");
        return;
    }
    lv_label_set_text_fmt(label, "%.1f%%", api->sensors[*index].sample[0].humidity);
}

static void batt_event_cb(lv_event_t *e)
{
    lv_obj_t *label = (lv_obj_t *)lv_event_get_target(e);
    uint8_t *index = (uint8_t *)lv_event_get_user_data(e);
    lv_msg_t *msg = lv_event_get_msg(e);
    SensorPushApi *api =  (SensorPushApi *)lv_msg_get_payload(msg);
    if (!index) {
        Serial.println("Empty index point");
        return;
    }
    if (!api ) {
        Serial.println("Empty api point");
        return;
    }
    lv_label_set_text_fmt(label, "%.1f V", api->sensors[*index].batt);
}

static uint8_t* selectedIndex = NULL;

static void temp_label_cb(lv_event_t* e) {
    selectedIndex = (uint8_t*)lv_event_get_user_data(e);

    lv_msg_send(OPEN_GRAPH, NULL);
}

void temperature_labels(lv_obj_t* parent){

    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_size(cont, LV_PCT(37), lv_disp_get_ver_res(NULL));
    lv_obj_align(cont, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_left(cont, 0, 0);
    
    static uint8_t index[SENSOR_NUM] = {0, 1, 2, 3, 4};
    static const char* names[SENSOR_NUM] = {"Outdoor", "Refrigerator", "Tank Bay", "Freezer", "Interior"};

    lv_style_t label_style;
    lv_style_init(&label_style);

    lv_obj_t* data_cont;
    lv_obj_t* name;
    lv_obj_t* label;
    lv_obj_t* col;

    for(uint8_t i = 0; i < SENSOR_NUM; i++) {
        data_cont = lv_obj_create(cont);
        lv_obj_set_flex_grow(data_cont, 1);
        lv_obj_set_style_bg_opa(data_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(data_cont, 0, 0);
        lv_obj_set_width(data_cont, LV_PCT(100));
        lv_obj_clear_flag(data_cont, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_set_style_pad_all(data_cont, 0, 0);
        lv_obj_set_style_bg_opa(data_cont, LV_OPA_60, LV_STATE_FOCUSED);
        lv_obj_set_style_bg_grad_dir(data_cont, LV_GRAD_DIR_HOR, LV_STATE_FOCUSED);
        lv_obj_set_style_bg_grad_color(data_cont, lv_color_black(), LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(data_cont, cols[i], LV_STATE_FOCUSED);
        lv_obj_add_event_cb(data_cont, temp_label_cb, LV_EVENT_PRESSED, &index[i]);

        col = lv_obj_create(data_cont);
        lv_obj_set_style_border_width(col, 0, 0);
        lv_obj_set_style_bg_color(col, cols[i], 0);
        lv_obj_align(col, LV_ALIGN_LEFT_MID, 0, 0);
        lv_obj_set_size(col, 10, LV_PCT(100));
        lv_obj_clear_flag(col, LV_OBJ_FLAG_SCROLLABLE);

        name = lv_label_create(data_cont);
        lv_label_set_text(name, names[i]);
        lv_obj_set_style_text_font(name, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(name, lv_color_white(), LV_PART_MAIN);
        lv_obj_align(name, LV_ALIGN_TOP_LEFT, 15, 0);

        label = lv_label_create(data_cont);
        lv_label_set_text(label, "--°F");
        lv_obj_set_style_text_font(label, &lv_font_montserrat_36, 0);
        lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
        lv_msg_subsribe_obj(SENSORPUSH_MSG_ID, label, NULL);
        lv_obj_add_event_cb(label, temperature_event_cb, LV_EVENT_MSG_RECEIVED, &index[i]);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 15, 2);
        lv_obj_set_width(label, LV_PCT(100));

        label = lv_label_create(data_cont);
        lv_label_set_text(label, "--%");
        lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
        lv_msg_subsribe_obj(SENSORPUSH_MSG_ID, label, NULL);
        lv_obj_add_event_cb(label, humidity_event_cb, LV_EVENT_MSG_RECEIVED, &index[i]);
        lv_obj_align(label, LV_ALIGN_BOTTOM_LEFT, 15, 0);
        lv_obj_set_width(label, LV_PCT(50));

        label = lv_label_create(data_cont);
        lv_label_set_text(label, "-- V");
        lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
        lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
        lv_msg_subsribe_obj(SENSORPUSH_MSG_ID, label, NULL);
        lv_obj_add_event_cb(label, batt_event_cb, LV_EVENT_MSG_RECEIVED, &index[i]);
        lv_obj_align(label, LV_ALIGN_BOTTOM_LEFT, LV_PCT(40), 0);
        lv_obj_set_width(label, LV_PCT(50));
    }
}

static lv_obj_t* kb = NULL;

void del_keyboard() {
    lv_keyboard_set_textarea(kb, NULL);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_del_async(kb);
    kb = NULL;
}

static void keyboard_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CANCEL) {
        if(kb != NULL){
            lv_msg_send(HIDE_KEYBOARD, NULL);
        }
    }
}

void create_keyboard(lv_obj_t* ta) {
    kb = lv_keyboard_create(lv_scr_act());
    lv_keyboard_set_textarea(kb, ta);
}

static void edit_textarea(lv_event_t* e){
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* input = lv_event_get_target(e);
    lv_obj_t* win = (lv_obj_t*)lv_event_get_user_data(e);

    if(code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED) {
        if(kb == NULL){
            create_keyboard(input);
            lv_obj_add_event_cb(kb, keyboard_cb, LV_EVENT_ALL, input);

            lv_anim_t anim;
            lv_anim_init(&anim);

            lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_set_var(&anim, win);
            lv_anim_set_values(&anim, 0, -100);
            lv_anim_set_time(&anim, 500);
            lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
            lv_anim_start(&anim);
        }
    } else if (code == LV_EVENT_DEFOCUSED || code == LV_EVENT_MSG_RECEIVED) {
        if(kb != NULL) {
            lv_anim_t anim;
            lv_anim_init(&anim);

            lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_obj_set_y);
            lv_anim_set_var(&anim, win);
            lv_anim_set_values(&anim, -100, 0);
            lv_anim_set_time(&anim, 500);
            lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);
            lv_anim_start(&anim);

            del_keyboard();
        }
    }
}

static void close_button_cb(lv_event_t* e) {
    static lv_obj_t* win = NULL;

    win = (lv_obj_t*)lv_event_get_user_data(e);

    if(kb != NULL) {
        del_keyboard();
    }

    if(win != NULL){
        lv_obj_del(win);
        win = NULL;
    }
}

lv_timer_t* fail = NULL;
lv_timer_t* timeout = NULL;

static void wifi_fail(lv_timer_t* timer) {
    if(WiFi.status() == WL_CONNECT_FAILED) {
        lv_msg_send(WIFI_FAILED, NULL);
        lv_timer_del(timer);
    }
}

static void wifi_timeout(lv_timer_t* timer) {
    WiFi.disconnect();
    lv_msg_send(WIFI_TIMEOUT, NULL);
    lv_timer_del(fail);
    lv_timer_del(timeout);
    timer = NULL;
    fail = NULL;
}

static void del_timer(void* s, lv_msg_t* m) {
    lv_timer_t** timer = (lv_timer_t**)m->user_data;
    lv_timer_del(*timer);
    *timer = NULL;
}

static String psswd;

static void submit_wifi_data(lv_event_t* e) {
    lv_obj_t* input = (lv_obj_t*)lv_event_get_user_data(e);
    psswd = lv_textarea_get_text(input);
    if(WiFi.isConnected()) {
        WiFi.disconnect();
    }

    WiFi.begin(WiFi_SSID, psswd);

    lv_obj_t* win = lv_obj_get_parent(lv_event_get_target(e));

    lv_obj_clean(win);

    fail = lv_timer_create(wifi_fail, 500, NULL);
    timeout = lv_timer_create(wifi_timeout, 8000, NULL);

    lv_msg_subscribe(WIFI_CONNECTED, del_timer, &fail);
    lv_msg_subscribe(WIFI_CONNECTED, del_timer, &timeout);

    lv_obj_t* label = lv_label_create(win);
    lv_label_set_text(label, "Autheticating...");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_center(label);
}

static void close_dialouge(lv_event_t* e) {
    static lv_obj_t* bg = NULL;

    bg = (lv_obj_t*)lv_event_get_user_data(e);

    if(bg != NULL) {
        lv_obj_del(bg);
        bg = NULL;
    }
}

static void update_wifi_dialouge(lv_event_t* e) {
    lv_msg_t* msg = lv_event_get_msg(e);
    lv_obj_t* win = lv_event_get_target(e);
    lv_obj_t* bg = (lv_obj_t*)lv_event_get_user_data(e);

    lv_obj_clean(win);

    lv_obj_t* label;
    lv_obj_t* btn;

    label = lv_label_create(win);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_center(label);

    switch(lv_msg_get_id(msg)){
        case WIFI_CONNECTED:
        {
            cJSON* root = getJSONFromFile(CONFIG_PATH);

            if(root) {
                cJSON_ReplaceItemInObjectCaseSensitive(root, "ssid", cJSON_CreateString(WiFi_SSID.c_str()));
                cJSON_ReplaceItemInObjectCaseSensitive(root, "password", cJSON_CreateString(psswd.c_str()));

                writeJSONToFile(root, CONFIG_PATH);

                cJSON_Delete(root);
            }

            lv_label_set_text(label, "Connected");
            break;
        }
        case WIFI_TIMEOUT:
        {
            lv_label_set_text(label, "Timeout");
            break;
        }
        case WIFI_FAILED:
        {
            lv_label_set_text(label, "Failed");
            break;
        }
        default:
        {
            Serial.println("Invalid message received");
            lv_label_set_text(label, "Invalid message received");
        }
    }

    btn = lv_btn_create(win);
    lv_obj_set_style_bg_color(btn, dim_green, 0);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_add_event_cb(btn, close_dialouge, LV_EVENT_CLICKED, bg);

    label = lv_label_create(btn);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_center(label);
    lv_label_set_text(label, "OK");
}

static void open_wifi_dialouge(lv_event_t* e) {
    lv_obj_t* bg = lv_obj_create(lv_scr_act());
    lv_obj_set_style_radius(bg, 0, 0);
    lv_obj_set_size(bg, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_set_style_bg_color(bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(bg, LV_OPA_50, 0);
    lv_obj_set_style_border_width(bg, 0, 0);

    lv_obj_t* btn = lv_event_get_target(e);
    lv_obj_t* label = lv_obj_get_child(btn, 1);

    lv_obj_t* win = lv_obj_create(bg);
    lv_obj_set_size(win, LV_PCT(50), LV_PCT(40));
    lv_obj_set_style_bg_color(win, LV_COLOR_MAKE(10, 10, 10), 0);
    lv_obj_set_style_radius(win, 10, 0);
    lv_obj_set_style_shadow_width(win, 80, 0);
    lv_obj_set_style_shadow_spread(win, 10, 0);
    lv_obj_set_style_shadow_color(win, lv_color_black(), 0);
    lv_obj_set_style_border_width(win, 0, 0);
    lv_msg_subscribe_obj(WIFI_CONNECTED, win, NULL);
    lv_msg_subscribe_obj(WIFI_TIMEOUT, win, NULL);
    lv_msg_subscribe_obj(WIFI_FAILED, win, NULL);
    lv_obj_add_event_cb(win, update_wifi_dialouge, LV_EVENT_MSG_RECEIVED, bg);

    lv_obj_center(win);

    const char* ssid = lv_label_get_text(label);
    WiFi_SSID = String(ssid);

    label = lv_label_create(win);
    lv_label_set_text(label, ssid);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* input = lv_textarea_create(win);
    lv_textarea_set_placeholder_text(input, "Password");
    lv_textarea_set_one_line(input, true);
    lv_textarea_set_password_mode(input, true);
    lv_obj_align(input, LV_ALIGN_CENTER, 0, -10);
    lv_obj_set_size(input, LV_PCT(80), 50);
    lv_obj_set_style_bg_color(input, LV_COLOR_MAKE(30, 30, 30), 0);
    lv_obj_set_style_border_width(input, 0, 0);
    lv_msg_subscribe_obj(HIDE_KEYBOARD, input, NULL);
    lv_obj_add_event_cb(input, edit_textarea, LV_EVENT_ALL, win);
    label = lv_textarea_get_label(input);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_CURSOR);

//=====DONE BUTTON======
    btn = lv_btn_create(win);
    lv_obj_set_style_bg_color(btn, dim_green, 0);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_add_event_cb(btn, submit_wifi_data, LV_EVENT_CLICKED, input);

    label = lv_label_create(btn);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_center(label);
    lv_label_set_text(label, "Done");
//=====CANCEL BUTTON====
    btn = lv_btn_create(win);
    lv_obj_set_style_bg_color(btn, dim_green, 0);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_add_event_cb(btn, close_button_cb, LV_EVENT_CLICKED, bg);

    label = lv_label_create(btn);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_center(label);
    lv_label_set_text(label, "Cancel");
}

static void update_wifi_device_list(lv_event_t* e) {

    lv_obj_t* list = (lv_obj_t*)lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_MSG_RECEIVED){
        lv_msg_t* msg = lv_event_get_msg(e);
        lv_obj_t* btn;
        switch(lv_msg_get_id(msg)){
            case WIFI_SCAN_DONE:
            {
                lv_obj_clean(list);

                int n = WiFi.scanComplete();
                if(n == -1) {
                    Serial.print("[WiFi] Couldn't complete WiFi scan");
                } else {
                    if(n > 0) {
                        int num = 0;
                        String* ssids = new String[n];
                        for (int i = 0; i < n; i++) {
                            String ssid = WiFi.SSID(i);
                            bool found = false;

                            for(int x = 0; x < num; x++) {
                                if(ssids[x] == ssid) {
                                    found = true;
                                    break;
                                }
                            }

                            if(found) continue;

                            if(WiFi.isConnected() && ssid == WiFi.SSID()) {
                                btn = lv_list_add_btn(list, LV_SYMBOL_WIFI, WiFi_SSID.c_str());
                                lv_obj_set_style_text_font(btn, &lv_font_montserrat_26, 0);
                                lv_obj_set_style_text_color(btn, green, 0);
                                lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_STATE_PRESSED);
                                lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_NONE, LV_STATE_PRESSED);
                                lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_NONE, 0);
                                lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
                                lv_obj_move_foreground(btn);
                            } else {
                                btn = lv_list_add_btn(list, LV_SYMBOL_WIFI, ssid.c_str());
                                lv_obj_set_style_text_font(btn, &lv_font_montserrat_26, 0);
                                lv_obj_set_style_text_color(btn, lv_color_white(), 0);
                                lv_obj_set_style_text_color(btn, green, LV_STATE_PRESSED);
                                lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_STATE_PRESSED);
                                lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_NONE, LV_STATE_PRESSED);
                                lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_NONE, 0);
                                lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
                                lv_obj_add_event_cb(btn, open_wifi_dialouge, LV_EVENT_CLICKED, NULL);
                            }

                            ssids[num++] = ssid;
                        }
                    } else {
                        Serial.print("[WiFi] No networks found");
                    }
                }

                WiFi.scanDelete();
                break;
            }
            case WIFI_CONNECTED:
            {
                btn = lv_list_add_btn(list, LV_SYMBOL_WIFI, WiFi_SSID.c_str());
                lv_obj_set_style_text_font(btn, &lv_font_montserrat_26, 0);
                lv_obj_set_style_text_color(btn, green, 0);
                lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_STATE_PRESSED);
                lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_NONE, LV_STATE_PRESSED);
                lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_NONE, 0);
                lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
                lv_obj_move_foreground(btn);

                break;
            }
            default:
            {
                Serial.println("Invalid message received");
                break;
            }
        }
    }
}

static void refresh_wifi(lv_event_t* e) {
    lv_anim_timeline_t* anim = (lv_anim_timeline_t*)lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_PRESSED){
        if(WiFi.scanComplete() == -2){
            WiFi.scanNetworks(true);
            
        }
        lv_anim_timeline_start(anim);
    } else if(code == LV_EVENT_MSG_RECEIVED){
        lv_anim_timeline_stop(anim);
    }

}

void wifi_conf(lv_obj_t* parent) {
    lv_anim_timeline_t* anim_timeline = lv_anim_timeline_create();

    lv_obj_t* list = lv_list_create(parent);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
    lv_msg_subscribe_obj(WIFI_SCAN_DONE, list, NULL);
    lv_msg_subscribe_obj(WIFI_CONNECTED, list, NULL);
    lv_obj_add_event_cb(list, update_wifi_device_list, LV_EVENT_MSG_RECEIVED, NULL);

    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_msg_subscribe_obj(WIFI_SCAN_DONE, btn, NULL);
    lv_msg_subscribe_obj(WIFI_SCAN_FAILED, btn, NULL);
    lv_obj_align(btn, LV_ALIGN_TOP_RIGHT, 40, -40);
    lv_obj_t* refresh_img = lv_img_create(btn);
    lv_img_set_zoom(refresh_img, 128);
    lv_img_set_src(refresh_img, &img_refresh);
    lv_obj_center(refresh_img);
    
    lv_anim_t anim;
    lv_anim_init(&anim);

    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_img_set_angle);
    lv_anim_set_var(&anim, refresh_img);
    lv_anim_set_early_apply(&anim, false);
    lv_anim_set_values(&anim, 0, 3600);
    lv_anim_set_time(&anim, 500);
    lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(&anim, lv_anim_path_linear);

    lv_anim_timeline_add(anim_timeline, 0, &anim);

    lv_obj_add_event_cb(btn, refresh_wifi, LV_EVENT_ALL, anim_timeline);
}

static lv_point_t temp_points[SENSOR_NUM][GRAPH_RES];
static const int GRAPH_CONTENT_H = (GRAPH_H - 2 * GRAPH_PADDING);

static void graph_cb(lv_event_t* e) {
    lv_obj_t* parent = lv_event_get_target(e);
    lv_msg_t *msg = lv_event_get_msg(e);
    SensorPushApi* api = (SensorPushApi*)lv_msg_get_payload(msg);

    lv_obj_clean(parent);

    if(GRAPH_RES <= 0 && SENSOR_NUM <= 0) return;
    
    double min = api->sensors[0].sample[0].temp, max = api->sensors[0].sample[0].temp;

    for(int n = 0; n < SENSOR_NUM; n++){
        SensorData data = api->sensors[n];
        for(int i = 1; i < GRAPH_RES; i++) {
            if(data.sample[i].temp < min) {
                min = data.sample[i].temp;
            } else if (data.sample[i].temp > max) {
                max = data.sample[i].temp;
            }
        }
    }

    double diff = max - min;
    lv_obj_t* line;

    for(int n = 0; n < SENSOR_NUM; n++){
        SensorData data = api->sensors[n];
        for(int i = 0; i < GRAPH_RES; i++) {
            temp_points[n][i] = {
                (short)(GRAPH_W - ((((double)i)/(GRAPH_RES - 1)) * GRAPH_W)),
                (short)(GRAPH_CONTENT_H - (((data.sample[i].temp - min)/(diff)) * GRAPH_CONTENT_H) + GRAPH_PADDING)
            };
        }
        
        line = lv_line_create(parent);
        lv_line_set_points(line, temp_points[n], GRAPH_RES);
        lv_obj_set_style_line_color(line, cols[n], 0);
        lv_obj_set_style_line_width(line, 3, 0);
        lv_obj_set_style_line_rounded(line, true, 0);

        if(selectedIndex != NULL && n != *selectedIndex) {
            lv_obj_set_style_line_opa(line, LV_OPA_10, 0);
        }
    }
}

static const lv_point_t graph_border[] = {
    {0, 0}, {0, GRAPH_H}, {GRAPH_W, GRAPH_H}
};

static void show_data_chart(lv_event_t* e) {
    lv_obj_t* cont = lv_event_get_target(e);
    lv_obj_t* graph = (lv_obj_t*)lv_event_get_user_data(e);

    if(lv_obj_has_flag(cont, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_clear_flag(cont, LV_OBJ_FLAG_HIDDEN);
    }
    
    int n = lv_obj_get_child_cnt(graph);

    if(n != SENSOR_NUM) {
        Serial.println("Failed to get graph children");
        return;
    }

    lv_obj_t* line;

    for(int i = 0; i < n; i++) {
        line = lv_obj_get_child(graph, i);
        if(i == *selectedIndex) {
            lv_obj_set_style_line_opa(line, LV_OPA_COVER, 0);
        } else {
            lv_obj_set_style_line_opa(line, LV_OPA_10, 0);
        }
    }
}

static void hide_data_chart(lv_event_t* e) {
    lv_obj_t* cont = (lv_obj_t*)lv_event_get_user_data(e);

    if(!lv_obj_has_flag(cont, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_add_flag(cont, LV_OBJ_FLAG_HIDDEN);
    }
}

void create_data_chart(lv_obj_t* parent){
    lv_obj_t* cont = lv_obj_create(parent);
    lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
    lv_obj_align(cont, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_size(cont, LV_PCT(70), LV_PCT(100));
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_HIDDEN);
    lv_msg_subscribe_obj(OPEN_GRAPH, cont, NULL);

    lv_obj_t* line;
    line = lv_line_create(cont);
    lv_line_set_points(line, graph_border, 3);
    lv_obj_set_style_line_color(line, LV_COLOR_MAKE(100, 100, 100), 0);
    lv_obj_set_style_line_width(line, 3, 0);
    lv_obj_set_style_line_rounded(line, true, 0);
    lv_obj_align(line, LV_ALIGN_LEFT_MID, 30, 0);
    lv_msg_subscribe_obj(SENSORPUSH_MSG_ID, line, NULL);
    lv_obj_add_event_cb(line, graph_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_obj_add_event_cb(cont, show_data_chart, LV_EVENT_MSG_RECEIVED, line);

    lv_obj_t* btn = lv_btn_create(cont);
    lv_obj_align(btn, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(btn, hide_data_chart, LV_EVENT_PRESSED, cont);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_t* txt = lv_label_create(btn);
    lv_label_set_text(txt, LV_SYMBOL_CLOSE);
    lv_obj_set_style_text_color(txt, lv_color_white(), 0);
}

static void last_update_cb(lv_event_t* e) {
    lv_obj_t* label = lv_event_get_target(e);
    lv_msg_t* msg = lv_event_get_msg(e);
    SensorPushApi* api = (SensorPushApi*)lv_msg_get_payload(msg);

    String date = api->last_time.substring(0, 10);
    String time = api->last_time.substring(13, 19);
    int hour = atoi(api->last_time.substring(11, 13).c_str());

    lv_label_set_text_fmt(label, "Last Updated: %s | %d%s", date, hour, time);
}

void last_update(lv_obj_t* parent) {
    lv_obj_t* label = lv_label_create(parent);

    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, LV_PCT(20), 0);
    lv_obj_set_style_pad_all(label, 0, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_msg_subscribe_obj(SENSORPUSH_MSG_ID, label, NULL);
    lv_obj_add_event_cb(label, last_update_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_label_set_text(label, "Last Updated: -- | --");
}

lv_obj_t* createButton(lv_obj_t* parent, const char* txt, lv_event_cb_t cb, void* user_data) {
    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_style_bg_color(btn, dim_green, 0);
    lv_obj_add_event_cb(btn, cb, LV_EVENT_PRESSED, user_data);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_t* label = lv_label_create(btn);
    lv_label_set_text(label, txt);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);

    return btn;
}

void change_brightness_cb(lv_event_t* e) {
    lv_obj_t* slider = lv_event_get_target(e);
    uint8_t val = (uint8_t)lv_slider_get_value(slider);
    
    amoled.setBrightness(val);
}

//TODO: Fix window formatting
void clear_data_cb(lv_event_t* e) {
    lv_obj_t* bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(bg, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_set_pos(bg, 0, 0);
    lv_obj_set_style_bg_color(bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(bg, LV_OPA_50, 0);
    lv_obj_set_style_border_width(bg, 0, 0);

    lv_obj_t* win = lv_msgbox_create(bg, NULL, NULL, NULL, false);
    lv_obj_center(win);
    lv_obj_set_style_bg_color(win, LV_COLOR_MAKE(10, 10, 10), 0);
    lv_obj_set_style_border_width(win, 0, 0);
    lv_obj_set_size(win, 300, 150);

    lv_obj_t* label = lv_label_create(win);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
    lv_label_set_text(label, "Are you sure you want to clear your data?");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_size(label, 200, 75);
    
    lv_obj_t* btn;
    btn = createButton(win, "Proceed", [](lv_event_t* e){
        lv_obj_t* win = (lv_obj_t*)lv_event_get_user_data(e);

        createDefaultConfig();

        lv_obj_del_async(win);
    }, bg);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    btn = createButton(win, "Cancel", [](lv_event_t* e){
        lv_obj_t* win = (lv_obj_t*)lv_event_get_user_data(e);

        lv_obj_del_async(win);
    }, bg);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
}

static String sp_email_input;
static String sp_psswd_input;

lv_obj_t* createTextarea(lv_obj_t* parent, const char* placeholder, String* res, bool psswd_mode) {
    lv_obj_t* input = lv_textarea_create(parent);
    lv_textarea_set_placeholder_text(input, placeholder);
    lv_textarea_set_one_line(input, true);
    lv_textarea_set_password_mode(input, psswd_mode);
    lv_obj_set_size(input, LV_PCT(80), 50);
    lv_obj_set_style_bg_color(input, LV_COLOR_MAKE(30, 30, 30), 0);
    lv_obj_set_style_border_width(input, 0, 0);
    lv_msg_subscribe_obj(HIDE_KEYBOARD, input, NULL);
    lv_obj_add_event_cb(input, [](lv_event_t* e){
        String* res = (String*)lv_event_get_user_data(e);
        lv_obj_t* input = lv_event_get_target(e);

        *res = lv_textarea_get_text(input);
    }, LV_EVENT_VALUE_CHANGED, res);
    lv_obj_t* label = lv_textarea_get_label(input);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_CURSOR);

    return input;
}

//TODO: Fix Window Formatting
void auth_btn_cb(lv_event_t* e) {
    lv_obj_t* bg = lv_obj_create(lv_scr_act());
    lv_obj_set_size(bg, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_set_pos(bg, 0, 0);
    lv_obj_set_style_bg_color(bg, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(bg, LV_OPA_50, 0);
    lv_obj_set_style_border_width(bg, 0, 0);

    lv_obj_t* win = lv_msgbox_create(bg, NULL, NULL, NULL, false);
    lv_obj_center(win);
    lv_obj_set_style_bg_color(win, LV_COLOR_MAKE(10, 10, 10), 0);
    lv_obj_set_style_border_width(win, 0, 0);
    lv_obj_set_size(win, 220, 200);

    lv_obj_t* email = createTextarea(win, "E-mail", &sp_email_input, false);
    lv_obj_align_to(email, win, LV_ALIGN_CENTER, 0, -10);
    lv_obj_add_event_cb(email, edit_textarea, LV_EVENT_ALL, win);
    lv_obj_set_width(email, LV_PCT(100));
    
    lv_obj_t* sp_psswd = createTextarea(win, "Password", &sp_psswd_input, true);
    lv_obj_align_to(sp_psswd, win, LV_ALIGN_CENTER, 0, 50);
    lv_obj_add_event_cb(sp_psswd, edit_textarea, LV_EVENT_ALL, win);
    lv_obj_set_width(sp_psswd, LV_PCT(100));
    
    lv_obj_t* btn;
    btn = createButton(win, "Login", [](lv_event_t* e){
        lv_obj_t* win = (lv_obj_t*)lv_event_get_user_data(e);

        sensorpushEmail = sp_email_input;
        sensorpushPsswd = sp_psswd_input;

        cJSON* root = getJSONFromFile(CONFIG_PATH);

        if(root){
            cJSON_ReplaceItemInObjectCaseSensitive(root, "sp_email", cJSON_CreateString(sensorpushEmail.c_str()));
            cJSON_ReplaceItemInObjectCaseSensitive(root, "sp_password", cJSON_CreateString(sensorpushPsswd.c_str()));

            writeJSONToFile(root, CONFIG_PATH);

            cJSON_Delete(root);
        }

        lv_obj_del_async(win);
    }, bg);
    lv_obj_align_to(btn, win, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    btn = createButton(win, "Cancel", [](lv_event_t* e){
        lv_obj_t* win = (lv_obj_t*)lv_event_get_user_data(e);

        lv_obj_del_async(win);
    }, bg);
    lv_obj_align_to(btn, win, LV_ALIGN_BOTTOM_RIGHT, 10, 0);
}

void settings_tab(lv_obj_t* parent) {
    lv_obj_t* slider = lv_slider_create(parent);
    lv_obj_t* icon = lv_label_create(parent);
    lv_label_set_text(icon, "Brightness");
    lv_obj_set_style_text_color(icon, lv_color_white(), 0);

    lv_slider_set_range(slider, 20, 255);
    lv_obj_align_to(slider, icon, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_add_event_cb(slider, change_brightness_cb, LV_EVENT_VALUE_CHANGED, 0);

    //Slider style
    lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, green, LV_PART_KNOB);
    lv_obj_set_style_bg_color(slider, green, LV_PART_INDICATOR);
    lv_slider_set_value(slider, 255, LV_ANIM_OFF);

    lv_obj_t* btn = lv_btn_create(parent);
    lv_obj_set_style_bg_color(btn, dim_green, 0);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_event_cb(btn, clear_data_cb, LV_EVENT_PRESSED, 0);
    lv_obj_t* label = lv_label_create(btn);
    lv_label_set_text(label, "Clear Data");
    lv_obj_set_style_text_color(label, lv_color_white(), 0);

    btn = createButton(parent, "SensorPush Login", auth_btn_cb, 0);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 40);
}

void drawGUI(void)
{
    lv_obj_t* tab_view = lv_tabview_create(lv_scr_act(), LV_DIR_RIGHT, 40);
    lv_obj_t* tab_btns = lv_tabview_get_tab_btns(tab_view);
    lv_obj_set_style_bg_color(tab_view, lv_color_black(), 0 );
    lv_obj_clear_flag(lv_tabview_get_content(tab_view), LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_NONE, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(tab_btns, lv_color_white(), 0);
    lv_obj_set_style_text_color(tab_btns, green, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(tab_btns, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_opa(tab_btns, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(tab_btns, lv_color_black(), 0);
    lv_obj_set_style_bg_color(tab_btns, lv_color_black(), LV_PART_ITEMS | LV_STATE_CHECKED);

    lv_obj_t* tab;

//  HOME TAB
    tab = lv_tabview_add_tab(tab_view, LV_SYMBOL_HOME);
    lv_obj_clear_flag(tab, LV_OBJ_FLAG_SCROLLABLE);

    draw_rv(tab);
    create_data_chart(tab);
    temperature_labels(tab);
    last_update(tab);

//  WIFI TAB
    tab = lv_tabview_add_tab(tab_view, LV_SYMBOL_WIFI);
    lv_obj_clear_flag(tab, LV_OBJ_FLAG_SCROLLABLE);

    wifi_conf(tab);

//  SETTINGS TAB
    tab = lv_tabview_add_tab(tab_view, LV_SYMBOL_SETTINGS);
    lv_obj_clear_flag(tab, LV_OBJ_FLAG_SCROLLABLE);

    settings_tab(tab);
}
