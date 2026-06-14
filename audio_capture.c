#include "audio_capture.h"
#include <math.h>
#include <pthread.h>
#include <pulse/def.h>
#include <pulse/error.h>
#include <pulse/sample.h>
#include <pulse/simple.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "utils.h"

void audio_capture_init(AudioData* data) {
    int error;
    static const pa_sample_spec ss = {.format = PA_SAMPLE_FLOAT32LE, .rate = 44100, .channels = 2};

    pa_buffer_attr attr;
    attr.maxlength = (uint32_t)-1;
    attr.tlength = (uint32_t)-1;
    attr.prebuf = (uint32_t)-1;
    attr.minreq = (uint32_t)-1;

    attr.fragsize = sizeof(float) * BUFF_SIZE * 2;

    data->stream = pa_simple_new(
        NULL, "audio-visual", PA_STREAM_RECORD,
        "alsa_output.usb-Lenovo_ThinkPad_USB-C_Dock_Audio_000000000000-00.analog-stereo.monitor",
        "capture", &ss, NULL, &attr, &error);
    if (data->stream == NULL) {
        die(pa_strerror(error));
    }

    data->left_out = fftwf_alloc_complex(BINS);
    data->right_out = fftwf_alloc_complex(BINS);

    data->in = fftwf_alloc_real(FRAMES);

    data->left_plan = fftwf_plan_dft_r2c_1d(FRAMES, data->in, data->left_out, FFTW_MEASURE);
    data->right_plan = fftwf_plan_dft_r2c_1d(FRAMES, data->in, data->right_out, FFTW_MEASURE);
    printf("stuff\n");
}

void audio_capture_buffer(AudioData* data) {
    int error;
    float buf[BUFF_SIZE * 2];

    if (pa_simple_read(data->stream, buf, sizeof(buf), &error) < 0) {
        die(pa_strerror(error));
    }

    for (int i = 0; i < BUFF_SIZE; i++) {
        int j = i * 2;
        data->left_audio_buffer[i] = buf[j];
        data->right_audio_buffer[i] = buf[j + 1];
    }
}

void calc_fftw(fftwf_plan* plan,
               float* in,
               fftwf_complex* out,
               float audio_buffer[BUFF_SIZE],
               float bar_heights[BINS]) {
    for (int i = 0; i < FRAMES; i++) {
        float sample = audio_buffer[i];

        float multiplier =
            0.5f * (1.0f - cosf(2.0f * (float)M_PI * (float)i / ((float)FRAMES - 1.0f)));

        in[i] = sample * multiplier;
    }

    fftwf_execute(*plan);
    for (int i = 0; i < BINS; i++) {
        float real = out[i][0];
        float imag = out[i][1];

        bar_heights[i] = sqrtf((real * real) + (imag * imag));
    }
}

void* audio_process(void* arg) {
    AppState* app_state = arg;
    AudioData* audio_data = &app_state->audio_data;

    float raw_bars_left[BINS] = {0};
    float raw_bars_right[BINS] = {0};

    while (true) {
        audio_capture_buffer(audio_data);

        calc_fftw(&audio_data->left_plan, audio_data->in, audio_data->left_out,
                  audio_data->left_audio_buffer, raw_bars_left);
        calc_fftw(&audio_data->right_plan, audio_data->in, audio_data->right_out,
                  audio_data->right_audio_buffer, raw_bars_right);

        scale_bars(raw_bars_left, app_state->bar_heights.left);
        scale_bars(raw_bars_right, app_state->bar_heights.right);
    }

    return NULL;
}

void scale_bars(float bars_in[BINS], float bars_out[BAR_COUNT]) {
    for (int i = 0; i < BAR_COUNT; i++) {
        float start_freq_percent = powf((float)i / (int)BAR_COUNT, 2.0f);
        float end_freq_percent = powf((float)(i + 1) / (int)BAR_COUNT, 2.0f);

        int start_bin = start_freq_percent * ((int)BINS - 1) + 1;
        int end_bin = end_freq_percent * ((int)BINS - 1) + 1;

        if (end_bin <= start_bin) {
            end_bin = start_bin + 1;
        }

        float sum = 0.0f;
        for (int j = start_bin; j < end_bin; j++) {
            sum += bars_in[j];
        }

        bars_out[i] = sum / (end_bin - start_bin);
    }
}

void free_audio_data(AudioData* data) {
    pa_simple_free(data->stream);
    fftwf_free(data->in);
    fftwf_destroy_plan(data->left_plan);
    fftwf_destroy_plan(data->right_plan);
    fftwf_free(data->left_out);
    fftwf_free(data->right_out);
}
