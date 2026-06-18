#ifndef UITLS_H
#define UITLS_H

#include "audio_capture.h"
#include "config.h"
#include "drawing.h"

// TODO: create config (color, sensitivity, bars/lines, filled/not filled, scaling)

typedef struct AppState {
    Config config;

    AudioData audio_data;
    BarHeights bar_heights;

    pthread_mutex_t data_mutex;

    bool data_changed;
    bool running;
} AppState;

void die(const char* msg);

void free_app_state(AppState* app_state);

#endif
