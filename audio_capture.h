#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <fftw3.h>
#include <pulse/simple.h>

#define BUFF_SIZE 1024
#define FRAMES BUFF_SIZE
#define BINS (FRAMES / 2 + 1)

typedef struct AudioData {
    float left_audio_buffer[BUFF_SIZE];
    float right_audio_buffer[BUFF_SIZE];

    float* in;
    fftwf_complex* left_out;
    fftwf_complex* right_out;

    fftwf_plan left_plan;
    fftwf_plan right_plan;

    pa_simple* stream;
} AudioData;

void audio_capture_init(AudioData* data);
void audio_capture_buffer(AudioData* data);

void calc_fftw(fftwf_plan* plan,
               float* in,
               fftwf_complex* out,
               float audio_buffer[BUFF_SIZE],
               float bar_heights[BINS]);

void free_audio_data(AudioData* data);

void* audio_process(void* arg);

#endif
