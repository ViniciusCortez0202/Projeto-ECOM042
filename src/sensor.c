#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <errno.h>

#include "sensor.h"
#include "radar_processing.h"

LOG_MODULE_DECLARE(radar);

// Define pinos do GPIO
#define SENSOR_GPIO_NODE DT_NODELABEL(gpio0)
#define SENSOR1_PIN      5
#define SENSOR2_PIN      6

//Fila de mensagem para comunicação do GIOP
K_MSGQ_DEFINE(sensor_msgq, sizeof(struct sensor_event), 16, 4);

static const struct device *sensor_gpio_dev = DEVICE_DT_GET(SENSOR_GPIO_NODE);
static struct gpio_callback sensor_cb;


void sensor_send_event(uint8_t sensor_id, uint8_t level)
{
    struct sensor_event evt = {
        .sensor_id = sensor_id,
        .level = level,
        .timestamp_ms = k_uptime_get_32(),
    };
    
    k_msgq_put(&sensor_msgq, &evt, K_FOREVER);
}

//Função de interupção para capturar os valores dos pinos
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

// Thread responsável por capturar os valores da fila de mensagem enviada pela interupção do sensores. Após isso, envia para o proessamento
static void sensor_thread(void *arg1, void *arg2, void *arg3)
{
    struct sensor_event evt;

    ARG_UNUSED(arg1);
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    LOG_INF("Iniciando thread do sensor");

    while (1) {        
        if (k_msgq_get(&sensor_msgq, &evt, K_FOREVER) == 0) {           
            radar_process_sensor_event(&evt);
        }
    }
}

K_THREAD_DEFINE(sensor_thread_id,
                2048,
                sensor_thread,
                NULL, NULL, NULL,
                5, 0, 0);

// Inicializador dos sensores
int sensor_init(void)
{
    int ret;

    if (!device_is_ready(sensor_gpio_dev)) {
    LOG_ERR("Sensor GPIO nao está pronto");
        return -ENODEV;
    }
    
    ret = gpio_pin_configure(sensor_gpio_dev, SENSOR1_PIN, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Falha em SENSOR1_PIN (err %d)", ret);
        return ret;
    }

    ret = gpio_pin_configure(sensor_gpio_dev, SENSOR2_PIN, GPIO_INPUT);
    if (ret < 0) {
        LOG_ERR("Falha em SENSOR2_PIN (err %d)", ret);
        return ret;
    }
    
    ret = gpio_pin_interrupt_configure(sensor_gpio_dev, SENSOR1_PIN,
                                       GPIO_INT_EDGE_RISING);
    if (ret < 0) {
        LOG_ERR("Falha em interupção para SENSOR1_PIN (err %d)", ret);
        return ret;
    }

    ret = gpio_pin_interrupt_configure(sensor_gpio_dev, SENSOR2_PIN,
                                       GPIO_INT_EDGE_RISING);
    if (ret < 0) {
        LOG_ERR("Falha em interupção para SENSOR2_PIN (err %d)", ret);
        return ret;
    }
    
    //Configura interupção
    gpio_init_callback(&sensor_cb,
                       sensor_gpio_isr,
                       BIT(SENSOR1_PIN) | BIT(SENSOR2_PIN));

    gpio_add_callback(sensor_gpio_dev, &sensor_cb);

    LOG_INF("Sensores configurados");

    return 0;
}
