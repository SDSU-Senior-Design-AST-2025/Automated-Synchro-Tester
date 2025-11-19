#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <stdint.h>

extern uint64_t DDR;  // Exposed for other modules if needed

void setOutput(uint8_t pin);
void setInput(uint8_t pin);
void configurePins(void);  // wrapper for setting outputs/inputs

#endif
