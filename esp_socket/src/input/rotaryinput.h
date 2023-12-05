
#ifndef ROTARYINPUT_H_
#define ROTARYINPUT_H_

void setup_input_gpios(void);

typedef enum InputEvent {
    CW_ROTATION = 0,
    CCW_ROTATION,
    PUSH,
} InputEvent;

#endif
