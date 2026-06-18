#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include "gdk/gdk.h"

#define CONFIG_FILE_NAME "config"

typedef enum Side { SIDE_VERTICAL, SIDE_HORIZONTAL } Side;
typedef enum Style { STYLE_BARS, STYLE_LINES } Style;

typedef struct Config {
    GKeyFile* file;
    GdkRGBA color;
    Side side;
    bool mirror;
    bool fill;
    double sensitivity;
} Config;

void init_config(Config* config);

#endif
