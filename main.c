#include <fftw3.h>
#include <gtk/gtk.h>
#include <gtk4-layer-shell/gtk4-layer-shell.h>
#include <pthread.h>
#include <pulse/simple.h>
#include "audio_capture.h"
#include "drawing.h"
#include "utils.h"

int main(int argc, char** argv) {
    AppState app_state = {0};
    pthread_t audio_capture_thread;
    pthread_mutex_init(&app_state.mutex, NULL);

    audio_capture_init(&app_state.audio_data);

    pthread_create(&audio_capture_thread, NULL, audio_process, &app_state);

    GtkApplication* app =
        gtk_application_new("com.collmike.audio-visual", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), &app_state);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    pthread_mutex_destroy(&app_state.mutex);
    free_audio_data(&app_state.audio_data);
    return status;
    return 0;
}
