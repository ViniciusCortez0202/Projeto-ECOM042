#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "sensor.h"

LOG_MODULE_DECLARE(radar);

/* Thread that simulates vehicles generating pulses */
static void sensor_sim_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a);
    ARG_UNUSED(b);
    ARG_UNUSED(c);

    LOG_INF("Sensor simulation thread started");

    while (1) {
        /* Example: simulate a LIGHT vehicle (2 axles on sensor 1 + pass on sensor 2) */

        LOG_INF("Simulating LIGHT vehicle...");

        /* Axle 1 on sensor 1 */
        sensor_send_event(1, 1);
        k_sleep(K_MSEC(10));
        sensor_send_event(1, 0);

        /* Axle 2 on sensor 1 */
        k_sleep(K_MSEC(20));
        sensor_send_event(1, 1);
        k_sleep(K_MSEC(10));
        sensor_send_event(1, 0);

        /* Vehicle reaches sensor 2 (second loop) */
        k_sleep(K_MSEC(50));
        sensor_send_event(2, 1);
        k_sleep(K_MSEC(10));
        sensor_send_event(2, 0);

        /* Wait some time before next vehicle */
        k_sleep(K_SECONDS(2));
    }
}

/* Create the simulation thread */
K_THREAD_DEFINE(sensor_sim_thread_id,
                1024,
                sensor_sim_thread,
                NULL, NULL, NULL,
                6, 0, 0);
