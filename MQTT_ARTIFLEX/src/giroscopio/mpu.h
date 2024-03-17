#include <esp_timer.h>
#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <math.h>

#include "sdkconfig.h"


void mpu6050(float *global_Angulo_y1, float *global_Angulo_y2);
