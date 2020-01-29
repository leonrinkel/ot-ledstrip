#ifndef LEDSTRIP_DRV_H__
#define LEDSTRIP_DRV_H__

#include "sdk_common.h"
#include "nrf_drv_pwm.h"

typedef struct {
    uint8_t       num_leds;
    nrf_drv_pwm_t pwm_instance;
    uint16_t*     p_pwm_sequence_values;
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

ret_code_t ledstrip_rgb(
    ledstrip_t* p_ledstrip,
    uint8_t     r,
    uint8_t     g,
    uint8_t     b
);

#endif /* LEDSTRIP_DRV_H__ */
