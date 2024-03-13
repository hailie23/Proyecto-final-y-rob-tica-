#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>

//configuracion wifi
const char* ssid = "Familia_Cohn";
const char* password = "809.560.1581.caFE";

//DIRECCION SERVIDOR
const char* mqttserver = "broker.hivemq.com";
const int mqttport = 1883;

//client id
const char* clientid = "esp_client";

//config topic
const char* topic1 = "EnviarMensajerodilla";
const char* topic2 = "EnviarMensajetobillo";
const char* topicRecibir1 = "ESPlimiterodillamax";
const char* topicRecibir2 = "ESPlimiterodillamin";
//int variableAumentar = 0;

WiFiClient espClient;
PubSubClient client(espClient);



//Direccion I2C de la IMU
const int MPU1=0x68;
const int MPU2=0x69;
//Ratios de conversion
#define A_R 16384.0 // 32768/2
#define G_R 131.0 // 32768/250
 
//Conversion de radianes a grados 180/PI
#define RAD_A_DEG = 57.295779
 
//MPU-6050 da los valores en enteros de 16 bits
//Valores RAW
int16_t AcX1, AcY1, AcZ1, GyX1, GyY1, GyZ1;
int16_t AcX2, AcY2, AcZ2, GyX2, GyY2, GyZ2;
//Angulos
float Acc1[2],Gy1[3],Angle1[3];
float Acc2[2],Gy2[3],Angle2[3];

 
String valores1;
String valores2;

long tiempo_prev1;
float dt1;
long tiempo_prev2;
float dt2;

float giro1;
float giro2;

void setup()
{
  Wire.begin(21,22); // D21(GPI21)=SDA / D22(GPIO22)=SCL
  Wire.beginTransmission(MPU1);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  Wire.begin(21,22);// D21(GPI21)=SDA / D22(GPIO22)=SCL
  Wire.beginTransmission(MPU2);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(115200);

  WiFi.begin(ssid,password);
    
    while(WiFi.status()!=WL_CONNECTED){
      delay(1000);
      Serial.println("conectando al wifi");  
    }

    Serial.println("conectado a la red");  

    //config server
  client.setServer(mqttserver ,mqttport);
}

void giroscopio(){
  
   //Leer los valores del Acelerometro de la IMU
   Wire.beginTransmission(MPU1);
   Wire.write(0x3B); //Pedir el registro 0x3B - corresponde al AcX1
   Wire.endTransmission(false);
   Wire.requestFrom(MPU1,6);   //A partir del 0x3B, se piden 6 registros
   AcX1=Wire.read()<<8|Wire.read(); //Cada valor ocupa 2 registros
   AcY1=Wire.read()<<8|Wire.read();
   AcZ1=Wire.read()<<8|Wire.read();

   //A partir de los valores del acelerometro, se calculan los angulos Y, X
   //respectivamente, con la formula de la tangente.
   Acc1[1] = atan(-1*(AcX1/A_R)/sqrt(pow((AcY1/A_R),2) + pow((AcZ1/A_R),2)))*RAD_TO_DEG;
  
   //Leer los valores del Giroscopio
   Wire.beginTransmission(MPU1);
   Wire.write(0x43);
   Wire.endTransmission(false);
   Wire.requestFrom(MPU1,6);   //A partir del 0x43, se piden 6 registros
   GyX1=Wire.read()<<8|Wire.read(); //Cada valor ocupa 2 registros
   GyY1=Wire.read()<<8|Wire.read();
   GyZ1=Wire.read()<<8|Wire.read();
 
   //Calculo del angulo del Giroscopio
   Gy1[1] = GyY1/G_R;

   dt1 = (millis() - tiempo_prev1) / 1000.0;
   tiempo_prev1 = millis();
 
   //Aplicar el Filtro Complementario
   Angle1[1] = 0.98 *(Angle1[1]+Gy1[1]*dt1) + 0.02*Acc1[1];
   
   float angulo_giro1 = Angle1[1]-1;
   giro1=angulo_giro1;

   //Mostrar los valores por consola
   Serial.print("Encoder Rodilla: ");
   Serial.print(String(angulo_giro1)+ " grados   ");

}

void giroscopio2(){
   //Leer los valores del Acelerometro de la IMU
   Wire.beginTransmission(MPU2);
   Wire.write(0x3B); //Pedir el registro 0x3B - corresponde al AcX1
   Wire.endTransmission(false);
   Wire.requestFrom(MPU2,6);   //A partir del 0x3B, se piden 6 registros
   AcX2=Wire.read()<<8|Wire.read(); //Cada valor ocupa 2 registros
   AcY2=Wire.read()<<8|Wire.read();
   AcZ2=Wire.read()<<8|Wire.read();

   //A partir de los valores del acelerometro, se calculan los angulos Y, X
   //respectivamente, con la formula de la tangente.
   Acc2[1] = atan(-1*(AcX2/A_R)/sqrt(pow((AcY2/A_R),2) + pow((AcZ2/A_R),2)))*RAD_TO_DEG;
   
   //Leer los valores del Giroscopio
   Wire.beginTransmission(MPU2);
   Wire.write(0x43);
   Wire.endTransmission(false);
   Wire.requestFrom(MPU2,6);   //A partir del 0x43, se piden 6 registros
   GyX2=Wire.read()<<8|Wire.read(); //Cada valor ocupa 2 registros
   GyY2=Wire.read()<<8|Wire.read();
   GyZ2=Wire.read()<<8|Wire.read();
 
   //Calculo del angulo del Giroscopio
   Gy2[1] = GyY2/G_R;

   dt2 = (millis() - tiempo_prev2) / 1000.0;
   tiempo_prev2 = millis();
 
   //Aplicar el Filtro Complementario
   Angle2[1] = 0.98 *(Angle2[1]+Gy2[1]*dt2) + 0.02*Acc2[1];

  float angulo_giro2 = Angle2[1]-1;    
  giro2=angulo_giro2;
   //Mostrar los valores por consola
  Serial.print("Encoder Tobillo: ");
  Serial.print(String(angulo_giro2 )+ " grados   ");
  
}
//reconectar wifi
void reconnect(){
   while(!client.connected()){
     Serial.println("intentando conexion MQTT");  


     if(client.connect(clientid)){
         Serial.println("estamos conectados a mqtt");  
         client.subscribe(topicRecibir1);
         client.subscribe(topicRecibir2);
     }
     else{
      Serial.println("fallo la conexion"); 
      Serial.println(client.state());
      delay(1000); 
     }
    
   }
}

void publicarmqtt(float angulo_giro1,float angulo_giro2){
  char mensaje1[10];
  char mensaje2[10];

 snprintf(mensaje1,sizeof(mensaje1),"%f", angulo_giro1);
 client.publish(topic1,mensaje1);
 Serial.println("dato enviado: 'rodilla':"+String( angulo_giro1));
 
 snprintf(mensaje2,sizeof(mensaje1),"%f",angulo_giro2);
 client.publish(topic2,mensaje2);
 Serial.println("dato enviado: 'tobillo':"+String(angulo_giro2));
}

void callback(char*topic, byte* payload, unsigned int length) {
   Serial.println("mensaje recibido:"); 
   Serial.println(topic);  // Imprime el tema (topic)

   Serial.println("mensaje del payload:");

   // Construir la cadena a partir del payload
   String mensaje = "";
   for (int i = 0; i < length; i++) {
      mensaje += (char)payload[i];
   }

   // Imprimir la cadena completa
   Serial.println(mensaje);
   Serial.println();
}

void loop(){

  if(!client.connected()){
  reconnect();
 }

giroscopio();
giroscopio2();
publicarmqtt(giro1, giro2);
delay(10);
//debo buscar la forma de que el string en recibir mqtt se guarde como float.
}