#include "drawing.h"
#include "audio_capture.h"
#include "cairo.h"
#include "glib.h"
#include "gtk/gtk.h"
#include "utils.h"

static void
draw_bars(cairo_t* cr, int x, int y, int width, int height, bool flipped, float bars[BAR_COUNT]);

void draw(GtkDrawingArea* area, cairo_t* cr, int width, int height, gpointer user_data) {
    AppState* app_state = (AppState*)user_data;

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.1);

    draw_bars(cr, 0, 0, width, height, false, app_state->bar_heights.left);
    draw_bars(cr, width, 0, width, height, true, app_state->bar_heights.right);
    // cairo_fill(cr);
}

static void
draw_bars(cairo_t* cr, int x, int y, int width, int height, bool flipped, float bars[BAR_COUNT]) {
    cairo_save(cr);

    cairo_translate(cr, x, y + (int)(height / 2.0f));

    float bar_thickness = (float)height / 2.0f / (int)BAR_COUNT;
    float max_width = (float)width / 3.0f;

    cairo_move_to(cr, 0, 0);

    for (int i = 0; i < BAR_COUNT; i++) {
        float pos_y = i * bar_thickness;

        float percentage = bars[i] / (FRAMES / 4.0f);
        percentage *= SENSITIVITY;
        if (percentage > 1.0f)
            percentage = 1.0f;

        float bar_width = percentage * max_width;
        bar_width = flipped ? -bar_width : bar_width;

        cairo_line_to(cr, (int)bar_width, (int)pos_y);
    }

    float bottom_y = (int)BAR_COUNT * bar_thickness;
    cairo_line_to(cr, 0, (int)bottom_y);
    cairo_close_path(cr);

    // mirror bottom half
    cairo_path_t* saved_path = cairo_copy_path(cr);
    cairo_scale(cr, 1.0, -1.0);
    cairo_append_path(cr, saved_path);
    cairo_fill(cr);
    cairo_path_destroy(saved_path);
    cairo_restore(cr);
}
