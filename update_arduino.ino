#include <Wire.h>
#include <SoftwareSerial.h>
#include "MAX30100_PulseOximeter.h"
#include <ADXL345.h>

SoftwareSerial serial_esp(2, 3);
uint32_t t = 0;
int r = 1000;
 
//GY-MAX30100
PulseOximeter sensor_oximetro;
int freq;
int freq_limiarinf = 45;
int freq_limiarsup = 90;
int alt_freq;
float SpO2;
float SpO2_limiarinf = 92;
float alt_SpO2;

//LM35
float Temperatura;
float temp_limiarinf = 35;
float temp_limiarsup = 37.5;
float alt_temperatura;
float Ler_conversor;
float sensor_temperatura;
float ADC_bit;

//ADXL345
ADXL345 accelerometer;

void setup() {
  
  //Inicializa a comunicação serial
  Serial.begin(115200);
  
  //Inicializa a comunicação serial com o ESP-01
  serial_esp.begin(115200);

  //Inicializa o GY-MAX30100 e a corrente do LED
  sensor_oximetro.begin();
  sensor_oximetro.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  //Inicializa o ADXL345
  accelerometer.begin();

  //Definição dos limiares de queda
  accelerometer.setFreeFallDuration(0.1);   
  accelerometer.setFreeFallThreshold(0.35);

  // Configura o pino de interrupção utilizado
  accelerometer.useInterrupt(ADXL345_INT1);
}
void loop() {
//Faz a leitura da SpO2 e frequência cardíaca
        sensor_oximetro.update();
  
  if (millis() - t > r) {

        
        //Leitura da temperatura, conversão e retorno do valor de temperatura
        sensor_temperatura=analogRead(A0);
        ADC_bit = (1 / 1024.0) * 5000;
        Ler_conversor = sensor_temperatura * ADC_bit;
        Temperatura = Ler_conversor / 10;
        freq = sensor_oximetro.getHeartRate();
        SpO2 = sensor_oximetro.getSpO2();
        
        //Descomentar para imprimir os valores para verificação no monitor serial
          Serial.print("{\"Temp\":");
          Serial.print(Temperatura);
          Serial.print(",");
          Serial.print("\"H_rate\":");
          Serial.print(freq);
          Serial.print(",");
          Serial.print("\"SpO2\":");
          Serial.print(SpO2); 
          
        if(Temperatura<=temp_limiarinf || Temperatura>=temp_limiarsup){
            alt_temperatura = Temperatura;
            Serial.print(",");
            Serial.print("\"AlteracaoTemp\":");
            Serial.print(alt_temperatura);
        }
        
        if(freq<freq_limiarinf || freq>freq_limiarsup){
            alt_freq = freq;
            Serial.print(",");
            Serial.print("\"AlteracaoBpm\":");
            Serial.print(alt_freq);
        }
        
        if(SpO2<=SpO2_limiarinf){
            alt_SpO2 = SpO2;
           Serial.print(",");
           Serial.print("\"AlteracaoSpO2\":");
           Serial.print(alt_SpO2);
        }

     //Leitura da aceleração e detecção de queda
        Vector nor = accelerometer.readNormalize();
        Activites activ = accelerometer.readActivites();
        
        if(activ.isFreeFall){
            Serial.print(",");
            Serial.print("\"Queda\":");
            Serial.print("tem queda");
       } 
        Serial.print("}");
        Serial.println();
        
        
        //Imprimindo via porta serial para o ESP-01  
        serial_esp.print("{\"Temp\":");
        serial_esp.print(Temperatura);
        serial_esp.print(",");
        serial_esp.print("\"H_rate\":");
        serial_esp.print(sensor_oximetro.getHeartRate());
        serial_esp.print(",");
        serial_esp.print("\"SpO2\":");
        serial_esp.print(sensor_oximetro.getSpO2());

        //Verificar evento crítico na temperatura
        if(Temperatura<=temp_limiarinf || Temperatura>=temp_limiarsup){
            alt_temperatura = Temperatura;
            serial_esp.print(",");
            serial_esp.print("\"Al_Temp\":");
            serial_esp.print(alt_temperatura);
            
        }

        //Verificar evento crítico na frequência cardíaca
        if(freq<freq_limiarinf || freq>freq_limiarsup){
            alt_freq = freq;
            serial_esp.print(",");
            serial_esp.print("\"Al_Bpm\":");
            serial_esp.print(alt_freq);
        }
        
        //Verificar evento crítico na saturação de oxigênio
        if(SpO2<=SpO2_limiarinf){
            alt_SpO2 = SpO2;
            serial_esp.print(",");
            serial_esp.print("\"Al_SpO2\":");
            serial_esp.print(alt_SpO2);
        }

        //Verificar evento crítico de queda 
        if(activ.isFreeFall){
            serial_esp.print(",");
            serial_esp.print("\"Queda\":");
            serial_esp.print(true);
        }
        
        serial_esp.print("}");
        serial_esp.println();
        t = millis();
    }
  delay(50);
}
