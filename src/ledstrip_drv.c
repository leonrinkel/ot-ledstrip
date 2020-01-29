#include "ledstrip_drv.h"

#include "mem_manager.h"

#define BITS_PER_LED 24

#define PWM_COMPARE_VALUE 20
#define PWM_DUTY_CYCLE_0  (0x8000 |  5UL)
#define PWM_DUTY_CYCLE_1  (0x8000 | 13UL)

static void dummy_pwm_handler(nrf_drv_pwm_evt_type_t event_type) {}

static const uint8_t gamma_correction[] =
{
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   1,   1,   1,   1,
    1,   1,   1,   1,   1,   1,   1,   1,
    1,   2,   2,   2,   2,   2,   2,   2,
    2,   3,   3,   3,   3,   3,   3,   3,
    4,   4,   4,   4,   4,   5,   5,   5,
    5,   6,   6,   6,   6,   7,   7,   7,
    7,   8,   8,   8,   9,   9,   9,   10,
    10,  10,  11,  11,  11,  12,  12,  13,
    13,  13,  14,  14,  15,  15,  16,  16,
    17,  17,  18,  18,  19,  19,  20,  20,
    21,  21,  22,  22,  23,  24,  24,  25,
    25,  26,  27,  27,  28,  29,  29,  30,
    31,  32,  32,  33,  34,  35,  35,  36,
    37,  38,  39,  39,  40,  41,  42,  43,
    44,  45,  46,  47,  48,  49,  50,  50,
    51,  52,  54,  55,  56,  57,  58,  59,
    60,  61,  62,  63,  64,  66,  67,  68,
    69,  70,  72,  73,  74,  75,  77,  78,
    79,  81,  82,  83,  85,  86,  87,  89,
    90,  92,  93,  95,  96,  98,  99,  101,
    102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127,
    129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158,
    160, 162, 164, 167, 169, 171, 173, 175,
    177, 180, 182, 184, 186, 189, 191, 193,
    196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233,
    236, 239, 241, 244, 247, 249, 252, 255
};

ret_code_t ledstrip_init(
    ledstrip_t*      p_ledstrip,
    ledstrip_conf_t* p_ledstrip_conf
) {

    ret_code_t ret_code;

    p_ledstrip->num_leds = p_ledstrip_conf->num_leds;
    p_ledstrip->pwm_instance = p_ledstrip_conf->pwm_instance;

    p_ledstrip->p_pwm_sequence_values = (uint16_t*) nrf_malloc(
        BITS_PER_LED * p_ledstrip_conf->num_leds);
    if (!p_ledstrip->p_pwm_sequence_values) return NRF_ERROR_NO_MEM;

    nrf_drv_pwm_config_t const pwm_config = {
        .output_pins  = { p_ledstrip_conf->output_pin },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_16MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = PWM_COMPARE_VALUE,
        .load_mode    = NRF_PWM_LOAD_COMMON,
        .step_mode    = NRF_PWM_STEP_AUTO
    };

    ret_code = nrf_drv_pwm_init(&p_ledstrip->pwm_instance,
        &pwm_config, dummy_pwm_handler);
    if (ret_code != NRF_SUCCESS) return ret_code;

    return NRF_SUCCESS;

}

ret_code_t ledstrip_rgb(
    ledstrip_t* p_ledstrip,
    uint8_t     r,
    uint8_t     g,
    uint8_t     b
) {

    for (int i = 0; i < p_ledstrip->num_leds; i++) {
        for (int j = 0; j < 8; j++) {
            p_ledstrip->p_pwm_sequence_values[i * BITS_PER_LED + j +  0] =
                (gamma_correction[g] & (1 << (7-j))) ?
                    PWM_DUTY_CYCLE_1 : PWM_DUTY_CYCLE_0;
            p_ledstrip->p_pwm_sequence_values[i * BITS_PER_LED + j +  8] =
                (gamma_correction[r] & (1 << (7-j))) ?
                    PWM_DUTY_CYCLE_1 : PWM_DUTY_CYCLE_0;
            p_ledstrip->p_pwm_sequence_values[i * BITS_PER_LED + j + 16] =
                (gamma_correction[b] & (1 << (7-j))) ?
                    PWM_DUTY_CYCLE_1 : PWM_DUTY_CYCLE_0;
        }
    }

    nrf_pwm_sequence_t const pwm_sequence = {
        .values = { .p_common = p_ledstrip->p_pwm_sequence_values },
        .length = BITS_PER_LED * p_ledstrip->num_leds,
        .repeats = 0,
        .end_delay = 0
    };

    return nrfx_pwm_simple_playback(&p_ledstrip->pwm_instance,
        &pwm_sequence, 1, NRFX_PWM_FLAG_STOP);

}
