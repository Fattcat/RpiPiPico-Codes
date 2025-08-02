#include "Adafruit_TinyUSB.h"

Adafruit_USBD_HID usb_hid;

const uint8_t keyboard_report_descriptor[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

namespace keyboard {
    typedef struct report {
        uint8_t modifiers;
        uint8_t reserved;
        uint8_t keys[6];
    } report;

    report prev_report = report { 0x00, 0x00, {0,0,0,0,0,0} };

    void send(report* k) {
        usb_hid.sendReport(0, (uint8_t*)k, sizeof(report));
    }

    void release() {
        report empty = {0,0,{0,0,0,0,0,0}};
        send(&empty);
    }

    void type(uint8_t key0, uint8_t key1, uint8_t key2, uint8_t key3, uint8_t key4, uint8_t key5, uint8_t modifiers) {
        prev_report.modifiers = modifiers;
        prev_report.keys[0] = key0;
        prev_report.keys[1] = key1;
        prev_report.keys[2] = key2;
        prev_report.keys[3] = key3;
        prev_report.keys[4] = key4;
        prev_report.keys[5] = key5;
        send(&prev_report);
        delay(5);
        release();
        delay(5);
    }
}

// SK layout páry (modifikátor, keycode) z duckify
const uint8_t key_arr_0[] PROGMEM = {0,11, 0,23, 0,23, 0,19, 0,22, 2,55, 2,47, 2,47, 0,26, 0,26, 0,26, 0,55, 0,29, 0,18, 0,24, 0,23, 0,24, 0,5, 0,8, 0,55, 0,6, 0,18, 0,16, 2,47, 0,26, 0,4, 0,23, 0,6, 0,11, 2,54, 0,25, 0,45, 0,7, 2,20, 0,26, 2,33, 0,26, 2,38, 2,26, 0,10, 2,27, 0,6, 2,20};

void duckyString(const uint8_t* keys, size_t len) {
    for (size_t i=0; i<len; i+=2) {
        uint8_t modifier = pgm_read_byte_near(keys + i);
        uint8_t keycode = pgm_read_byte_near(keys + i + 1);
        keyboard::type(keycode, 0, 0, 0, 0, 0, modifier);
    }
}

void setup() {
    usb_hid.setReportDescriptor(keyboard_report_descriptor, sizeof(keyboard_report_descriptor));
    usb_hid.begin();

    delay(2000);

    // Win+R podľa duckify
    keyboard::type(21, 0, 0, 0, 0, 0, 8);
    delay(1000);

    duckyString(key_arr_0, sizeof(key_arr_0));
    delay(1000);

    // ENTER
    keyboard::type(40, 0, 0, 0, 0, 0, 0);
}

void loop() {}
