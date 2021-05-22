/*
Wave generator to create 60 Hz singals on arduino DUE 
Autor:
Jonatan Aponte

1. .ino files allways contain a setup call and a main loop which are call by default
*/

#include <Arduino.h>
#include "Waveform.h"
#define oneHzSample 1000000 / maxSamplesNum

int i = 0;
int frequency = 60;
void setup()
{
    analogWriteResolution(12);

    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
}

void loop()
{
    analogWrite(DAC0, (uint16_t)waveformsTable[0][i]);
    analogWrite(DAC1, (uint16_t)waveformsTable[0][i]);
    Serial.println((uint16_t)waveformsTable[0][i]);
    i++;
    if (i == maxSamplesNum)
    {
        i = 0;
    }
    if (i > waveformsTable[0][i])
    {
        digitalWrite(LED_BUILTIN, HIGH);
    }
    else
    {
        digitalWrite(LED_BUILTIN, LOW);
    }

    // Delays 1000 uS
    // Define frequency in Hertz
    // 1000

    delay(1000 / (maxSamplesNum * frequency));
}