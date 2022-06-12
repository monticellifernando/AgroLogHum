# AgroLegHumX v1.0

Cliente de WIFI con varios sensores de humedad

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


| RTC |  Arduino |
| --- |   --- | 
| SCL |  A5 |
| SDA |  A4 |



| Sensor de humedad  | Arduino |
| ----      | ----  
| A0 de sensor 0 | A0 |
| A0 de sensor 1 | A1 |


![Arduino pinout](arduino-nano-pins.png "Pinout de arduino")





