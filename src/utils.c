#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "glib.h"

void die(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void free_app_state(AppState* app_state) {
    free_audio_data(&app_state->audio_data);
    g_key_file_free(app_state->config.file);
}
