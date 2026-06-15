#ifndef DRAWING_H
#define DRAWING_H

#include "audio_capture.h"
#include "glib.h"
#include "gtk/gtk.h"

#define SENSITIVITY 2.0f

typedef struct AppState AppState;

typedef struct BarHeights {
    float left[BAR_COUNT];
    float right[BAR_COUNT];
} BarHeights;

void draw(GtkDrawingArea* area, cairo_t* cr, int width, int height, gpointer user_data);

#endif
