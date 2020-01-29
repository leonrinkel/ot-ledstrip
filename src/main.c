/**
 * Copyright (c) 2017 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup cli_example_main main.c
 * @{
 * @ingroup cli_example
 * @brief An example presenting OpenThread CLI.
 *
 */

#include "app_scheduler.h"
#include "app_timer.h"
#include "bsp_thread.h"
#include "mem_manager.h"
#include "nrf_log_ctrl.h"
#include "nrf_log.h"
#include "nrf_log_default_backends.h"

#include "thread_coap_utils.h"
#include "thread_utils.h"

#include <openthread/thread.h>

#include "ledstrip.h"
#include "ledstrip_drv.h"

#define SCHED_QUEUE_SIZE      32                              /**< Maximum number of events in the scheduler queue. */
#define SCHED_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum app_scheduler event size. */


static void bsp_event_handler(bsp_event_t event)
{
    switch(event)
    {
        default:
            return;
    }
}


/***************************************************************************************************
 * @section State
 **************************************************************************************************/

static void thread_state_changed_callback(uint32_t flags, void * p_context)
{
    if (flags & OT_CHANGED_THREAD_ROLE)
    {
        switch (otThreadGetDeviceRole(p_context))
        {
            case OT_DEVICE_ROLE_CHILD:
            case OT_DEVICE_ROLE_ROUTER:
            case OT_DEVICE_ROLE_LEADER:
                break;

            case OT_DEVICE_ROLE_DISABLED:
            case OT_DEVICE_ROLE_DETACHED:
            default:
                thread_coap_utils_provisioning_enable_set(false);
                break;
        }
    }

    NRF_LOG_INFO("State changed! Flags: 0x%08x Current role: %d\r\n",
                 flags, otThreadGetDeviceRole(p_context));
}

/***************************************************************************************************
 * @section Initialization
 **************************************************************************************************/

/**@brief Function for initializing the Application Timer Module.
 */
static void timer_init(void)
{
    uint32_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing the Thread Board Support Package.
 */
static void thread_bsp_init(void)
{
    uint32_t error_code = bsp_init(BSP_INIT_LEDS, bsp_event_handler);
    APP_ERROR_CHECK(error_code);

    error_code = bsp_thread_init(thread_ot_instance_get());
    APP_ERROR_CHECK(error_code);
}


/**@brief Function for initializing the Thread Stack.
 */
static void thread_instance_init(void)
{
    thread_configuration_t thread_configuration =
    {
        .radio_mode        = THREAD_RADIO_MODE_RX_ON_WHEN_IDLE,
        .autocommissioning = true,
        .autostart_disable = false,
    };

    thread_init(&thread_configuration);
    thread_cli_init();
    thread_state_changed_callback_set(thread_state_changed_callback);

    uint32_t err_code = bsp_thread_init(thread_ot_instance_get());
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the Constrained Application Protocol Module.
 */
static void thread_coap_init(void)
{
    thread_coap_utils_configuration_t thread_coap_configuration =
    {
        .coap_server_enabled               = true,
        .coap_client_enabled               = false,
        .configurable_led_blinking_enabled = false,
    };

    thread_coap_utils_init(&thread_coap_configuration);
}


/**@brief Function for deinitializing the Thread Stack.
 *
 */
static void thread_instance_finalize(void)
{
    bsp_thread_deinit(thread_ot_instance_get());
    thread_soft_deinit();
}


/**@brief Function for initializing scheduler module.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


static void mem_manager_init(void)
{
    ret_code_t ret_code = nrf_mem_init();
    APP_ERROR_CHECK(ret_code);
}


/***************************************************************************************************
 * @section Main
 **************************************************************************************************/

int main(int argc, char *argv[])
{
    ret_code_t ret_code;

    log_init();
    scheduler_init();
    timer_init();
    mem_manager_init();

    ledstrip_conf_t ledstrip_conf = {
        .num_leds     = 30,
        .output_pin   = 31,
        .pwm_instance = NRF_DRV_PWM_INSTANCE(0)
    };

    ret_code = ledstrip_init(&ledstrip, &ledstrip_conf);
    APP_ERROR_CHECK(ret_code);

    while (true)
    {
        // Initialize the Thread stack.
        thread_instance_init();
        thread_coap_init();
        thread_bsp_init();

        while (!thread_soft_reset_was_requested())
        {
            thread_process();
            app_sched_execute();

            if (NRF_LOG_PROCESS() == false)
            {
                thread_sleep();
            }
        }

        thread_instance_finalize();
    }
}

/**
 *@}
 **/
