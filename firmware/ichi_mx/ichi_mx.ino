/*
  ichi mx via firmware
  (c) 2025 Takuya Urakawa, 5z6p.com

  Based on HID Keyboard mouse combo example, compatible with QMK via protoco for remapping
  created 2023
  by Deqing Sun for use with CH55xduino
*/

#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userQmkCompatibleKeyboard/USBHIDKeyboardMouse.h"
#include "keyboardConfig.h"

//these variables will be externally refered by the via library, they shall match the json file
//on ch552 there is 128 Byte of data flash, so we keep the row*col*layer to be less than 64 (2Byte each key)

#define BUTTON1_PIN 17

bool button1PressPrev = false;

unsigned long previousKeyScanMillis = 0;

void setup() {
  USBInit();
  __data uint8_t dataLength = 2; // 2 byte per key
  {
    //initailize the data flash if all key maps are 0xFF
    __data uint8_t allConfigFF = 1;
    for (__data uint8_t i = 0; i < dataLength; i++) {
      if (eeprom_read_byte(i) != 0xFF) {
        allConfigFF = 0;
        break;
      }
    }
    if (allConfigFF) {
      // write the default keymap
      // you can find keycode from https://github.com/qmk/qmk_firmware/blob/master/quantum/keycodes.h
      // set 2byte per key
      const uint8_t defaultKeymap[] = {0x00, 0x28}; // Enter
      for (__data uint8_t i = 0; i < dataLength; i++) {
        eeprom_write_byte(i, defaultKeymap[i]);
      }
    }
  }

  { //initialize the keys or matrix.
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
  }
}

void loop() {

  via_process();

  if ((signed int)(millis() - previousKeyScanMillis) >= 50) { //naive debouncing
    // scan the keys or matrix.
    previousKeyScanMillis = millis();

    bool button1Press = !digitalRead(BUTTON1_PIN);
    if (button1PressPrev != button1Press) {
      button1PressPrev = button1Press;
      press_qmk_key(0, 0, 0, button1Press);
    }
  }

}
