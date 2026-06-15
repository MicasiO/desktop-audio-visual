#ifndef UITLS_H
#define UITLS_H

#include "audio_capture.h"
#include "drawing.h"

// TODO: create config (color, sensitivity, bars/lines, filled/not filled, scaling, show on
// displays, hide when app opened)

typedef struct AppState {
    AudioData audio_data;
    BarHeights bar_heights;

    pthread_mutex_t data_mutex;

    bool data_changed;
    bool running;
} AppState;

void die(const char* msg);

#endif
