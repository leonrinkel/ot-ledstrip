/**
 * Copyright (c) 2017-2019, Nordic Semiconductor ASA
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

#ifndef THREAD_COAP_UTILS_H__
#define THREAD_COAP_UTILS_H__

#include <stdbool.h>
#include <openthread/coap.h>

#include "thread_utils.h"


/**@brief Enumeration describing light commands. */
typedef enum
{
    THREAD_COAP_UTILS_LIGHT_CMD_OFF = '0',
    THREAD_COAP_UTILS_LIGHT_CMD_ON
} thread_coap_utils_light_command_t;


/**@brief Type definition of the function used to handle light resource change.
 */
typedef void (*thread_coap_utils_light_command_handler_t)(thread_coap_utils_light_command_t light_state);


/***************************************************************************************************
 * @section CoAP utils core functions.
 **************************************************************************************************/

/**@brief Function for initializing the CoAP service with specified resources.
 *
 * @details The @p thread_init function needs to be executed before calling this function.
 *
 */
void thread_coap_utils_init(void);


/**@brief Function for stopping the CoAP service. */
void thread_coap_utils_deinit(void);


#endif /* THREAD_COAP_UTILS_H__ */
