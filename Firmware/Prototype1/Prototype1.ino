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
unsigned char counterMode = 0;
int currentTime;

struct ModeSettings {
    char name[5];
    int maxTime;
    int warnTime;
};

const ModeSettings Modes[] = {
#if 0 // a nice short debug time value
    {" 5 s",     10,  7},
#endif
    {"30 s",      30,     10},
    {"1min",      60,     10},
    {"5min",  5 * 60,     30},
    {"30 m", 30 * 60, 5 * 60}
};
unsigned int ModeMax = sizeof(Modes) / sizeof(Modes[0]);

void startupAnimation(int animationDelay)
{
    digitalWrite(PIN_MODE_LIGHT, HIGH);
    digitalWrite(PIN_BUTTON_LIGHT, HIGH);

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
    digitalWrite(PIN_MODE_LIGHT, LOW);
    digitalWrite(PIN_BUTTON_LIGHT, LOW);
}

void scrollText(char *message, int &pos, int animationDelay)
{
    int letterPos = 0;
    int messageLength = strlen(message);
    alpha4.clear();
    for (
        int messagePos = pos; 
        (messagePos < messageLength) &&
        (messagePos < messageLength) &&
        (letterPos < 4); messagePos++)
    {
        alpha4.writeDigitAscii(letterPos++, message[messagePos]);
    }
    alpha4.writeDisplay();
    delay(animationDelay);
    pos++;
}

void doShrek()
{
    const char *message = "   dubious Fun?   ";
    const unsigned int messageLength = strlen(message);
    int pos = 0;
    while (1) 
    {
        scrollText(message, pos, 250);
        if (pos >= messageLength)
            return;
    }
}

void rotateMode()
{
    if (!firstModePush)
        counterMode = (counterMode + 1) % ModeMax;
    else
        firstModePush = false;
    alpha4.setBrightness(15);
    alpha4.writeDigitAscii(0, Modes[counterMode].name[0]);
    alpha4.writeDigitAscii(1, Modes[counterMode].name[1]);
    alpha4.writeDigitAscii(2, Modes[counterMode].name[2]);
    alpha4.writeDigitAscii(3, Modes[counterMode].name[3]);
    alpha4.writeDisplay();
    delay(100);
}

// Handle presses of the mode button. Returns when the main button is pressed.
void attractLoop()
{
    unsigned long longPress = 0;
    unsigned long screenReset = 0;
    printPush();
    digitalWrite(PIN_MODE_LIGHT, LOW);
    digitalWrite(PIN_BUTTON_LIGHT, LOW);
    printPush();
    bouncerMode.update();
    bouncerBig.update();
    while (1)
    {
        if (bouncerMode.update())
        {
            longPress = millis() + 5000;
            while (0 == bouncerMode.read())
            {
                bouncerMode.update();
                if (millis() > longPress)
                {
                    doShrek();
                    firstModePush = true;
                }
            }
            rotateMode();
            screenReset = millis() + 5000;
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
            return;
        }
        if (screenReset && millis() > screenReset)
        {
            screenReset = 0;
            printPush();
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
    
}

void printPush()
{
    alpha4.setBrightness(4);
    alpha4.writeDigitAscii(0, 'P');
    alpha4.writeDigitAscii(1, 'u');
    alpha4.writeDigitAscii(2, 's');
    alpha4.writeDigitAscii(3, 'h');
    alpha4.writeDisplay();
}

void printTime()
{
    unsigned int minutes = currentTime / 60;
    unsigned int seconds = currentTime % 60;
    alpha4.clear();
    if (minutes)
    {
        alpha4.writeDigitAscii(0, (minutes / 10) + '0');
        alpha4.writeDigitAscii(1, (minutes % 10) + '0', true);
    }else {
        alpha4.writeDigitRaw(1, ((uint16_t) 0x01) << 14);
    }
    alpha4.writeDigitAscii(2, (seconds / 10) + '0');
    alpha4.writeDigitAscii(3, (seconds % 10) + '0');
    alpha4.writeDisplay();
}

void printFail()
{
    alpha4.setBrightness(15);
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(PIN_MODE_LIGHT, HIGH);
        digitalWrite(PIN_BUTTON_LIGHT, HIGH);
        alpha4.writeDigitAscii(0, '*');
        alpha4.writeDigitAscii(1, '*');
        alpha4.writeDigitAscii(2, '*');
        alpha4.writeDigitAscii(3, '*');
        alpha4.writeDisplay();
        delay(250);
        digitalWrite(PIN_MODE_LIGHT, LOW);
        digitalWrite(PIN_BUTTON_LIGHT, LOW);
        alpha4.clear();
        alpha4.writeDisplay();
        delay(250);
    }
    alpha4.setBrightness(4);
}

unsigned char checkButtonDelay(int delayValue, int stepValue, unsigned char animate)
{
    int buttonLight = 0;
    char buttonLightDirection = 1;
    int currentAnimationShift = -1;
    while (delayValue > 0)
    {
        bouncerBig.update();
        if (0 == bouncerBig.read())
        {
            while (0 == bouncerBig.read())
            {
                bouncerBig.update();
            }
            if (animate)
            {
                alpha4.writeDigitAscii(0, ' ');
                alpha4.writeDisplay();
            }
            return 1;
        }
        if (animate && currentTime < 60)
        {
            unsigned char ANIMATION_SHIFT[] = {10,7,13,12,11,6,8,9};
            int shiftValue = delayValue / 125;
            shiftValue = (shiftValue < 0) ? 0 : ((shiftValue > 7) ? 7 : shiftValue);
            if (currentAnimationShift != shiftValue)
            {
                alpha4.writeDigitRaw(0, 0x01 << ANIMATION_SHIFT[shiftValue]);
                alpha4.writeDisplay();
                currentAnimationShift = shiftValue;
            }
        }
        // Animate "breathing" when we're past the warning threshold.
        if (animate && currentTime <= Modes[counterMode].warnTime)
        {
            buttonLight += buttonLightDirection * 5;
            if (buttonLight >= 255)
            {
                buttonLightDirection = -1;
                buttonLight = 255;
            } else if (buttonLight <= 0) {
                buttonLightDirection = 1;
                buttonLight = 0;
            }
            analogWrite(PIN_MODE_LIGHT, buttonLight);
            analogWrite(PIN_BUTTON_LIGHT, buttonLight);
        }
        delay(stepValue);
        delayValue -= stepValue;
    }
    return 0;
}

// Perform the actual countdown operation.
// Return true if the user wants to countdown again (big button)
// Return false if the user times out and we want to go back to the attract loop
bool countdownLoop()
{
    unsigned char pressed = 0;
    currentTime = Modes[counterMode].maxTime;
    while (currentTime > 0)
    {
        printTime();
        if (checkButtonDelay(1000, 5, 1))
        {
            pressed = 1;
            break;
        }
        currentTime--;
    }
    if (!pressed)
    {
        // Print failure stars, then "Push"
        printFail();
        return;
    } else {
        digitalWrite(PIN_MODE_LIGHT, LOW);
        digitalWrite(PIN_BUTTON_LIGHT, LOW);
        // Blink remaining time for a bit
        alpha4.setBrightness(4);
        for (int i = 0; i < 4; i++)
        {
            printTime();
            if (checkButtonDelay(500, 5, 0))
                return true;
            alpha4.clear();
            alpha4.writeDisplay();
            if (checkButtonDelay(500, 5, 0))
                return true;
        }
        return false;
    }
}

void loop() 
{
    attractLoop();
    while (countdownLoop())
    {
    }
}
