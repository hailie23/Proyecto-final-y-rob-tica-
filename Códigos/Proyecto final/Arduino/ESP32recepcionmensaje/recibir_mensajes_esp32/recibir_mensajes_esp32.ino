#include <WiFi.h>
#include <PubSubClient.h>

//configuracion wifi

const char* ssid = "banana street";
const char* password = "S3Nb0nzakura!";

const char* mqttserver = "broker.hivemq.com";
const int mqttport = 1883;


//client id

const char* clientid = "esp_client"; 

//topic para recibir mensajes 

const char* topicRecibir1 = "ESPlimiterodillamin";
const char* topicRecibir2 = "ESPlimiterodillamax";
const char* topicRecibir3 = "ESPempezarterapia";
const char* topicRecibir4 = "ESPrepeticiones";
const char* topicvariable ="";

//valores que salen de los topicos del mqtt
float valormqtt;

float Valormin; //Angulo minimo de flexion.
float Valormax; //Angulo maximo de flexion.
int Valorestado;//operacion de movimiento 1 - "si", 2-"no";
int Repeticiones; //es el numero de repeticiones que hará el movimiento.

WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
 Serial.begin(115200);

  //conectarme al wifi

  WiFi.begin(ssid,password);
    while(WiFi.status()!=WL_CONNECTED){
    delay(1000);
    Serial.println("conectando al wifi");  
   } 

    Serial.println("conectado a la red");


   //config server

 
 client.setServer(mqttserver ,mqttport);
 client.setCallback(callback);

}

void loop() {
 if(!client.connected()){
  reconnect();
 }

 client.loop();
 recibirvalores();
}

void reconnect(){
   while(!client.connected()){
     Serial.println("intentando conexion MQTT");  


     if(client.connect(clientid)){
         Serial.println("estamos conectados a mqtt");  
         client.subscribe(topicRecibir1);
         client.subscribe(topicRecibir2);
         client.subscribe(topicRecibir3);
         client.subscribe(topicRecibir4);  
     }
     else{
      Serial.println("fallo la conexion"); 
      Serial.println(client.state());
      delay(1000); 
     }
    
   }

}

void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("mensaje recibido: ");
topicvariable=topic;
Serial.println(topicvariable);  // Imprime el tema (topicvariable)

   // Construir la cadena a partir del payload
   String mensaje = "";
   
   for (int i = 0; i < length; i++) {
      mensaje += (char)payload[i];
   }
   //mensaje debe convertirse ne un float
   float fmensaje = mensaje.toFloat();
   valormqtt=fmensaje;
   // Imprimir la cadena completa
   Serial.print("mensaje del payload: ");
   Serial.println(mensaje);
   Serial.print("mensaje en float: "); 
   Serial.println(fmensaje);
   Serial.println();
   }


   void recibirvalores(){

    if((strcmp(topicvariable, topicRecibir1) == 0)){
      //Serial.println("hola1");
      Valormin=valormqtt;
    }
    if((strcmp(topicvariable, topicRecibir2) == 0)){
      //Serial.println("hola2");
      Valormax=valormqtt;
    }
    
    if((strcmp(topicvariable, topicRecibir3) == 0)){
      //Serial.println("hola3");
      Valorestado=valormqtt;
    }
    if((strcmp(topicvariable, topicRecibir4) == 0)){
      //Serial.println("hola4");
      Repeticiones=valormqtt;
    }
    Serial.print("valormin: "+ String(Valormin)+"   ");
    Serial.print("valormax: "+ String(Valormax)+"   ");
    Serial.print("valorestado: "+ String(Valorestado)+"   ");
    Serial.println("repeticiones: "+ String(Repeticiones));
   }

/*
falta que los ultimos 3 valores sean los parametros para que funcione la maquina de terapia, a esta maquina habra que agregarle los numeros de repeticiones.

Valormin se refiere al numero de minimo grados de la extencion.
Valormax se refiere al numero maximo de grados de la extencion.
Valorestado se refiere a si esta funcionando la operacion de movimiento 1 - "si", 2-"no";
Repeticiones es el numero de repeticiones que hará el movimiento.
*/


