#ifndef DRAWING_H
#define DRAWING_H

#include "audio_capture.h"
#include "glib.h"
#include "gtk/gtk.h"

#define SENSITIVITY 10.0f

typedef struct BarHeights {
    float left[BINS];
    float right[BINS];
} BarHeights;

static gboolean on_tick(GtkWidget* widget, GdkFrameClock* frame_clock, gpointer user_data);
static void draw(GtkDrawingArea* area, cairo_t* cr, int width, int height, gpointer user_data);
static void
draw_bars(cairo_t* cr, int x, int y, int width, int height, bool flipped, float bars[BINS]);
void activate(GtkApplication* app, BarHeights* bar_heights);

#endif
