/*
 * =====================================================================================
 *
 *       Filename:  ROM.ino (AgroLogHum.cxx)
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
 */

#include "AgroLogHumX.h"
// ============= Includes ===========

void setup()                    // run once, when the sketch starts
{

    Serial.begin(115200);           // set up Serial library at 115200 bps
    Serial.println("Setting up...");
    //ReiniciarEEPROM(); //<--- Hack para reiniciar la EEPROM

    //if (Serial) Serial.println("LCD...");
    //if (Serial) Serial.println("LCD clear...");
    lcd.clear();
    //if (Serial) Serial.println("LCD begin...");
    lcd.begin(16, 2);
    //if (Serial) Serial.println("LCD print...");
    lcd.print("Setup...");
    lcd.setCursor(0, 1);
    lcd.print(MemoriaTotal);
    lcd.print("B free");

    delay(1000);
    lcd.clear();
    lcd.print("Iniciando reloj");

    Serial.println("Inicializando Reloj...");
    //Inicializamos el RTC
    if (!rtc.begin()){
        lcd.clear();
        lcd.print("Error de reloj");
        Serial.println("ERROR:: Reloj no conectado! O error leyendo el reloj...");
        while(1){
            delay(1000);
        }
    }
    lcd.clear();
    lcd.print("Reloj oK!");
    delay(100);

    // Inicializar el sensor de temperatura
    Serial.println("Inicializando sensor...");
    lcd.print("Iniciando sensor");

    // if (!aht.begin()){
    //     lcd.clear();
    //     lcd.print("Error de sensor");
    //     if (Serial) Serial.println("ERROR:: sensor no conectado! O error leyendo el sensor...");
    //     while(1){
    //       delay(1000);
    //     }
    // }
    // lcd.clear();
    // lcd.print("Sensor OK");
    // delay(100);



    DumpEEPROM();



    lcd.clear();
    lcd.print("hora?");
    Serial.println("Revisando reloj:");
    if (RtcPowerLost()) {
        lcd.setCursor(0, 1);
        lcd.print("ERROR FECHA");
        Serial.println("Pone la fecha:");
        //delay(500);

    }

    //Serial.println("#t,Temp,Hum");  // prints hello with ending line break 

}

void loop() {

    //if(m_debug){
    //    Serial.println("Loop()");
    //}

    // Puerto Serie
    if (stringComplete){
        stringComplete = false;
        String Command="";
        String Value = "";
        ReadInput();

        // if (m_debug){
        //     Serial.println("Comando "+Command);
        //     Serial.println("Valor "+String(Value));
        // }
        int Result = Interpret();
        if (Result>0){
            Serial.println("No entendi eso :-P ");
            // Serial.println("Interpret devuelve " + String(Result));
            // Serial.println("ERROR:"+String(Result));
            PrintHelp();
        }
        InputString = "";
    }
    serialEvent();

    // lectura de AHT10 
    //sensors_event_t hum, temp;
    // Decirle al sensor que tome los datos
    //aht.getEvent(&hum, &temp);

    int Humedad0 = analogRead(A0);
    int Humedad1 = analogRead(A1);




    // Chequeamos la hora
    // if (m_debug){
    //     Serial.println("Levantando la fecha");
    // }
    HoraFecha = rtc.now(); //obtenemos la hora y fecha actual


    segundo=HoraFecha.second(); // No lo pongo en dato para que no me llene la EEPROM
    Lectura.Humedad0 = Humedad0;
    Lectura.Humedad1 = Humedad1;
    Lectura.minuto= HoraFecha.minute();
    Lectura.hora= HoraFecha.hour();
    Lectura.dia= HoraFecha.day();
    Lectura.mes= HoraFecha.month();
    Lectura.anio= HoraFecha.year()-2000;
    //diaDeLaSemana=HoraFecha.dayOfTheWeek();







    //if (Evento%(MostrarCada/10) == 0) { 
        // if (m_debug){
        //     Serial.println("Actualizando lcd:");
        // }

        lcd.clear();
        lcd.begin(16, 2);
        lcd.setCursor(0, 0);

        if(ErrorFecha){
            lcd.print("Error Fecha");
        } else {

            lcd.print(String(Lectura.dia));
            lcd.print("/");
            lcd.print(String(Lectura.mes));
            lcd.print(" ");
            if (Lectura.hora<10) lcd.print("0");
            lcd.print(String(Lectura.hora));
            lcd.print(":");
            if (Lectura.minuto<10) lcd.print("0");
            lcd.print(String(Lectura.minuto));
            lcd.print(":");
            if (segundo<10) lcd.print("0");
            lcd.print(String(segundo));
        }

        lcd.setCursor(0, 1);
        lcd.print(Lectura.Humedad0);
        lcd.print(" ");
        lcd.print(Lectura.Humedad1);
        lcd.print(" ");
    //}

    // Primero chequeamos que no tengamos una medición hecha en el futuro:
    Dato LecturaAnterior;
    EEPROM.get(2+Posicion*sizeof(Dato), LecturaAnterior);
    DateTime EstaMedida(Lectura.anio, Lectura.mes, Lectura.dia, Lectura.hora, Lectura.minuto, segundo);
    //DateTime UltimaMedidaGuardada(EEPROM[2+Posicion*sizeof(Dato)], EEPROM[2+Posicion*sizeof(Dato)+1], EEPROM[2+Posicion*sizeof(Dato)+2], EEPROM[2+Posicion*sizeof(Dato)+3], EEPROM[2+Posicion*sizeof(Dato)+4], 0); 
    DateTime MedidaAnterior(LecturaAnterior.anio, LecturaAnterior.mes, LecturaAnterior.dia, LecturaAnterior.hora, LecturaAnterior.minuto, segundo);
    // if (m_debug){
    //     Serial.println("Esta Medida:");
    //     MostrarDato(Lectura);
    //     Serial.print("Ultima Medida Guardada, Posicion ");

    //     Serial.println(Posicion);
    //     MostrarDato(LecturaAnterior);
    // }

    TimeSpan Diff = EstaMedida - MedidaAnterior  ;

    // if (m_debug){
    //     Serial.print("Sec Diff= ");
    //     Serial.println(Diff.totalseconds());
    // }

    //ErrorFecha = false;

    //if (Diff.totalseconds() <= 0) {
    //    ErrorFecha = true;
    //}

    // Ahora revisar si hay que guardar en la EEPROM
    if (!ErrorFecha && (Diff.totalseconds() > UnaMedidaCada*UnidadTiempoDeEspera || EstaVacia(LecturaAnterior))){
        //if (JustWokeUp){
        //    Serial.println("OK es la primera medida desde q me desperté... Algo pasó... Me colgué?");
        //}
        GrabarDatoEnEEPROM(Lectura);
    }





    delay(1000);
    Evento+=1;
    // if(m_debug){
    //     Serial.println();
    // }

//    JustWokeUp = false;
}

bool EstaVacia(Dato Lectura){
    bool vacio = false;
    if ( Lectura.minuto + Lectura.hora + Lectura.dia + Lectura.mes + Lectura.anio  < 5 ){
        vacio = true;
    }
    return(vacio);

}

void ReiniciarEEPROM(){

    // Pone todos los registros de la eeprom en valores por defecto.
    // Datos en 0
    // Posicion en 0 y UnaMedidaCada en 4

    EEPROM[0] = 0;
    Posicion = 0;
    EEPROM[1] = 4;

    int idx = 2;
    while ( idx < MemoriaTotal) {
        Serial.print(idx);
        Serial.print("/");
        Serial.println(MemoriaTotal);
        EEPROM[idx] = 0;
        idx++;
        //delay(100);
    }
}






void MostrarDato(Dato Lectura){

    Serial.print(Lectura.anio);
    Serial.print("/");
    if (Lectura.mes<10) Serial.print("0");
    Serial.print(Lectura.mes);
    Serial.print("/");
    if (Lectura.dia<10) Serial.print("0");
    Serial.print(Lectura.dia);
    Serial.print(",");
    if (Lectura.hora<10) Serial.print("0");
    Serial.print(Lectura.hora);
    Serial.print(":");
    if (Lectura.minuto<10) Serial.print("0");
    Serial.print(Lectura.minuto);
    Serial.print(":00,");
    Serial.print(Lectura.Humedad0);
    Serial.print(",");
    Serial.println(Lectura.Humedad1);


}

void GrabarDatoEnEEPROM(Dato Lectura){
    Serial.print("Grabando en memoria  ");
    // Primero aumento el valor de la posición. Voy a escribir en el SIGUIENTE bloque
    Posicion = EEPROM[0]+1;
    // si la posición que se quiere grabar es mayor a 50, entonces la pone en 0
    if (2+Posicion*sizeof(Dato)>MemoriaTotal){
        Posicion=0;
    }
    EEPROM[0] = Posicion;
    Serial.println(Posicion);


    EEPROM.put(2+Posicion*sizeof(Dato), Lectura);
    lcd.setCursor(0, 0);
    lcd.print("Grabando:");

    //MostrarDato(Lectura);

}


int ValorHastaComa(){
    int resultado = -1;


    int idx = Valor.indexOf(",");
    //if(m_debug) Serial.print("idx = ");
    //if(m_debug) Serial.println(idx);
    if (idx>0){
        // if(m_debug) Serial.print("Valor Leido = ");
        String v_res = Valor.substring(0,idx);
        String ValorNuevo = Valor.substring(idx+1);
        Valor = ValorNuevo;
        // if(m_debug) Serial.println(v_res);
        resultado = v_res.toInt();
        // if(m_debug) Serial.println(resultado);
        // if(m_debug) Serial.println("Valor = "+Valor);
    } else {
        Serial.println("Algo esta mal:");
        Serial.println(Valor);
    }
    return(resultado);
}

//void ParseFecha(String Valor){
//    //if (m_debug){
//        Serial.print("Parse Fecha, FechaIdx = ");
//        Serial.println(FechaIdx);
//    //}
//    if (FechaIdx >= 5){
//        FechaIdx=0;
//        Serial.println("Return PareseFecha");
//        return;
//    }
//
//    int idx = Valor.indexOf(",");
//    Serial.print("idx = ");
//    Serial.println(idx);
//    if (idx>0){
//        Serial.print("NuevaFecha[FechaIdx] = ");
//        Serial.println(NuevaFecha[FechaIdx]);
//        NuevaFecha[FechaIdx] = Valor.substring(0,idx).toInt();
//        Serial.print("valor = ");
//        Serial.println(Valor);
//        Valor = Valor.substring(idx+1);
//        FechaIdx++;
//        ParseFecha(Valor);
//    }
//}

bool RtcPowerLost(){

    segundo=rtc.now().second();

    // Esperamos 2 segundos. Si los segundos siguen dando igual, entonces se perdió la energía)
    delay(1100);

    ErrorFecha = false;
    if (segundo - rtc.now().second() == 0) {
        //rtc.adjust(DateTime(2022,3,16,7,4));
        ErrorFecha = true;
    } 
    return(ErrorFecha);

}



void PrintHelp(){
    Serial.println("Acá tengo que mostrar los comandos posibles:");
    //Serial.println("============================================");
    Serial.println("F <Año>,<Mes>,<Día>,<Hora>,<Minuto>,<Segundo> -> Pone la fecha");
    Serial.println("M    -> Muestra datos de la memoria");
    Serial.println("B    -> Borra la memoria");
    Serial.println("L    -> Hace una lectura");
//    Serial.println("Debug   -> Muestra en pantalla un montón de giladas para depurar el programa");
    Serial.println("CH <Horas> -> Graba cada <Horas> horas");
//    Serial.println("CM <Minutos> -> Graba en memoria una medida cada <Minutos> minutos");
    Serial.println();
    // 

}
void ReadInput(){ 
    int idx = InputString.indexOf(" ");
    // if (m_debug){
    //     Serial.println("ReadInput = "+InputString);
    //     Serial.println(" idx  = "+String(idx));
    // }


    Command = InputString.substring(0,idx);
    Valor = InputString.substring(idx+1);

    // if (m_debug){
    //     Serial.println(" Command = " + Command);
    //     Serial.println(" arg  = " + Valor);
    // }
}

void serialEvent() {
    if (m_debug){
        Serial.println("serialEvent()");
    }
    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        Serial.print(inChar);
        // add it to the InputString:

        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if (inChar == '\n' ||inChar == '\r' ) {
            stringComplete = true;
            Serial.println();
        } else {
            InputString += inChar;
        }
    }
}
int Interpret(){
    // Return codes:
    // 0  -> All OK 
    // 1  -> Didnt recognize the command
    // 2  -> Command was recognized but expected a value
    int Resultado = 0;


    if (Command=="Debug"){
        // Switch Debug Flag
        if (m_debug){
            m_debug=false;
            Serial.print("Debug = NO");
            delay(1000);
        } else {
            m_debug=true;
            Serial.print("Debug = Sí");
            delay(1000);
        }
    }else if (Command == "F" ){
        // La fecha tiene que ser del modo
        // Año,mes,dia,hora,min,seg


        int m_anio = ValorHastaComa();
        if (m_anio  < 0 ) return(1);
        int m_mes = ValorHastaComa();
        if (m_mes  < 0 ) return(1);
        int m_dia = ValorHastaComa();
        if (m_dia  < 0 ) return(1);
        int m_hora = ValorHastaComa();
        if (m_hora  < 0 ) return(1);
        int m_min = ValorHastaComa();
        if (m_min  < 0 ) return(1);
        int m_seg = Valor.toInt();
        if (m_seg  < 0 ) return(1);

        rtc.adjust(DateTime( m_anio, m_mes, m_dia, m_hora, m_min, m_seg));
        ErrorFecha = false;
        FechaIdx=0;

    }else if( Command == "M"){
        // Arrancar con la posición actual +1 (que es el dato más viejo) e imprimirlos a todos
        int TotalDatos = MemoriaTotal / sizeof(Dato)  ;
        if (m_debug){
            Serial.print("sizeof(Dato) = ");
            Serial.print( sizeof(Dato) );
            Serial.print("Datos en EEPROM = ");
            Serial.println( TotalDatos );

        }
        Serial.print("EEPROM[0] = ");
        Serial.println( EEPROM[0] );

        Serial.print("EEPROM[1] = ");
        Serial.println( EEPROM[1] );
        int m_posicion=EEPROM[0];
        Serial.println("#Fecha,Humedad0,Humedad1");
        for (unsigned int i=0; i<TotalDatos; i++){
            m_posicion ++;
            if (m_posicion > TotalDatos){
                m_posicion -= TotalDatos;
            }
            Dato m_DatoEprom;
            EEPROM.get(2+m_posicion*sizeof(Dato), m_DatoEprom);
            if (!EstaVacia(m_DatoEprom)){
                if (m_debug){
                    Serial.print(i);
                    Serial.print(" ");
                    Serial.print(m_posicion);
                    Serial.print(" ");
                }
                MostrarDato(m_DatoEprom);
            }

        }
    } else if(Command == "B") {
        Serial.println("Borrando la EEPROM");
        ReiniciarEEPROM();
    }else if (Command == "CH"){
        int m_horas = Valor.toInt();
        if (m_horas <=0){
            Serial.print("Mmmm no entendi cada cuantas horas querés que guarde los datos: \" ");
            Serial.print(Valor);
            Serial.println("\"");
            Resultado=1;
        } else {
            UnaMedidaCada = m_horas;
            EEPROM[1] = UnaMedidaCada;
            UnidadTiempoDeEspera = 3600;

        }
    } else if (Command == "CM"){
        int m_minutos = Valor.toInt();
        if (m_minutos <=0){
            Serial.print("Mmmm no entendi cada cuantos minutos querés que guarde los datos: \" ");
            Serial.print(Valor);
            Serial.println("\"");
            Resultado=1;
        } else {
            UnaMedidaCada = m_minutos;
            EEPROM[1] = UnaMedidaCada;
            UnidadTiempoDeEspera = 60;
        }

    } else if (Command == "L") {
        // Hace una lectura y la muestra por el puerto serie
        MostrarDato(Lectura);
    } else {
        Resultado = 1;
    }
    return(Resultado);
}


void DumpEEPROM(){
    Serial.print("[0] = ");
    Posicion=EEPROM[0]; // Levanta la posicion de la ultima lectura
    Serial.println(Posicion);
    
    UnaMedidaCada = EEPROM[1];
    Serial.print("[1] = ");
    Serial.println(UnaMedidaCada);
}
