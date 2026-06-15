#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <fftw3.h>
#include <pulse/simple.h>
#include <stdbool.h>

#define BUFF_SIZE 1024
#define FRAMES BUFF_SIZE
#define BINS (FRAMES / 2 + 1)

#define BAR_SCALE 10
#define BAR_COUNT (BINS / BAR_SCALE)

typedef struct AppState AppState;

typedef struct AudioData {
    float left_audio_buffer[BUFF_SIZE];
    float right_audio_buffer[BUFF_SIZE];

    pa_simple* stream;

    float* in;
    fftwf_complex* left_out;
    fftwf_complex* right_out;

    fftwf_plan left_plan;
    fftwf_plan right_plan;

    char* device_name;
    pthread_mutex_t device_mutex;
    bool device_changed;
} AudioData;

void audio_capture_init(AppState* app_state);

void* audio_process(void* arg);

void free_audio_data(AudioData* data);

#endif
