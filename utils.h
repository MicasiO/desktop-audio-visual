#ifndef UITLS_H
#define UITLS_H

#include "audio_capture.h"
#include "drawing.h"

// TODO: create config (color, sensitivity, bars/lines, filled/not filled, scaling, show on
// displays, hide when app opened)
// optimize

typedef struct AppState {
    AudioData audio_data;
    BarHeights bar_heights;
    bool data_changed;
    pthread_mutex_t mutex;
    cairo_surface_t* background_cache;
} AppState;

void die(const char* msg);

#endif
