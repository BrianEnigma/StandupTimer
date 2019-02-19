#define PIN_MODE_LIGHT 3
#define PIN_BUTTON_LIGHT 5
#define PIN_MODE 4
#define PIN_BUTTON 6

void setup() 
{
    pinMode(PIN_MODE_LIGHT, OUTPUT);
    pinMode(PIN_BUTTON_LIGHT, OUTPUT);
    pinMode(4, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);
}

void loop() 
{
    digitalWrite(PIN_MODE_LIGHT, HIGH);
    digitalWrite(PIN_BUTTON_LIGHT, HIGH);
    delay(1000);
    digitalWrite(PIN_MODE_LIGHT, LOW);
    digitalWrite(PIN_BUTTON_LIGHT, LOW);
    delay(1000);
}
