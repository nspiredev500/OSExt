#ifndef ADC_H
#define ADC_H


extern const uint32_t CHANNEL_VBATT;
extern const uint32_t CHANNEL_VSYS;
extern const uint32_t CHANNEL_B12;
extern const uint32_t CHANNEL_KEYPAD;


double adc_read_channel(uint32_t channel);





















#endif