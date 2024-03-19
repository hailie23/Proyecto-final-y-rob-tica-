#include "giroscopio/mpu.h"

#define PIN_SDA 21
#define PIN_CLK 22
#define I2C_ADDRESS1 0x68 // I2C address of MPU6050
#define I2C_ADDRESS2 0x69 // I2C address of MPU6050

#define A_R 16384.0 // 32768/2
#define G_R 131.0 // 32768/250
#define RAD_A_DEG 57.295779513082320876798154814105
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_PWR_MGMT_1   0x6B


/*
 * The following registers contain the primary data we are interested in
 * 0x3B MPU6050_ACCEL_XOUT_H
 * 0x3C MPU6050_ACCEL_XOUT_L
 * 0x3D MPU6050_ACCEL_YOUT_H
 * 0x3E MPU6050_ACCEL_YOUT_L
 * 0x3F MPU6050_ACCEL_ZOUT_H
 * 0x50 MPU6050_ACCEL_ZOUT_L
 * 0x41 MPU6050_TEMP_OUT_H
 * 0x42 MPU6050_TEMP_OUT_L
 * 0x43 MPU6050_GYRO_XOUT_H
 * 0x44 MPU6050_GYRO_XOUT_L
 * 0x45 MPU6050_GYRO_YOUT_H
 * 0x46 MPU6050_GYRO_YOUT_L
 * 0x47 MPU6050_GYRO_ZOUT_H
 * 0x48 MPU6050_GYRO_ZOUT_L
 */


static char tag[] = "mpu6050";

#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)   do { esp_err_t rc = (x); if (rc != ESP_OK) { ESP_LOGE("err", "esp_err_t = %d", rc); assert(0 && #x);} } while(0);

void mpu6050(float *global_Angulo_y1,float *global_Angulo_y2) {
    uint8_t data1[14];
	uint8_t data2[14];
	short accel_x1, accel_y1, accel_z1, gyro_y1;
	float ang_acc_y1, ang_giro_y1, Angulo_y1=0;
	long tiempo_prev=0;
	float dt;
	short accel_x2, accel_y2, accel_z2, gyro_y2;
	float ang_acc_y2, ang_giro_y2, Angulo_y2=0;
    
    
    ESP_LOGI(tag, "task_mpu6050 started");
	ESP_LOGI(tag, ">> mpu6050");
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = PIN_SDA;
	conf.scl_io_num = PIN_CLK;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 100000;
    conf.clk_flags=0;

	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

	i2c_cmd_handle_t cmd;
	vTaskDelay(200/portTICK_PERIOD_MS);

	// Configuración inicial del primer MPU6050
	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS1 << 1) | I2C_MASTER_WRITE, 1));
	i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, 1);
	ESP_ERROR_CHECK(i2c_master_stop(cmd));
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS1 << 1) | I2C_MASTER_WRITE, 1));
	i2c_master_write_byte(cmd, MPU6050_PWR_MGMT_1, 1);
	i2c_master_write_byte(cmd, 0, 1);
	ESP_ERROR_CHECK(i2c_master_stop(cmd));
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	// Configuración inicial del segundo MPU6050
	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS2 << 1) | I2C_MASTER_WRITE, 1));
	i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, 1);
	ESP_ERROR_CHECK(i2c_master_stop(cmd));
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	cmd = i2c_cmd_link_create();
	ESP_ERROR_CHECK(i2c_master_start(cmd));
	ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS2 << 1) | I2C_MASTER_WRITE, 1));
	i2c_master_write_byte(cmd, MPU6050_PWR_MGMT_1, 1);
	i2c_master_write_byte(cmd, 0, 1);
	ESP_ERROR_CHECK(i2c_master_stop(cmd));
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	while(1) {
	// Lectura de datos del primer MPU6050
		cmd = i2c_cmd_link_create();
		ESP_ERROR_CHECK(i2c_master_start(cmd));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS1 << 1) | I2C_MASTER_WRITE, 1));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, 1));
		ESP_ERROR_CHECK(i2c_master_stop(cmd));
		ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
		i2c_cmd_link_delete(cmd);

		cmd = i2c_cmd_link_create();
		ESP_ERROR_CHECK(i2c_master_start(cmd));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS1 << 1) | I2C_MASTER_READ, 1));

		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1,   0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+1, 0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+2, 0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+3, 0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+4, 0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+5, 0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+6, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+7, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+8, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+9, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+10, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+11, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+12, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data1+13, 1));

		ESP_ERROR_CHECK(i2c_master_stop(cmd));
		ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
		i2c_cmd_link_delete(cmd);

		// Procesar los datos del acelerómetro
        accel_x1 = (data1[0] << 8) | data1[1];
        accel_y1 = (data1[2] << 8) | data1[3];
        accel_z1 = (data1[4] << 8) | data1[5];

		//A partir de los valores del acelerometro, se calculan los angulos Y, X
   		//respectivamente, con la formula de la tangente.
   		ang_acc_y1 = atan(-1*(accel_x1/A_R)/sqrt(pow((accel_y1/A_R),2) +pow((accel_z1/A_R),2)))*RAD_A_DEG;
        // Leer los valores del giroscopio
        gyro_y1 = (data1[10] << 8) | data1[11];
		//Calculo del angulo del Giroscopio
  		ang_giro_y1 = gyro_y1/G_R;

		
		//Lectura de datos del segundo MPU6050
		cmd = i2c_cmd_link_create();
		ESP_ERROR_CHECK(i2c_master_start(cmd));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS2 << 1) | I2C_MASTER_WRITE, 1));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, MPU6050_ACCEL_XOUT_H, 1));
		ESP_ERROR_CHECK(i2c_master_stop(cmd));
		ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
		i2c_cmd_link_delete(cmd);

		cmd = i2c_cmd_link_create();
		ESP_ERROR_CHECK(i2c_master_start(cmd));
		ESP_ERROR_CHECK(i2c_master_write_byte(cmd, (I2C_ADDRESS2 << 1) | I2C_MASTER_READ, 1));

		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2,   0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+1, 0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+2, 0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+3, 0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+4, 0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+5, 0));
		ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+6, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+7, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+8, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+9, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+10, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+11, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+12, 0));
        ESP_ERROR_CHECK(i2c_master_read_byte(cmd, data2+13, 1));

		ESP_ERROR_CHECK(i2c_master_stop(cmd));
		ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS));
		i2c_cmd_link_delete(cmd);

		// Procesar los datos del acelerómetro
        accel_x2 = (data2[0] << 8) | data2[1];
        accel_y2 = (data2[2] << 8) | data2[3];
        accel_z2 = (data2[4] << 8) | data2[5];

		//A partir de los valores del acelerometro, se calculan los angulos Y, X
   		//respectivamente, con la formula de la tangente.
   		ang_acc_y2 = atan(-1*(accel_x2/A_R)/sqrt(pow((accel_y2/A_R),2) +pow((accel_z2/A_R),2)))*RAD_A_DEG;
        // Leer los valores del giroscopio
        gyro_y2 = (data2[10] << 8) | data2[11];

		//Calculo del angulo del Giroscopio
  		ang_giro_y2 = gyro_y2/G_R;

		int64_t tiempo_actual = esp_timer_get_time();
		dt = ((tiempo_actual - tiempo_prev) / 1000.0)-20;
  		tiempo_prev = tiempo_actual;

		//Aplicar el Filtro Complementario

		Angulo_y1 = 0.98 *((Angulo_y1+(ang_giro_y1*dt/1000)) + (0.02*ang_acc_y1));
		Angulo_y2 = 0.98 *((Angulo_y2+(ang_giro_y2*dt/1000)) + (0.02*ang_acc_y2));
		
		*global_Angulo_y1=Angulo_y1;
		*global_Angulo_y2=Angulo_y2;
		//ESP_LOGI(tag, "Angulo_y1: %f",Angulo_y1);
		//ESP_LOGI(tag, "Angulo_y2: %f",Angulo_y2);

	}

	vTaskDelete(NULL);
} // task_hmc5883l