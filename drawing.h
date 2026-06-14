#ifndef DRAWING_H
#define DRAWING_H

#include "audio_capture.h"
#include "glib.h"
#include "gtk/gtk.h"

#define SENSITIVITY 4.0f

typedef struct BarHeights {
    float left[BAR_COUNT];
    float right[BAR_COUNT];
} BarHeights;

static gboolean on_tick(GtkWidget* widget, GdkFrameClock* frame_clock, gpointer user_data);

static void draw(GtkDrawingArea* area, cairo_t* cr, int width, int height, gpointer user_data);

static void
draw_bars(cairo_t* cr, int x, int y, int width, int height, bool flipped, float bars[BAR_COUNT]);

void activate(GtkApplication* app, BarHeights* bar_heights);

static void create_window_for_monitor(GtkApplication* app,
                                      GdkMonitor* monitor,
                                      BarHeights* bar_heights);

static void on_monitors_changed(GListModel* monitors,
                                guint position,
                                guint removed,
                                guint added,
                                gpointer user_data);

#endif
