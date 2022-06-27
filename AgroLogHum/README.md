# AgroLegHum v2.0

## Hardware:

| Hw                                                                                            | Tipo           |
| ----                                                                                          | -----          |
| Arduino nano                                                                                  | Motherboard    |
| [RTC DS1307](https://naylampmechatronics.com/blog/52_tutorial-rtc-ds1307-y-eeprom-at24c.html) | RTC            |
| LCD 1602                                                                                      | Pantalla       |
| [AHT10](https://learn.adafruit.com/adafruit-aht20/arduino)                                    | Sensor Humedad |


## Conecciones

| Display | Arduino |
| --- | --- |
| rs  | D3  |
| en  | D2  |
| d4  | D4  |
| d5  | D5  |
| d6  | D6  |
| d7  | D7  |


| RTC | AHT10 | Arduino |
| --- | --- |  --- | 
| SCL | SCL | A5 |
| SDA | SDA | A4 |

| SDCard | Arduino |
| ---    | ---     |
| CS     | D10     |
| MOSI   | D11     |
| MISO   | D12     |
| SCK    | D13     |


![Arduino pinout](arduino-nano-pins.png "Pinout de arduino")


## Comandos

El dispositivo se comanda desde el puerto serie, via usb a 115200 baud. Los comandos permiten configurar el dispositivo y operarlo. 
| Instrucción | Argumento                       | Detalle                                                                             |
| ------      | ------                          | -----                                                                               |
| B           |                                 | Borra el archivo con todos los registros.                                           |
| CH          | Horas                           | Graba una medición cada <Horas> horas en el archivo de registro                     |
| CM          | Minutos                         | Graba una medición cada <Minutos> minutos en el archivo de registro                 |
| F           | Año,Mes,Día,Hora,Minuto,Segundo | Pone fecha en el dispositivo                                                        |
| G           |                                 | Hace una enrada en el archivo de registro con la fecha y las mediciones del momento |
| L           |                                 | Muestra los datos registrados en el archivo de registro                             |

Si el comando ingresado es incorrecto, el programa va a mosrar un mensaje de ayuda comentando cuales son los comandos posibles.

```
No entendi eso :-P
Comandos posibles:
B -> Borra la memoria
CH <Horas> -> Graba cada <Horas> horas
F <Año>,<Mes>,<Día>,<Hora>,<Minuto>,<Segundo> -> Pone fecha
G -> Graba datos
L -> muestra datos
```

Los comandos se envían por el puerto serie terminados por un fin de linea (o `[ENTER]`). Hay dos tipos de comandos.
* Comandos que consisten en una instrucción
* Comandos que están compuestos por una instrucción y un argumento

Los argumentos se envían separados por un espacio de las Instrucciones. Por ejemplo, para poner la fecha:

    F 2022,6,24,18,3,10
    
Pone la fecha en 24 de junio de 2022 a las 18:03:10. Si el comando fecha `F` es enviado sin argumentos, el dispositivo muestra la fecha actual con el estado de la medición de Humedad y Temperatura



