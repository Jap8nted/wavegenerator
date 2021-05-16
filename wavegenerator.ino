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

void setup()
{
    analogWriteResolution(12);
}

void loop()
{
    analogWrite(DAC0, waveformsTable[0][i]);

    i++;
    if (i == maxSamplesNum)
    {
        i = 0;
    }
    // Delays 1000 uS
    delayMicroseconds(1000)
}