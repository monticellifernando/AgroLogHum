/*
 * =====================================================================================
 *
 *       Filename:  AgroLogHum.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/03/22 09:04:33
 *       Revision:  none
 *       Compiler:  gcc
 *          Board:  Arduino nano
 *
 *         Author:  Fernando Monticelli (), Fernando.Monticelli@cern.ch
 *   Organization:  UNLP / UBA
 *
 * =====================================================================================


 Data logger de temperatura y humedad

 Conecciones

 Sensor, humedad y temperatura: 
AHT10: 


Display:
rs = D3
en = D2
d4 = A3
d5 = A2
d6 = A1
d7 = A0; 


RTC / AHT10
SCL = A5
SDA = A4


| SD Card | Arduino |
|  ----   | ---- |
|   cs    |  10  |
|  MOSI   |  11  |
|  SCK    |  13  |
|  MISO   |  12  |


 */

#define _ID_ "AgroLogHumSD"

// Para grabar en EEPROM
#include <EEPROM.h>

//Sensor de humedad
// Codigo https://learn.adafruit.com/adafruit-aht20/arduino
#include <Adafruit_AHTX0.h>

// Para el rtc
//DS 1307
#include <Wire.h>
#include "RTClib.h"

// LCD stuff
#include<LiquidCrystal.h>


// SD card
#include <Fat16.h>
SdCard SD;
Fat16 file;
int SD_ChipSelect = 10;

// ============ Periféricos =============

// Ahora el RTC
RTC_DS1307 rtc;

// Sensor de humedad y temperatura
Adafruit_AHTX0 aht;

// Pines del LCD
//const int rs = 2;
//const int en = 3;
//const int d4 = 4;
//const int d5 = 5;
//const int d6 = 6;
//const int d7 = 7; 

// LCD
LiquidCrystal lcd(2,3,4,5,6,7);

// ========= Variables y parámetros ============

int MemoriaTotal = EEPROM.length();

int TamanoDato = 50;
// Mensajes de depurado
#define DEBUG false

// Si recién me desperté
//bool JustWokeUp = true;

// Tiempos
int Evento=0;

//// Espera entre eventos de lectura. Esto es cuanto espera para leer todo y mostrarlo en la pantalla LCD
//int DeltaT = 1000; // 1s
// 
int Posicion=0;
int MostrarCada = 60; // Esto es en minutos
int UnaMedidaCada = 4; // Esto es en Segundos, Minutos u Horas, dependiendo de las unidades de tiempo de epera de abajo
//#define Horas 3600;
//#define Minutos 60;
//#define Segundos 1;
int UnidadTiempoDeEspera = 3600;

// Logic to do the average of temperature of last 5measurements



int segundo;
struct Dato {
    int minuto;
    int hora;
    int dia;
    int mes;
    int anio; 
    float Humedad;
    float Temperatura;
} ;

DateTime HoraFecha;


Dato Lectura; // Aca voy a guardar los datos de fecha y huimedad y temperatura

// Para leer comandos del terminal serie
String InputString = "";         // a String to hold incoming data
String Command, Valor; // Donde vamos a guardar la interpretacion del comando
                                
bool stringComplete ;  // whether the string is complete

bool ErrorFecha = true; // Si es cierto. Hay que poner la fecha!
int FechaIdx = 0; // Este se usa para ParseFecha. Es la posición del entero que está guardando
                   
                  
// Funciones
//
void DumpEEPROM();
bool EstaVacia(Dato Lectura); 

void ReiniciarEEPROM(); // Borra el contenido de la EEPROM

void MostrarDato(Dato Lectura); // Muestra el contenido de Lectura en el puerto serie
                                

void GrabarDatoEnEEPROM(Dato Lectura); // Graba el contenido de Lectura en la próxima posición de memoria de la EEPROM


void ParseFecha(); // Interpreta la Fecha en formato Anio,Mes,dia,Hora,minuto para meter cada uno de esos campos dentro de NuevaFecha

int ValorHastaComa(); // Devuelve el entero que está hasta la siguiente coma

bool RtcPowerLost() ; // HAck para checkear si el reloj está seteado o no

void PrintHelp(); // Muestra mensaje de ayuda en el terminal serie
                  


void ReadInput(); // Lee la entrada de la terminal serie. Y la prepara para interpretarla como un Comnando y un Valor

void serialEvent(); // Función de evento serie para interpretar la entrada del puerto serie

int Interpret(); // Interpreta Commando Valor y hace algo al respecto, dependiendo del comando
