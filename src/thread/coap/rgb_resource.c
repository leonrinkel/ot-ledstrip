#include "rgb_resource.h"

#include "thread_utils.h"

#include "ledstrip.h"
#include "ledstrip_drv.h"


static void coap_rgb_handler(void*, otMessage*, const otMessageInfo*);
static void coap_rgb_put_handler(void*, otMessage*, const otMessageInfo*);


otCoapResource coap_rgb_resource = {
    .mUriPath = "rgb",
    .mHandler = coap_rgb_handler,
    .mContext = NULL,
    .mNext    = NULL
};


void coap_rgb_handler(
    void*                p_context,
    otMessage*           p_message,
    const otMessageInfo* p_message_info
) {

    if (
        otCoapMessageGetType(p_message) != OT_COAP_TYPE_CONFIRMABLE &&
        otCoapMessageGetType(p_message) != OT_COAP_TYPE_NON_CONFIRMABLE
    ) return;

    switch (otCoapMessageGetCode(p_message))
    {
    case OT_COAP_CODE_PUT:
        coap_rgb_put_handler(p_context, p_message, p_message_info);
        return;

    default: return;
    }

}


static void coap_rgb_put_handler(
    void*                p_context,
    otMessage*           p_message,
    const otMessageInfo* p_message_info
) {

    char rgbhex[7] = { 0 };
    if (otMessageRead(p_message, otMessageGetOffset(
            p_message), &rgbhex, 6) != 6) return;
    ledstrip_set_rgbhex(&ledstrip, (const char*) &rgbhex);

    if (otCoapMessageGetType(p_message) == OT_COAP_TYPE_CONFIRMABLE) {

        otError error;
        otMessage* p_response;
        otInstance* p_instance = thread_ot_instance_get();

        p_response = otCoapNewMessage(p_instance, NULL);
        if (p_response == NULL) return;

        error = otCoapMessageInitResponse(p_response, p_message,
            OT_COAP_TYPE_ACKNOWLEDGMENT, OT_COAP_CODE_CHANGED);
        if (error != OT_ERROR_NONE) {
            otMessageFree(p_response);
            return;
        }

        error = otCoapSendResponse(p_instance,
            p_response, p_message_info);
        if (error != OT_ERROR_NONE) {
            otMessageFree(p_response);
            return;
        }

    }

}
