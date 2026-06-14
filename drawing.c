#include "drawing.h"
#include <stdio.h>
#include "audio_capture.h"
#include "glib.h"
#include "gtk/gtk.h"
#include "gtk4-layer-shell.h"

static gboolean on_tick(GtkWidget* widget, GdkFrameClock* frame_clock, gpointer user_data) {
    gtk_widget_queue_draw(widget);

    return G_SOURCE_CONTINUE;
}

static void draw(GtkDrawingArea* area, cairo_t* cr, int width, int height, gpointer user_data) {
    BarHeights* bar_heights = user_data;

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);

    draw_bars(cr, 0, 0, width, height, false, bar_heights->left);
    draw_bars(cr, width, 0, width, height, true, bar_heights->right);
    cairo_fill(cr);
}

static void
draw_bars(cairo_t* cr, int x, int y, int width, int height, bool flipped, float bars[BINS]) {
    float bar_thickness = (float)height / BINS;

    int offset = 10;

    for (int i = offset; i < BINS / 2 + offset; i++) {
        float pos_y = (float)height / 2 + i * bar_thickness - offset * 2;
        float flipped_pos_y = (float)height / 2 - i * bar_thickness + offset * 2;
        float raw_volume = bars[i];

        float percentage = raw_volume / (FRAMES / 4.0f);

        percentage *= SENSITIVITY;

        if (percentage > 1.0f) {
            percentage = 1.0f;
        }

        float max_pixel_width = (float)width / 5.0f;
        float bar_width = percentage * max_pixel_width;

        bar_width = flipped ? -bar_width : bar_width;

        cairo_rectangle(cr, x, pos_y + y, bar_width, bar_thickness - 1.0f);
        cairo_rectangle(cr, x, flipped_pos_y + y, bar_width, bar_thickness - 1.0f);
    }
}

void activate(GtkApplication* app, BarHeights* bar_heights) {
    GtkCssProvider* css_provider = gtk_css_provider_new();
    const char* css_data =
        "window {\n"
        "  background-color: rgba(0, 0, 0, 0.0);\n"
        "  background-image: none;\n"
        "  box-shadow: none;\n"
        "}\n";
    gtk_css_provider_load_from_string(css_provider, css_data);
    gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                               GTK_STYLE_PROVIDER(css_provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css_provider);

    GtkWindow* gtk_window = GTK_WINDOW(gtk_application_window_new(app));

    gtk_layer_init_for_window(gtk_window);

    gtk_layer_set_layer(gtk_window, GTK_LAYER_SHELL_LAYER_BACKGROUND);

    // We don't need to get keyboard input
    // gtk_layer_set_keyboard_mode(gtk_window, GTK_LAYER_SHELL_KEYBOARD_MODE_NONE); // NONE is
    // default

    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_LEFT, 0);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_RIGHT, 0);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_TOP, 0);
    gtk_layer_set_margin(gtk_window, GTK_LAYER_SHELL_EDGE_BOTTOM, 0);

    static const gboolean anchors[] = {TRUE, TRUE, TRUE, TRUE};
    for (int i = 0; i < GTK_LAYER_SHELL_EDGE_ENTRY_NUMBER; i++) {
        gtk_layer_set_anchor(gtk_window, i, anchors[i]);
    }

    // gtk_window_set_default_size(gtk_window, 200, 1080);
    // gtk_window_set_resizable(gtk_window, FALSE);

    GtkWidget* draw_area = gtk_drawing_area_new();
    gtk_widget_add_tick_callback(draw_area, on_tick, bar_heights, NULL);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(draw_area), draw, bar_heights, NULL);

    gtk_window_set_child(gtk_window, draw_area);
    gtk_window_present(gtk_window);
}
