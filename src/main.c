#include <fftw3.h>
#include <gtk/gtk.h>
#include <gtk4-layer-shell/gtk4-layer-shell.h>
#include <pthread.h>
#include <pulse/simple.h>
#include "app.h"
#include "audio_device.h"
#include "config.h"
#include "gio/gio.h"
#include "utils.h"

int main(int argc, char** argv) {
    AppState app_state = {0};
    app_state.running = true;

    init_config(&app_state.config);

    pthread_mutex_init(&app_state.audio_data.device_mutex, NULL);
    pthread_t audio_device_thread;
    pthread_create(&audio_device_thread, NULL, audio_device_listener, &app_state.audio_data);

    audio_capture_init(&app_state);

    pthread_mutex_init(&app_state.data_mutex, NULL);
    pthread_t audio_capture_thread;
    pthread_create(&audio_capture_thread, NULL, audio_process, &app_state);

    GtkApplication* app =
        gtk_application_new("com.collmike.desktop-audio-visual", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &app_state);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    app_state.running = false;
    pthread_join(audio_capture_thread, NULL);

    pthread_mutex_destroy(&app_state.data_mutex);

    free_app_state(&app_state);
    return status;
}
