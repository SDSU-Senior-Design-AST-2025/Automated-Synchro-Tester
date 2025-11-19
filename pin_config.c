#include "pin_config.h"
#include <stdint.h>
#include <stdio.h>

uint64_t DDR = 0; // Data Direction Register, 0 = input, 1 = output


void setOutput(uint8_t pin) {
    DDR |= ((uint64_t)1 << pin);
}

void setInput(uint8_t pin) {
    DDR &= ~((uint64_t)1 << pin); // clear bit ? input
}

void configurePins(void) {
    uint8_t outputPins[] = {
        12,13,14,15,16,17,19,28,29,30,31,32,33,36,37,38,39,45,46,47,48,49
    };
    int numPins = sizeof(outputPins)/sizeof(outputPins[0]);

    for (int i = 0; i < numPins; i++) {
        setOutput(outputPins[i]);
    }

    uint8_t inputPins[] = {9, 10, 25, 26};
    int numInputs = sizeof(inputPins)/sizeof(inputPins[0]);

    for (int i = 0; i < numInputs; i++) {
        setInput(inputPins[i]);
    }
}
