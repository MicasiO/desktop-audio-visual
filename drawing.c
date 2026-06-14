#include "drawing.h"
#include "audio_capture.h"
#include "cairo.h"
#include "gdk/gdk.h"
#include "gio/gio.h"
#include "glib-object.h"
#include "glib.h"
#include "glibconfig.h"
#include "gtk/gtk.h"
#include "gtk4-layer-shell.h"
#include "utils.h"

static gboolean on_tick(GtkWidget* widget, GdkFrameClock* frame_clock, gpointer user_data) {
    gtk_widget_queue_draw(widget);

    return G_SOURCE_CONTINUE;
}

static void draw(GtkDrawingArea* area, cairo_t* cr, int width, int height, gpointer user_data) {
    AppState* app_state = (AppState*)user_data;

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);

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

    // cairo_move_to(cr, 0, 0);

    for (int i = 0; i < BAR_COUNT; i++) {
        float pos_y = i * bar_thickness;

        float percentage = bars[i] / (FRAMES / 4.0f);
        percentage *= SENSITIVITY;
        if (percentage > 1.0f)
            percentage = 1.0f;

        float bar_width = percentage * max_width;
        bar_width = flipped ? -bar_width : bar_width;

        cairo_rectangle(cr, x, pos_y, (int)bar_width, bar_thickness);

        // if (i == 0) {
        //     cairo_move_to(cr, (int)bar_width, (int)pos_y);
        // } else {
        //     cairo_line_to(cr, (int)bar_width, (int)pos_y);
        // }
    }

    cairo_path_t* saved_path = cairo_copy_path(cr);

    cairo_scale(cr, 1.0, -1.0);

    cairo_append_path(cr, saved_path);

    cairo_fill(cr);

    cairo_path_destroy(saved_path);

    cairo_restore(cr);
}

static void create_window_for_monitor(GtkApplication* app,
                                      GdkMonitor* monitor,
                                      AppState* app_state) {
    GtkWindow* gtk_window = GTK_WINDOW(gtk_application_window_new(app));

    gtk_layer_init_for_window(gtk_window);

    gtk_layer_set_monitor(gtk_window, monitor);

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

    GtkWidget* draw_area = gtk_drawing_area_new();
    gtk_widget_add_tick_callback(draw_area, on_tick, app_state, NULL);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(draw_area), draw, app_state, NULL);

    gtk_window_set_child(gtk_window, draw_area);
    gtk_window_present(gtk_window);
}

void activate(GtkApplication* app, AppState* app_state) {
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

    g_object_set_data(G_OBJECT(app), "app_state", app_state);

    GdkDisplay* display = gdk_display_get_default();
    GListModel* monitors = gdk_display_get_monitors(display);

    g_signal_connect(monitors, "items-changed", G_CALLBACK(on_monitors_changed), app);

    for (guint i = 0; i < g_list_model_get_n_items(monitors); i++) {
        GdkMonitor* monitor = g_list_model_get_item(monitors, i);
        create_window_for_monitor(app, monitor, app_state);
        g_object_unref(monitor);
    }
}

static void on_monitors_changed(GListModel* monitors,
                                guint position,
                                guint removed,
                                guint added,
                                gpointer user_data) {
    GtkApplication* app = GTK_APPLICATION(user_data);

    AppState* app_state = g_object_get_data(G_OBJECT(app), "app_state");

    for (guint i = 0; i < added; i++) {
        GdkMonitor* new_monitor = g_list_model_get_item(monitors, position + i);

        create_window_for_monitor(app, new_monitor, app_state);

        g_object_unref(new_monitor);
    }
}
