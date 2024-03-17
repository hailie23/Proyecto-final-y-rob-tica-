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
// #include "giroscopio/mpu.h"

esp_mqtt_client_handle_t Client;
extern typedefFLAGS Flag;
// float global_Angulo_y1, global_Angulo_y2;
static char tag[] = "main.c";

// Estructura para pasar los ángulos como parámetro a las tareas
// typedef struct {
//     float *angulo_y1;
//     float *angulo_y2;
// } Angulos;

// Variable global para almacenar los datos del MQTT
char mqtt_read[100];
float Valormin; //Angulo minimo de flexion.
float Valormax; //Angulo maximo de flexion.
int Valorestado;//operacion de movimiento 1 - "si", 2-"no";
int Repeticiones;
// Función para leer los datos del MQTT y almacenarlos en una variable
void read_mqtt_data(char *topic, char *data);
/*
void process_mqtt_data(char *topic, char *data) {
    float mqtt_read = atof(data);
    // Realizar acciones basadas en el tópico recibido
    if (strcmp(topic, "ESPlimiterodillamin") == 0) {
        Valormin=mqtt_read;
        ESP_LOGI(tag, "Valormin: %f", Valormin);
    } else if (strcmp(topic, "ESPlimiterodillamax") == 0) {
        Valormax=mqtt_read;
        ESP_LOGI(tag, "Valormax: %f", Valormax);
    }else if (strcmp(topic, "ESPempezarterapia") == 0) {
        Valorestado=mqtt_read;
        ESP_LOGI(tag, "Valormax: %f", Valorestado);
    }else if (strcmp(topic, "ESPrepeticiones") == 0) {
        Repeticiones=mqtt_read;
        ESP_LOGI(tag, "Valormax: %f", Repeticiones);
    }
}*/
// Función de tarea para leer los datos del giroscopio y actualizar los ángulos
// void mpu_task(void *pvParameters) {
//     Angulos *angulos = (Angulos *)pvParameters;

//     while(1) {
//         mpu6050(angulos->angulo_y1, angulos->angulo_y2);
//         vTaskDelay(50 / portTICK_PERIOD_MS);
//     }
// }


// Función de tarea para publicar los ángulos por MQTT
void mqtt_task(void *pvParameters) {
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)pvParameters;
    char mqtt_data[50];

    while(1) {
        
        esp_mqtt_client_publish(client, "esp_rodilla", "muy duro", 0, 1, 0);

        vTaskDelay(4000 / portTICK_PERIOD_MS);  // Publicar cada medio segundo
    }
}

void app_main(void) {

    // Crear una estructura para pasar los ángulos como parámetro a las tareas
   
    // Inicializar WiFi y esperar hasta que esté listo
    wifi_init_sta();
    while(!Flag.MQTT_READY);
    Client = Get_client();

    // Crear la tarea para publicar los datos por MQTT
    xTaskCreate(mqtt_task, "mqtt_task", 2048, (void *)Client, 5, NULL);
    
    
    
    


}