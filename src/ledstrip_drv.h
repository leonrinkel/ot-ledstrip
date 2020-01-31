#ifndef LEDSTRIP_DRV_H__
#define LEDSTRIP_DRV_H__

#include "sdk_common.h"
#include "nrf_drv_pwm.h"

typedef struct {
    uint8_t       num_leds;
    nrf_drv_pwm_t pwm_instance;
    uint16_t*     p_pwm_sequence_values;
    uint32_t      color;
} ledstrip_t;

typedef struct {
    uint8_t       num_leds;
    uint8_t       output_pin;
    nrf_drv_pwm_t pwm_instance;
} ledstrip_conf_t;

ret_code_t ledstrip_init(
    ledstrip_t*      p_ledstrip,
    ledstrip_conf_t* p_ledstrip_conf
);

ret_code_t ledstrip_set_rgbhex(
    ledstrip_t* p_ledstrip,
    const char* rgbhex
);

#endif /* LEDSTRIP_DRV_H__ */
