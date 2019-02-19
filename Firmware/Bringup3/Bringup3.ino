#include <Wire.h>
#include <Bounce.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define PIN_MODE_LIGHT 3
#define PIN_BUTTON_LIGHT 5
#define PIN_MODE 4
#define PIN_BUTTON 6

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();
Bounce bouncerMode = Bounce(PIN_MODE, 5);
Bounce bouncerBig = Bounce(PIN_BUTTON, 5);
bool firstModePush = true;
unsigned char mode = 0;
int currentTime;

struct ModeSettings {
    char name[5];
    int maxTime;
    int warnTime;
};

const ModeSettings Modes[] = {
    {"30 s", 30, 10},
    {"1min", 60, 10},
    {"5min", 5 * 30, 30}
};
unsigned int ModeCount = sizeof(Modes) / sizeof(Modes[0]);

void startupAnimation(int animationDelay)
{
    // Outer Spinner
    for (int shiftValue = 0; shiftValue < 6; shiftValue++)
    {
        for (int pos = 0; pos < 4; pos++)
            alpha4.writeDigitRaw(pos, 0x01 << shiftValue);
        alpha4.writeDisplay();
        delay(animationDelay);
    }
    // Inner Spinner
    unsigned char INNER_SPINNER_SHIFT_VALUE[] = {
        8, 9, 10, 7, 13, 12, 11, 6, 8
    };
    for (int shiftValue = 0; shiftValue < sizeof(INNER_SPINNER_SHIFT_VALUE) / sizeof(INNER_SPINNER_SHIFT_VALUE[0]); shiftValue++)
    {
        for (int pos = 0; pos < 4; pos++)
            alpha4.writeDigitRaw(pos, ((uint16_t) 0x01) << INNER_SPINNER_SHIFT_VALUE[shiftValue]);
        alpha4.writeDisplay();
        delay(animationDelay);
    }
    for (int pos = 0; pos< 4; pos++)
    {
        alpha4.clear();
        alpha4.writeDigitRaw(pos, ((uint16_t) 0x01) << 14);
        alpha4.writeDisplay();
        delay(animationDelay);
    }
}

void rotateMode()
{
    if (!firstModePush)
        mode = (mode + 1) % ModeCount;
    else
        firstModePush = false;
    alpha4.writeDigitAscii(0, Modes[mode].name[0]);
    alpha4.writeDigitAscii(1, Modes[mode].name[1]);
    alpha4.writeDigitAscii(2, Modes[mode].name[2]);
    alpha4.writeDigitAscii(3, Modes[mode].name[3]);
    alpha4.writeDisplay();
    delay(100);
}

void waitForButton()
{
    bouncerMode.update();
    bouncerBig.update();
    while (1)
    {
        if (bouncerMode.update())
        {
            while (0 == bouncerMode.read())
            {
                bouncerMode.update();
            }
            rotateMode();
        }
        if (bouncerBig.update())
        {
            while (0 == bouncerBig.read())
            {
                bouncerBig.update();
            }
            alpha4.clear();
            alpha4.writeDisplay();
            firstModePush = true;
            break;
        }
    }
}

void setup() 
{
    pinMode(PIN_MODE_LIGHT, OUTPUT);
    pinMode(PIN_BUTTON_LIGHT, OUTPUT);
    pinMode(4, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);

    alpha4.begin(0x70);
    alpha4.setBrightness(4);
    alpha4.clear();
    alpha4.writeDisplay();
    startupAnimation(50);
    alpha4.clear();
    alpha4.writeDisplay();
    
    alpha4.writeDigitAscii(0, 'P');
    alpha4.writeDigitAscii(1, 'u');
    alpha4.writeDigitAscii(2, 's');
    alpha4.writeDigitAscii(3, 'h');
    alpha4.writeDisplay();
    waitForButton();
}

void printTime()
{
    alpha4.clear();
    alpha4.writeDigitRaw(1, ((uint16_t) 0x01) << 14);
    alpha4.writeDigitAscii(2, (currentTime / 10) + '0');
    alpha4.writeDigitAscii(3, (currentTime % 10) + '0');
    alpha4.writeDisplay();
}

void printFail()
{
    alpha4.setBrightness(15);
    for (int i = 0; i < 4; i++)
    {
        alpha4.writeDigitAscii(0, '*');
        alpha4.writeDigitAscii(1, '*');
        alpha4.writeDigitAscii(2, '*');
        alpha4.writeDigitAscii(3, '*');
        alpha4.writeDisplay();
        delay(250);
        alpha4.clear();
        alpha4.writeDisplay();
        delay(250);
    }
    alpha4.setBrightness(4);
}

void loop() 
{
    digitalWrite(PIN_MODE_LIGHT, !digitalRead(PIN_MODE));
    digitalWrite(PIN_BUTTON_LIGHT, !digitalRead(PIN_BUTTON));
}
