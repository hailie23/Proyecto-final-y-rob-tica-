#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include <esp_timer.h>
#include <driver/i2c.h>
#include <math.h>

#include "sdkconfig.h"

#include "conexion_wifi/wifi1.h"
#include "conexion_mqtt/mqtt1.h"
#include "giroscopio/mpu.h"

esp_mqtt_client_handle_t Client;
extern typedefFLAGS Flag;
static char tag[] = "main.c";
int count=0;
char data[100];
float global_Angulo_y1, global_Angulo_y2;

void app_main(void)
{
    
      
      wifi_init_sta();
      while(!Flag.MQTT_READY);
      Client=Get_client();

      while (1)
      {
        
        mpu6050(&global_Angulo_y1, &global_Angulo_y2);
        sprintf(data,"%f",global_Angulo_y1);
        ESP_LOGI(tag, "saliendo de bucle");
        esp_mqtt_client_publish(Client, "esprodilla", data, 0, 1, 0);
        vTaskDelay(10/portTICK_PERIOD_MS);
      }
      
}

