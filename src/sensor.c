#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <errno.h>

#include "sensor.h"

/* Use the same log module as main.c */
LOG_MODULE_DECLARE(radar);

#define SENSOR_GPIO_NODE DT_NODELABEL(gpio0)
#define SENSOR1_PIN      5
#define SENSOR2_PIN      6

struct sensor_event {
    uint8_t sensor_id;     // 1 or 2
    uint8_t level;         // 0 or 1
    uint32_t timestamp_ms; // k_uptime_get_32()
};

/* Queue to pass events from ISR / simulator to sensor thread */
K_MSGQ_DEFINE(sensor_msgq, sizeof(struct sensor_event), 16, 4);

/* GPIO device + callback (for real GPIO path) */
static const struct device *sensor_gpio_dev = DEVICE_DT_GET(SENSOR_GPIO_NODE);
static struct gpio_callback sensor_cb;

void sensor_send_event(uint8_t sensor_id, uint8_t level)
{
    struct sensor_event evt = {
        .sensor_id = sensor_id,
        .level = level,
        .timestamp_ms = k_uptime_get_32(),
    };

    /* For simulation/testing it is fine to block if queue is full */
    k_msgq_put(&sensor_msgq, &evt, K_FOREVER);
}

/* GPIO ISR: called by the driver when a real edge happens on the pins */
static void sensor_gpio_isr(const struct device *dev,
                            struct gpio_callback *cb,
                            uint32_t pins)
{
    ARG_UNUSED(cb);

    if (pins & BIT(SENSOR1_PIN)) {
        uint8_t level = gpio_pin_get(dev, SENSOR1_PIN);
        sensor_send_event(1, level);
    }

    if (pins & BIT(SENSOR2_PIN)) {
        uint8_t level = gpio_pin_get(dev, SENSOR2_PIN);
        sensor_send_event(2, level);
    }
}

/* Thread that consumes events and prints them */
static void sensor_thread(void *arg1, void *arg2, void *arg3)
{
    struct sensor_event evt;

    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    LOG_INF("Sensor thread started");

    while (1) {
        /* Block until a new event arrives */
        if (k_msgq_get(&sensor_msgq, &evt, K_FOREVER) == 0) {
            LOG_INF("Sensor %d changed to %d at %u ms",
                    evt.sensor_id,
                    evt.level,
                    (unsigned int)evt.timestamp_ms);
        }
    }
}

/* Create the sensor thread */
K_THREAD_DEFINE(sensor_thread_id,
                1024,
                sensor_thread,
                NULL, NULL, NULL,
                5, 0, 0);

/* Public init function called from main.c */
int sensor_init(void)
{
    int ret;

    if (!device_is_ready(sensor_gpio_dev)) {
        LOG_ERR("Sensor GPIO device is not ready");
        return -ENODEV;
    }

    /* Configure pins as input (no pull-up: driver does not support it) */
    ret = gpio_pin_configure(sensor_gpio_dev, SENSOR1_PIN, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Failed to configure SENSOR1_PIN (err %d)", ret);
        return ret;
    }

    ret = gpio_pin_configure(sensor_gpio_dev, SENSOR2_PIN, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Failed to configure SENSOR2_PIN (err %d)", ret);
        return ret;
    }

    /* Interrupt on rising edge only */
    ret = gpio_pin_interrupt_configure(sensor_gpio_dev, SENSOR1_PIN,
                                       GPIO_INT_EDGE_RISING);
    if (ret < 0) {
        LOG_ERR("Failed to configure interrupt for SENSOR1_PIN (err %d)", ret);
        return ret;
    }

    ret = gpio_pin_interrupt_configure(sensor_gpio_dev, SENSOR2_PIN,
                                       GPIO_INT_EDGE_RISING);
    if (ret < 0) {
        LOG_ERR("Failed to configure interrupt for SENSOR2_PIN (err %d)", ret);
        return ret;
    }

    /* Register callback for both pins */
    gpio_init_callback(&sensor_cb,
                       sensor_gpio_isr,
                       BIT(SENSOR1_PIN) | BIT(SENSOR2_PIN));

    gpio_add_callback(sensor_gpio_dev, &sensor_cb);

    LOG_INF("GPIO sensors configured on pins %d and %d", SENSOR1_PIN, SENSOR2_PIN);
    LOG_INF("Waiting for sensor events...");

    return 0;
}
