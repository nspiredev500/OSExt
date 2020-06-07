#ifndef ADC_H
#define ADC_H


const uint32_t CHANNEL_VBATT;
const uint32_t CHANNEL_VSYS;
const uint32_t CHANNEL_B12;
const uint32_t CHANNEL_KEYPAD;


double adc_read_channel(uint32_t channel);





















#endif