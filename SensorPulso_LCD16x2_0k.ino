// Medidor de pulso cardiaco 
// Auth: ISC. Omar Emmanuel Lara Juárez
// Date: 26/11/2019
// Conexion Sensor Pulsos -> Arduino UNO:   S -> A0 , Vcc -> 5V , (-) -> GND
// Conexion i2C -> Arduino UNO:  SCL-> A5   SDA->A4  VCC->5V  GND->GND

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);   // inicializa la interfaz I2C del LCD 16x2
int estatus = 0;
int pulsePin = 0;                   // Sensor de Pulso conectado al puerto A0
int pulseLed = 13;               sdf   // PIN para el pulso cardiaco 
int ledR = 2;                       // Led Verde
int ledA = 3;                       // Led Amarillo
int ledV = 4;                       // Led Rojo
int button = 5;                     // Button Arranque
int sensorTemp = 1;                 // Sensor Temp
int buttonState = 0;                // Estado de el boton 
// Estas variables son volatiles porque son usadas durante la rutina de interrupcion en la segunda Pestaña
volatile int BPM;                   // Pulsaciones por minuto
volatile int Signal;                // Entrada de datos del sensor de pulsos
volatile int IBI = 600;             // tiempo entre pulsaciones
volatile boolean Pulse = false;     // Verdadero cuando la onda de pulsos es alta, falso cuando es Baja
volatile boolean QS = false;        // Verdadero cuando el Arduino Busca un pulso del Corazon

void setup(){ 
 
 pinMode(ledR, OUTPUT); 
 pinMode(ledA, OUTPUT); 
 pinMode(ledV, OUTPUT); 
 pinMode(pulseLed, OUTPUT); 
 pinMode(button, INPUT); 
 lcd.init();                         // Se inicializa el LCD 16x2
 lcd.backlight();                           
 lcd.clear(); 
 lcd.setCursor(0,0);                   //Muestra en el LCD el valor de BPM 
 lcd.print(">>> VITAL TEC <<<");
 lcd.setCursor(0,1);                   //Muestra en el LCD el valor de BPM 
 lcd.print("  ESPERANDO...");
 Serial.begin(9600);                // Puerto serial configurado a 9600 Baudios
 interruptSetup();                  // Configura la interrucion para leer el sensor de pulsos cada 2mS  
}

void loop(){
 buttonState = digitalRead(button);    //Se revisa si el botton esta presionado.
 if(buttonState == HIGH && estatus == 0){
      estatus = 1;
      buttonState = LOW;    //Se revisa si el botton esta presionado.
      int sumTemp=0;
      int sumPulsos=0;
      int TempFinal =0;
      int BPMFinal =0;
       for(int i=30;i>0;i--){
            lcd.clear(); 
            lcd.setCursor(0,0);                   //Muestra en el LCD el valor de BPM 
            lcd.print("TOMANDO SIGNOS..");
            lcd.setCursor(0,1);                   //Muestra en el LCD el valor de BPM 
            lcd.print(i);
            lcd.print(" SEG.");
            int pulso = analogRead(A0);           //Lee el valor del pulsometro conectado al puerto Analogo A0
            int val = analogRead(A1);
            float mv = (val/1024.0)*5000;
            float cel = mv/10;                   // Calulculada Tem en °C
            float bpm_Temp = BPM;   
            Serial.print("Temp: ");
            Serial.print(cel);
            Serial.println(" °C");
            Serial.print("BPM: ");
            Serial.println(bpm_Temp);
            if (QS == true){                       // Bandera del Quantified Self es verdadera cuando el Arduino busca un pulso del corazon
                   QS = false;                          // Reset a la bandera del Quantified Self 
            }
            sumTemp += cel;
            sumPulsos += bpm_Temp;
            if (pulso >= 530) {                   // Enciende led 13 cuando el pulso pasa de un valor (debe ajustarse)
               digitalWrite(pulseLed, HIGH);
            }else{
                digitalWrite(pulseLed, LOW);
            }
            delay(1000);
       }
       TempFinal = sumTemp/30;
       BPMFinal = sumPulsos/30;
       //Aqui validaciones y semi diagnostio en LEDS
       lcd.clear(); 
       lcd.setCursor(0,0);                   //Muestra en el LCD el valor de Temperatura corporal
       lcd.print("TEMP : ");
       lcd.print(TempFinal);
       lcd.print("°C");           
       lcd.setCursor(0,1);                   //Muestra en el LCD el valor de BPM 
       lcd.print("BPM : ");
       lcd.print(BPMFinal);
       while(buttonState == LOW){
         buttonState = digitalRead(button);    //Se revisa si el botton esta presionado.       
         estatus = 0;
       }
       digitalWrite(ledR,LOW);
       digitalWrite(ledA,LOW);
       digitalWrite(ledV,LOW); 
       lcd.clear(); 
       lcd.setCursor(0,0);                   //Muestra en el LCD el valor de BPM 
       lcd.print(">>> VITAL TEC <<<");
       lcd.setCursor(0,1);                   //Muestra en el LCD el valor de BPM 
       lcd.print("  ESPERANDO...");
   }
   delay(50);
}
