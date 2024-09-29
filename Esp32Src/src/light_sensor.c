#include "light_sensor.h"

uint32_t voltage = 0;
void init_lightsensor()
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
}

int getRawLight(){
    return adc1_get_raw(ADC1_CHANNEL_6);
}