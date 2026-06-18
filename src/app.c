#include "app.h"
#include "drawing.h"
#include "gtk/gtk.h"
#include "gtk4-layer-shell.h"
#include "utils.h"

static gboolean on_tick(GtkWidget* widget, GdkFrameClock* frame_clock, gpointer user_data);
static void create_window(GtkApplication* app, AppState* app_state);

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

    // g_object_set_data(G_OBJECT(app), "app_state", app_state);

    create_window(app, app_state);

    g_timeout_add_seconds(5, (GSourceFunc)g_application_quit, app);
}

static void create_window(GtkApplication* app, AppState* app_state) {
    GtkWindow* gtk_window = GTK_WINDOW(gtk_application_window_new(app));

    gtk_layer_init_for_window(gtk_window);
    // gtk_layer_set_monitor(gtk_window, monitor);
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

static gboolean on_tick(GtkWidget* widget, GdkFrameClock* frame_clock, gpointer user_data) {
    gtk_widget_queue_draw(widget);

    return G_SOURCE_CONTINUE;
}
