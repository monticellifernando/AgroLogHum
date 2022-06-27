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

#include "AgroLogHum.h"
// ============= Includes ===========

void setup()                    // run once, when the sketch starts
{

    MinSerial.begin(115200);           // set up MinSerial library at 115200 bps
    //MinSerial.println("Setting up...");
    MinSerial.println(_ID_);
    //ReiniciarEEPROM(); //<--- Hack para reiniciar la EEPROM

    //if (MinSerial) MinSerial.println("LCD...");
    //if (MinSerial) MinSerial.println("LCD clear...");
    lcd.clear();
    //if (MinSerial) MinSerial.println("LCD begin...");
    lcd.begin(16, 2);
    //if (MinSerial) MinSerial.println("LCD print...");
    lcd.print("Setup...");
    // lcd.setCursor(0, 1);
    // lcd.print(MemoriaTotal);
    // lcd.print("B free");

    TamanoDato = sizeof(Dato);

    delay(1000);
    //lcd.clear();
    //lcd.print("Iniciando reloj");

    //MinSerial.println("Inicializando Reloj...");
    //Inicializamos el RTC
    while (!rtc.begin()){
        lcd.clear();
        String errormsg = "ERROR:reloj";
        lcd.print(errormsg);
        MinSerial.println(errormsg);
        delay(1000);
    }
    // lcd.clear();
    // lcd.print("Reloj oK!");
    // delay(100);

    // Inicializar el sensor de temperatura
    //MinSerial.println("Inicializando sensor...");
    //lcd.print("Iniciando sensor");

    while (!aht.begin()){
        lcd.clear();
        String errormsg = "ERROR:sensor";
        lcd.print(errormsg);
        MinSerial.println(errormsg);
        delay(1000);
    }
    //lcd.clear();
    //lcd.print("Sensor OK");
    //delay(100);

    // Inicialización de tarjeta SD:
    //lcd.clear();
    //lcd.print("SD OK!");
    //delay(100);

    //DumpEEPROM();



    lcd.clear();
    lcd.print("hora?");
    //MinSerial.println("Revisando reloj:");
    if (RtcPowerLost()) {
        lcd.setCursor(0, 1);
        lcd.print("ERROR FECHA");
        MinSerial.println("Pone la fecha:");
        //delay(500);

    }

    //MinSerial.println("#t,Temp,Hum");  // prints hello with ending line break 

}

void loop() {

    // Leer la configuración de la EEPROM
    Posicion = EEPROM[0];
    UnaMedidaCada = EEPROM[1];
    UnidadTiempoDeEspera = EEPROM[2];

    //if(m_debug){
    //    MinSerial.println("Loop()");
    //}

    // Puerto Serie
    if (stringComplete){
        stringComplete = false;
        String Command="";
        String Value = "";
        ReadInput();

        // if (m_debug){
        //     MinSerial.println("Comando "+Command);
        //     MinSerial.println("Valor "+String(Value));
        // }
        int Result = Interpret();
        if (Result>0){
            MinSerial.println("No entendi eso :-P ");
            // MinSerial.println("Interpret devuelve " + String(Result));
            // MinSerial.println("ERROR:"+String(Result));
            PrintHelp();
        }
        InputString = "";
    }
    serialEvent();

    // lectura de AHT10 
    sensors_event_t hum, temp;
    // Decirle al sensor que tome los datos
    aht.getEvent(&hum, &temp);

    float Humedad = hum.relative_humidity;
    float TempC =   temp.temperature;

    if (isnan(Humedad)){
        Humedad = -99;
    }

    if(isnan(TempC)) {
        TempC = -99;
    }

    if (TempC == 0){
        //lcd.clear();
        //lcd.print("Temperatura 0");
        //delay(500);
        return;
    }







    // Chequeamos la hora
    // if (m_debug){
    //     MinSerial.println("Levantando la fecha");
    // }
    HoraFecha = rtc.now(); //obtenemos la hora y fecha actual


    Lectura.segundo=HoraFecha.second(); // No lo pongo en dato para que no me llene la EEPROM
    Lectura.Humedad = Humedad;
    Lectura.Temperatura = TempC;
    Lectura.minuto= HoraFecha.minute();
    Lectura.hora= HoraFecha.hour();
    Lectura.dia= HoraFecha.day();
    Lectura.mes= HoraFecha.month();
    Lectura.anio= HoraFecha.year()-2000;
    //diaDeLaSemana=HoraFecha.dayOfTheWeek();






    //if (Evento%MostrarCada == 0) { 
    //    MostrarDato(Lectura);
    //}

    if (Evento%(MostrarCada/10) == 0) { 

        lcd.clear();
        lcd.begin(16, 2);
        lcd.setCursor(0, 0);

        if(ErrorFecha){
            lcd.print("ERRORFecha");
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
            if (Lectura.segundo<10) lcd.print("0");
            lcd.print(String(Lectura.segundo));
        }

        lcd.setCursor(0, 1);
        lcd.print(Lectura.Temperatura);
        lcd.print("C ");
        lcd.print(Lectura.Humedad);
        lcd.print("% ");
    }

    // Primero chequeamos que no tengamos una medición hecha en el futuro:
    Dato LecturaAnterior = LeerDeArchivo();
    //EEPROM.get(2+Posicion*TamanoDato, LecturaAnterior);
    DateTime EstaMedida(Lectura.anio, Lectura.mes, Lectura.dia, Lectura.hora, Lectura.minuto, Lectura.segundo);
    DateTime MedidaAnterior(LecturaAnterior.anio, LecturaAnterior.mes, LecturaAnterior.dia, LecturaAnterior.hora, LecturaAnterior.minuto, LecturaAnterior.segundo);
    // if (m_debug){
    //     MinSerial.println("Esta Medida:");
    //     MostrarDato(Lectura);
    //     MinSerial.print("Ultima Medida Guardada, Posicion ");

    //     MinSerial.println(Posicion);
    //     MostrarDato(LecturaAnterior);
    // }

    TimeSpan Diff = EstaMedida - MedidaAnterior  ;


    // Ahora revisar si hay que guardar en la EEPROM
    if (!ErrorFecha && (Diff.totalseconds() > UnaMedidaCada*UnidadTiempoDeEspera*60 || EstaVacia(LecturaAnterior))){
        //if (JustWokeUp){
        //    MinSerial.println("OK es la primera medida desde q me desperté... Algo pasó... Me colgué?");
        //}
        //GrabarDatoEnEEPROM(Lectura);
        GrabarDatoEnSD();
    }





    delay(1000);
    Evento+=1;
    // if(m_debug){
    //     MinSerial.println();
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
    // Posicion en 0 y UnaMedidaCada en 4, UnidadTiempoDeEspera en 60

    EEPROM[0] = 0;
    
    EEPROM[1] = 4;

    EEPROM[2] = 60;

    int idx = 3;
    while ( idx < MemoriaTotal) {
        MinSerial.print(idx);
        MinSerial.print("/");
        MinSerial.println(MemoriaTotal);
        EEPROM[idx] = 0;
        idx++;
        //delay(100);
    }

}





String MostrarDato(Dato Lectura){

    String Output = "";
    
    Output += Lectura.anio;

    Output += ("/");
    if (Lectura.mes<10) Output += ("0");
    Output += (Lectura.mes);
    Output += ("/");
    if (Lectura.dia<10) Output += ("0");
    Output += (Lectura.dia);
    Output += (",");
    if (Lectura.hora<10) Output += ("0");
    Output += (Lectura.hora);
    Output += (":");
    if (Lectura.minuto<10) Output += ("0");
    Output += (Lectura.minuto);
    Output += (":");
    if (Lectura.segundo<10) Output += ("0");
    Output += (Lectura.segundo);
    Output += (",");
    Output += (Lectura.Temperatura);
    Output += (",");
    Output += (Lectura.Humedad);

    return(Output);
}

//void GrabarDatoEnEEPROM(Dato Lectura){
//    //MinSerial.print("Grabando en memoria  ");
//    // Primero aumento el valor de la posición. Voy a escribir en el SIGUIENTE bloque
//    Posicion = EEPROM[0]+1;
//    // si la posición que se quiere grabar es mayor a 50, entonces la pone en 0
//    if (2+Posicion*TamanoDato>MemoriaTotal){
//        Posicion=0;
//    }
//    EEPROM[0] = Posicion;
//    //MinSerial.println(Posicion);
//
//
//    EEPROM.put(3+Posicion*TamanoDato, Lectura);
//    lcd.setCursor(0, 0);
//    lcd.print("Grabando:");
//
//    //MostrarDato(Lectura);
//
//}

void GrabarDatoEnSD(){
    lcd.setCursor(14, 0);
    lcd.print("G");
    //MinSerial.println("Grabando");
    CheckSD();
    // Abro el archivo primero:
    //file.writeError = false;
    file = SD.open(Archivo, FILE_WRITE);
    if (!file){
        MinSerial.println("GrabarERROR abrir");
        return;
    }
    if (file.size() < 10){
        // File esta vacío (o roto). Asi que le mandamos el encabezado
        file.println("#Fecha,Hora,Temperatura,Humedad");
    }

    file.println(MostrarDato(Lectura));

    if (!file.close()) {
        MinSerial.println("ERROR cerrar"); 
        return;
    }
    //MostrarDato(Lectura,file);

    //MostrarDato(Lectura);

}


int ValorHastaComa(){
    int resultado = -1;


    int idx = Valor.indexOf(",");
    //if(m_debug) MinSerial.print("idx = ");
    //if(m_debug) MinSerial.println(idx);
    if (idx>0){
        // if(m_debug) MinSerial.print("Valor Leido = ");
        String v_res = Valor.substring(0,idx);
        String ValorNuevo = Valor.substring(idx+1);
        Valor = ValorNuevo;
        // if(m_debug) MinSerial.println(v_res);
        resultado = v_res.toInt();
        // if(m_debug) MinSerial.println(resultado);
        // if(m_debug) MinSerial.println("Valor = "+Valor);
    // } else {
    //     MinSerial.println("Algo esta mal:");
    //     MinSerial.println(Valor);
    }
    return(resultado);
}


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
    MinSerial.println("Comandos posibles:");
    //MinSerial.println("============================================");
    MinSerial.println("B -> Borra la memoria");
//    MinSerial.println("C -> Muestra configuración");
    MinSerial.println("CH <Horas> -> Graba cada <Horas> horas");
//    MinSerial.println("CM <Minutos> -> Graba en memoria una medida cada <Minutos> minutos");
//    MinSerial.println("D   -> Muestra en pantalla un montón de giladas para depurar el programa");
    MinSerial.println("F <Año>,<Mes>,<Día>,<Hora>,<Minuto>,<Segundo> -> Pone fecha");
    MinSerial.println("G -> Graba datos");
    MinSerial.println("L -> muestra datos");
    MinSerial.println();
    // 

}
void ReadInput(){ 
    int idx = InputString.indexOf(" ");
    // if (m_debug){
    //     MinSerial.println("ReadInput = "+InputString);
    //     MinSerial.println(" idx  = "+String(idx));
    // }


    Command = InputString.substring(0,idx);
    Valor = InputString.substring(idx+1);

    // if (m_debug){
    //     MinSerial.println(" Command = " + Command);
    //     MinSerial.println(" arg  = " + Valor);
    // }
}

void serialEvent() {
    //if (m_debug){
    //    MinSerial.println("serialEvent()");
    //}
    while (MinSerial.available()) {
        // get the new byte:
        char inChar = (char)MinSerial.read();
        MinSerial.print(inChar);
        // add it to the InputString:

        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if (inChar == '\n' ||inChar == '\r' ) {
            stringComplete = true;
            MinSerial.println();
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


    //if (Command=="D"){
    //    // Switch Debug Flag
    //    if (m_debug){
    //        m_debug=false;
    //        MinSerial.print("Debug = NO");
    //        delay(1000);
    //    } else {
    //        m_debug=true;
    //        MinSerial.print("Debug = Sí");
    //        delay(1000);
    //    }
    //}else 
    if (Command == "F" ){
        // La fecha tiene que ser del modo
        // Año,mes,dia,hora,min,seg


        int m_anio = ValorHastaComa();
        if (m_anio  < 0 ) { // Si solo tipeas F, entonces te muestra la fecha y la medicion
            MinSerial.println(MostrarDato(Lectura));
            return(Resultado);
        }
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

    }else if( Command == "C"){ 
        DumpEEPROM();
    }else if( Command == "G"){
        GrabarDatoEnSD();
    }else if( Command == "L"){
        Log();
    } else if(Command == "B") {
        MinSerial.println("Borrando...");
        SD.remove(Archivo);
        //ReiniciarEEPROM();
    }else if (Command == "CH"){
        int m_horas = Valor.toInt();
        if (m_horas <=0){
            MinSerial.print("No entendi -> \" ");
            MinSerial.print(Valor);
            MinSerial.println("\"");
            Resultado=1;
        } else {
            UnaMedidaCada = m_horas;
            EEPROM[1] = UnaMedidaCada;
            UnidadTiempoDeEspera = 60;
            EEPROM[2] = UnidadTiempoDeEspera;

        }
    } else if (Command == "ls"){
        ls();
    } else if (Command == "CM"){
        int m_minutos = Valor.toInt();
        if (m_minutos <= 0){
            MinSerial.print("No entendi -> \" ");
            MinSerial.print(Valor);
            MinSerial.println("\"");
            Resultado=1;
        } else {
            UnaMedidaCada = m_minutos;
            EEPROM[1] = UnaMedidaCada;
            UnidadTiempoDeEspera = 1;
            EEPROM[2] = UnidadTiempoDeEspera;
        }

    } else if (Command == "ID"){
        MinSerial.println(_ID_);
    } else {
        Resultado = 1;
    }
    return(Resultado);
}


void DumpEEPROM(){
    for (unsigned int i = 0; i<3 ; i++){
        MinSerial.print("[");
        MinSerial.print(i);
        MinSerial.print("] = ");
        MinSerial.println(EEPROM[i]);
    }
}

void CheckSD(){
    while (!SD.begin(SD_ChipSelect)){
        lcd.clear();
        String errormsg = "ERROR:SD";
        lcd.print(errormsg);
        MinSerial.println(errormsg);
        delay(1000);
    }
}

void ls(){
    CheckSD();
    File dir;
    if (!dir.open("/")){
        error("dir.open failed");
    }
    // Open next file in root.
    // Warning, openNext starts at the current position of dir so a
    // rewind may be necessary in your application.
    while (file.openNext(&dir, O_RDONLY)) {
        file.printFileSize(&MinSerial);
        MinSerial.write(' ');
        file.printModifyDateTime(&MinSerial);
        MinSerial.write(' ');
        file.printName(&MinSerial);
        if (file.isDir()) {
            // Indicate a directory.
            MinSerial.write('/');
        }
        MinSerial.println();
        file.close();
    }

}

void Log(){
    // EEPROM Log
    /* 
    // Arrancar con la posición actual +1 (que es el dato más viejo) e imprimirlos a todos
    int TotalDatos = MemoriaTotal / TamanoDato  ;
    // if (m_debug){
    //     //MinSerial.print("TamanoDato = ");
    //     //MinSerial.print( TamanoDato );
    //     //MinSerial.print("Datos en EEPROM = ");
    //     //MinSerial.println( TotalDatos );
    //     MinSerial.print("EEPROM[0] = ");
    //     MinSerial.println( EEPROM[0] );

    //     MinSerial.print("EEPROM[1] = ");
    //     MinSerial.println( EEPROM[1] );

    // }
    int m_posicion=EEPROM[0];
    MinSerial.println("#Fecha,Hora,Temperatura,Humedad");
    for (unsigned int i=0; i<TotalDatos; i++){
    m_posicion ++;
    if (m_posicion > TotalDatos){
    m_posicion -= TotalDatos;
    }
    Dato m_DatoEprom;
    EEPROM.get(2+m_posicion*TamanoDato, m_DatoEprom);
    if (!EstaVacia(m_DatoEprom)){
    if (m_debug){
    MinSerial.print(i);
    MinSerial.print(" ");
    MinSerial.print(m_posicion);
    MinSerial.print(" ");
    }
    MostrarDato(m_DatoEprom, MinSerial);
    }

    }
     */

    // Open file for reading:
    file = SD.open(Archivo, FILE_READ);
    if (!file){
        MinSerial.println("LOGERROR abrir");
        return;
    }
    while(file.available()) {
        MinSerial.write(file.read());
    }
    file.close();
    MinSerial.println();
}
    
Dato LeerDeArchivo(){ 
    // Lee la ultima entrada del archivo de log en una estructura de Dato

    CheckSD();
    file = SD.open(Archivo, FILE_WRITE);
    Dato m_dato;
    // Levantar la ultima entrada
    if (!file){
        MinSerial.println("ERROR leer");
        return(m_dato);
    }
    unsigned long int f_size = file.size();
    file.seek(f_size-31);

    String m_string = "";
    char m_char;
    while (file.available()){
        m_char = file.read();
        m_string += m_char;
    }
    //MinSerial.println(m_string);
    file.close();


    // m_dato.anio        = m_string.substring(1,2).toInt();
    // m_dato.mes         = m_string.substring(3,4).toInt();
    // m_dato.dia         = m_string.substring(6,7).toInt();
    // m_dato.hora        = m_string.substring(9,10).toInt();
    // m_dato.minuto      = m_string.substring(12,13).toInt();
    // m_dato.segundo     = m_string.substring(15,16).toInt();
    // m_dato.Temperatura = m_string.substring(18,22).toFloat();
    // m_dato.Humedad     = m_string.substring(24).toFloat();

    m_dato.anio        = (m_string.substring(0,2).toInt());
    m_dato.mes         = (m_string.substring(3,5).toInt());
    m_dato.dia         = (m_string.substring(6,8).toInt());
    m_dato.hora        = (m_string.substring(9,11).toInt());
    m_dato.minuto      = (m_string.substring(12,15).toInt());
    m_dato.segundo     = (m_string.substring(15,18).toInt());
    m_dato.Temperatura = (m_string.substring(18,22).toFloat());
    m_dato.Humedad     = (m_string.substring(24).toFloat());

    //MinSerial.println(m_string.substring(0,2).toInt());
    //MinSerial.println(m_string.substring(3,5).toInt());
    //MinSerial.println(m_string.substring(6,8).toInt());
    //MinSerial.println(m_string.substring(9,11).toInt());
    //MinSerial.println(m_string.substring(12,15).toInt());
    //MinSerial.println(m_string.substring(15,18).toInt());
    //MinSerial.println(m_string.substring(18,22).toFloat());
    //MinSerial.println(m_string.substring(24).toFloat());

    //MinSerial.println(MostrarDato(m_dato));

    return(m_dato);

}
