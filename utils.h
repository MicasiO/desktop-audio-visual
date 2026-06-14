#ifndef UITLS_H
#define UITLS_H

#include "audio_capture.h"
#include "drawing.h"

typedef struct AppState {
    AudioData audio_data;
    BarHeights bar_heights;
} AppState;

void die(const char* msg);

#endif
