#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "gdk/gdk.h"
#include "glib.h"
#include "utils.h"

// static void create_default_config();
static void parse_config(void);
static bool config_exists(void);
static char* config_location(void);

static void create_default_config(Config* config);

static void set_config_value_side(Config* config, Side side);
static void set_config_value_color(Config* config, const GdkRGBA color);
static void set_config_value_mirror(Config* config, const bool value);
static void set_config_value_fill(Config* config, const bool value);
static void set_config_value_sensitivity(Config* config, const double value);

static char* config_location(void) {
    const char* dir = g_get_user_config_dir();
    char* loc = g_build_filename(dir, "desktop-audio-visual", CONFIG_FILE_NAME, NULL);

    return loc;
}

static bool config_exists(void) {
    char* config_dir = config_location();

    bool exists = g_file_test(config_dir, G_FILE_TEST_EXISTS);

    free(config_dir);
    return exists;
}

static void set_config_value_color(Config* config, const GdkRGBA color) {
    GError* error = NULL;

    char* rgba_string = gdk_rgba_to_string(&color);
    g_key_file_set_string(config->file, "style", "color", rgba_string);
    free(rgba_string);

    char* loc = config_location();
    if (!g_key_file_save_to_file(config->file, loc, &error)) {
        die("g_key_file_save_to_file");
    }

    free(loc);

    config->color = color;
}

static void set_config_value_side(Config* config, Side side) {
    GError* error = NULL;

    switch (side) {
        case SIDE_VERTICAL:
            g_key_file_set_string(config->file, "style", "side", "v");
            break;
        case SIDE_HORIZONTAL:
            g_key_file_set_string(config->file, "style", "side", "h");
            break;
        default:
            break;
    }

    char* loc = config_location();
    if (!g_key_file_save_to_file(config->file, loc, &error)) {
        die("g_key_file_save_to_file");
    }

    free(loc);

    config->side = side;
}

static void set_config_value_fill(Config* config, const bool value) {
    GError* error = NULL;

    g_key_file_set_boolean(config->file, "style", "fill", value);

    char* loc = config_location();
    if (!g_key_file_save_to_file(config->file, loc, &error)) {
        die("g_key_file_save_to_file");
    }

    free(loc);

    config->fill = value;
}

static void set_config_value_mirror(Config* config, const bool value) {
    GError* error = NULL;

    g_key_file_set_boolean(config->file, "style", "mirror", value);

    char* loc = config_location();
    if (!g_key_file_save_to_file(config->file, loc, &error)) {
        die("g_key_file_save_to_file");
    }

    free(loc);

    config->mirror = value;
}

static void set_config_value_sensitivity(Config* config, const double value) {
    GError* error = NULL;

    g_key_file_set_double(config->file, "style", "sensitivity", value);

    char* loc = config_location();
    if (!g_key_file_save_to_file(config->file, loc, &error)) {
        die("g_key_file_save_to_file");
    }

    free(loc);

    config->mirror = value;
}

static void create_default_config(Config* config) {
    char* loc = config_location();
    char* loc_dir = g_path_get_dirname(loc);

    // create directory if doesn't exist
    if (g_mkdir_with_parents(loc_dir, 0755) != 0) {
        die("g_mkdir_with_parents");
    }

    free(loc_dir);
    free(loc);

    GKeyFile* key_file = g_key_file_new();
    config->file = key_file;

    set_config_value_mirror(config, true);
    set_config_value_fill(config, true);
    set_config_value_side(config, SIDE_HORIZONTAL);
    set_config_value_color(config, (GdkRGBA){1.0, 0.5, 1.0, 0.5});
    set_config_value_sensitivity(config, 2);
}

void init_config(Config* config) {
    if (config_exists()) {
        // load config
    } else {
        create_default_config(config);
    }
}
