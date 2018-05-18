#include <SoftwareSerial.h>
#include <ArduinoBlue.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Pines
const int BLUETOOTH_TX = 4;
const int BLUETOOTH_RX = 2;
const int pinBuzzer = 9;
const int pinPulsador = 7;
const int pinLed = 11; // PWM

// Constantes
const int freq = 466;
const int duracion = 500;

// Variables
int valorPulsador = 0;
int brillo = 128; 
int valorDesplazamiento, idBoton, idDesplazador;

// Inicializaciones
SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);
ArduinoBlue telefono(bluetooth);
LiquidCrystal_I2C lcd(0x3F,20,4);  //Crea el objeto lcd en la dirección 0x3F. 20 columnas x 4 filas

void setup() {
    // Monitor seria a 9600 bps.
    Serial.begin(9600);

    // Iniciamos bluetooth a 9600 bps.
    bluetooth.begin(9600);

    // Una pausa para dejar tiempo a la inicialización
    delay(100);    

    // Enviamos comando para renombrar el dispositivo
    bluetooth.write("AT+NAMEMiArduino");

    pinMode(pinPulsador, INPUT);
    analogWrite(pinLed, brillo);

    // Inicializamos el LCD
    lcd.init();
    
    // Encendemos la luz de fondo.
    lcd.backlight();
    
    // Escribimos en el LCD.
    lcd.print("[ Arduino & iPhone ]");
    lcd.setCursor(0,3);
    lcd.print(" ... Esperando ...");
    Serial.println("... Esperando ...");
}

void loop() {
    // Si se ha pulsado algún botón en el móvil, obtenemos su id
    idBoton = telefono.getButton();
    if (idBoton != -1) {
        Serial.print("Botón: ");
        Serial.println(idBoton);
        // según el botón pulsado hacemos una cosa u otra
        if(idBoton == 1){
          lcd.setCursor(0,2);
          lcd.print("Reproduciendo tono");
          tone(pinBuzzer, freq, duracion);
          delay(500);
          lcd.setCursor(0,2);
          lcd.print("                  ");
        }
        if(idBoton == 3){
          lcd.setCursor(0,2);
          lcd.print("Apagando pantalla");
          lcd.noBacklight();
          delay(1000);
          lcd.setCursor(0,2);
          lcd.print("                 ");
        }
        if(idBoton == 4){
          lcd.setCursor(0,2);
          lcd.print("Encendiendo pantalla");
          lcd.backlight();
          delay(1000);
          lcd.setCursor(0,2);
          lcd.print("                    ");
        }
    }

    // Si se ha cambiado el valor de algún desplazador en el móvil, obtenemos su id
    idDesplazador = telefono.getSliderId();

    // Y su valor (va de 0 a 200).
    valorDesplazamiento = telefono.getSliderVal();
    if (idDesplazador != -1) {
        Serial.print("Id. Desplazador: ");
        Serial.print(idDesplazador);
        Serial.print("\tValor: ");
        Serial.println(valorDesplazamiento);
        if(idDesplazador == 2){
          // para la intensidad del led necesitamos un valor entre 0 y 255
          int valLed = map(valorDesplazamiento, 0, 200, 0, 255);
          // vamos a mostrar un porcentaje --> un valor entre 0 y 100
          int valMostrar = map(valorDesplazamiento, 0, 200, 0, 100);
          lcd.setCursor(0,2);
          lcd.print("Brillo led: " + String(valMostrar)  + "%   ");
          analogWrite(pinLed, valLed);
          delay(100);
          lcd.setCursor(0,2);
          lcd.print("                    ");
        }
    }

    // Si se ha enviado un texto desde el móvil, lo obtenemos
    String texto = telefono.getText();
    if (texto != "") {
        Serial.println(texto);
        lcd.setCursor(0,1);
        lcd.print(texto);
    }

    // Si se ha pulsado, mandamos un mensaje al teléfono
    valorPulsador = digitalRead(pinPulsador);  //lectura digital de pin
    if (valorPulsador == HIGH) {
        Serial.println("Se ha pulsado");
        telefono.sendMessage("Se ha pulsado"); // se envía al teléfono el mensaje
        delay(1000);
    }
}
