#include "audio_device.h"
#include <pthread.h>
#include <pulse/context.h>
#include <pulse/def.h>
#include <pulse/introspect.h>
#include <pulse/mainloop.h>
#include <pulse/subscribe.h>
#include "audio_capture.h"
#include "utils.h"

static void device_subscription_callback(pa_context* c,
                                         pa_subscription_event_type_t t,
                                         uint32_t idx,
                                         void* arg);
static void device_info_callback(pa_context* c, const pa_server_info* i, void* arg);
static void context_state_callback(pa_context* c, void* arg);

void* audio_device_listener(void* arg) {
    pa_mainloop* mainloop = pa_mainloop_new();
    if (mainloop == NULL) {
        die("pa_mainloop");
    }

    pa_context* context = pa_context_new(pa_mainloop_get_api(mainloop), "desktop-audio-visual");
    if (context == NULL) {
        pa_mainloop_free(mainloop);
        die("pa_context");
    }

    pa_context_set_state_callback(context, context_state_callback, arg);
    if (pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL) >= 0) {
        pa_mainloop_run(mainloop, NULL);
    }

    pa_context_disconnect(context);
    pa_context_unref(context);
    pa_mainloop_free(mainloop);

    return NULL;
}

static void device_info_callback(pa_context* c, const pa_server_info* i, void* arg) {
    AudioData* audio_data = (AudioData*)arg;
    if (!i || !i->default_sink_name) {
        return;
    }

    pthread_mutex_lock(&audio_data->device_mutex);

    if (audio_data->device_name) {
        free(audio_data->device_name);
    }

    audio_data->device_name = malloc(strlen(i->default_sink_name) + 9);
    if (audio_data->device_name) {
        sprintf(audio_data->device_name, "%s.monitor", i->default_sink_name);
        audio_data->device_changed = true;
    }

    pthread_mutex_unlock(&audio_data->device_mutex);
}

static void device_subscription_callback(pa_context* c,
                                         pa_subscription_event_type_t t,
                                         uint32_t idx,
                                         void* arg) {
    if ((t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK) == PA_SUBSCRIPTION_EVENT_SERVER) {
        pa_context_get_server_info(c, device_info_callback, arg);
    }
}

static void context_state_callback(pa_context* c, void* arg) {
    if (pa_context_get_state(c) == PA_CONTEXT_READY) {
        pa_context_get_server_info(c, device_info_callback, arg);
        pa_context_set_subscribe_callback(c, device_subscription_callback, arg);
        pa_context_subscribe(c, PA_SUBSCRIPTION_MASK_SERVER, NULL, NULL);
    }
}
