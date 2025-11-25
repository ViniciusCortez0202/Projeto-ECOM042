#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>

#include "radar_display.h"

#define ANSI_RED    "\x1b[31m"
#define ANSI_YELLOW "\x1b[33m"
#define ANSI_GREEN  "\x1b[32m"
#define ANSI_RESET  "\x1b[0m"

#if DT_NODE_EXISTS(DT_CHOSEN(zephyr_display))
#define DISPLAY_LABEL DT_LABEL(DT_CHOSEN(zephyr_display))
#else
#define DISPLAY_LABEL NULL
#endif

static const struct device *disp_dev;

int radar_display_init(void)
{
    if (DISPLAY_LABEL == NULL) {
        disp_dev = NULL;
        return -ENODEV;
    }

    disp_dev = device_get_binding(DISPLAY_LABEL);
    if (!disp_dev) {
        return -ENODEV;
    }

    if (!device_is_ready(disp_dev)) {
        disp_dev = NULL;
        return -ENODEV;
    }

    display_blanking_off(disp_dev);

    return 0;
}

static const char *vehicle_type_str(enum vehicle_type t)
{
    return (t == VEHICLE_TYPE_HEAVY) ? "PESADO" : "LEVE";
}

static const char *status_str(enum radar_infraction_status s)
{
    switch (s) {
    case RADAR_STATUS_NORMAL: return "NORMAL";
    case RADAR_STATUS_WARNING: return "ATENÇÃO";
    case RADAR_STATUS_INFRACTION: return "INFRAÇÃO";
    default: return "DESCONHECIDO";
    }
}

static const char *status_color(enum radar_infraction_status s)
{
    switch (s) {
    case RADAR_STATUS_NORMAL: return ANSI_GREEN;
    case RADAR_STATUS_WARNING: return ANSI_YELLOW;
    case RADAR_STATUS_INFRACTION: return ANSI_RED;
    default: return ANSI_RESET;
    }
}

static void update_dummy_display(const char *line)
{
    if (!disp_dev) {
        return;
    }

    uint8_t buf[64];
    size_t n = strlen(line);
    if (n > sizeof(buf)) n = sizeof(buf);

    memcpy(buf, line, n);

    struct display_buffer_descriptor desc = {
        .buf_size = n,
        .width = (uint16_t)n, 
        .height = 1,
        .pitch = (uint16_t)n,
    };

    display_write(disp_dev, 0, 0, &desc, buf);
}

void radar_display_show(const struct radar_display_data *data)
{
    if (!data) return;

    char line[128];

    snprintk(line, sizeof(line),
             "Veiculo=%s eixos=%u velocidade=%d limite=%d status=%s",
             vehicle_type_str(data->type),
             data->axle_count,
             data->measured_speed_kmh,
             data->limit_kmh,
             status_str(data->status));

    update_dummy_display(line);

    printk("%s[RADAR] %s%s\n",
           status_color(data->status),
           line,
           ANSI_RESET);
}
