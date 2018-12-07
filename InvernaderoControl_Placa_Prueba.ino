// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include "RTClib.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Using version 1.2.1
//#include "rele.h"
//#include "menu.h"
// The LCD constructor - address shown is 0x27 - may or may not be correct for yours
// Also based on YWRobot LCM1602 IIC V1
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
RTC_DS1307 RTC;
String daysOfTheWeek[7] = { "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };
//rele rele1,rele2,rele3;
//String Fecha_inicio_Ciclo; // fecha que se almacena en el momento de configurar 
uint16_t F_Anio=2018;
uint8_t F_Mes=12;
uint8_t F_Dia=1;
uint8_t F_Hora=0;
uint8_t F_Min=0;
uint8_t F_Sec=0;

// Rotary encoder declarations

static int pinA = 2; // Our first hardware interrupt pin is digital pin 2
static int pinB = 3; // Our second hardware interrupt pin is digital pin 3
volatile byte aFlag = 0; // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0; // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile byte encoderPos = 0; //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
volatile byte oldEncPos = 0; //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)
volatile byte reading = 0; //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent

volatile byte fila =0;
volatile byte oldfila = 1;
volatile byte columna=0;
byte cursorLCD[8]  = {B00000, B00000, B00110, B01111, B01111, B00110, B00000};

// Button reading, including debounce without delay function declarations
const byte buttonPin = 4; // this is the Arduino pin we are connecting the push button to
byte oldButtonState = HIGH;  // assume switch open because of pull-up resistor
byte buttonState;
const unsigned long debounceTime = 10;  // milliseconds
unsigned long buttonPressTime;  // when the switch last changed state
boolean buttonPressed = 0; // a flag variable
////////////////////////////// Enter reading
const byte escapePin = 7; // this is the Arduino pin we are connecting the push button to
byte escapeState;
byte oldescapeState = HIGH;
boolean escapePressed = 0;
unsigned long escapePressTime;
///////////////////////////////////////////////////////////7



// Menu and submenu/setting declarations

byte Mode = 0;// This is which menu mode we are in at any given time (top level or one of the submenus)
byte oldMode = 0;
const byte modeMax = 3; // This is the number of submenus/settings you want
byte setting1 = 0;  // a variable which holds the value we set
byte setting2 = 0;  // a variable which holds the value we set
byte setting3 = 0;  // a variable which holds the value we set




void setup()
{

Wire.begin(); //configura el bus I2C estableciendo arduino como MASTER

//#########################    configuración del reloj de tiempo real (RTC)   ############################################################################
RTC.begin();
if (! RTC.isrunning()) // se verifica si el modulo rtc esta en funcionamiento, de lo contrario, se imprime: RTC is NOT running!
{
//lcd.println("RTC is NOT running!");
//delay(10000);

}
RTC.adjust(DateTime(F_Anio,F_Mes,F_Dia,F_Hora,F_Min,F_Sec)); //esta funcion establecera en el modulo la fecha de creación del archivo .hex generado al compilar el sketch
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

///################################################################################################################################

  //Rotary encoder section of setup
  pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  attachInterrupt(0,PinA,RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(1,PinB,RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
  // button section of setup
  pinMode (buttonPin, INPUT_PULLUP); // setup the button pin


  //Impresion Pantalla Bienvenida
  lcd.begin(20,4);
  LcdInicio();
  delay(3000);
  lcd.clear();

  //Crea caracter especial LCD
  lcd.createChar(0, cursorLCD);

}


void loop()
{

  rotaryMenu();
  controlButtonState();

}

//################################### Rotary Menu ###################################################3
void rotaryMenu()
{

//Pasa una sola vez dentro de la sentencia if al cambiar de  Menu

  switch (Mode)
  {
///////////////////////////////////////CASE 0//////////////////////////
    case 0:

      LcdMenuPrincipal();

      if (buttonPressed == 1)
      {
        lcd.clear();
        Mode = 1;
        buttonPressed = 0;
      }
      else Mode =0;

      break;
///////////////////////////////////////CASE 1//////////////////////////

    case 1:

      LcdMenuConfiguracion();
      controlMovEncoder();

      if (buttonPressed == 1  && fila == 0 )
      {
        lcd.clear();
        Mode = 2;
        buttonPressed = 0;

      }

      else if ( buttonPressed == 1  && fila == 1 )
      {
        lcd.clear();
        Mode = 3;
        buttonPressed = 0;
        //encoderPos=0;
      }

      else if ( buttonPressed == 1  && fila == 3 )
      {
        lcd.clear();
        Mode = 0;
        buttonPressed = 0;
        //encoderPos=0;
      }
      else Mode =1;


      break;

///////////////////////////////////////CASE 2//////////////////////////

    case 2:

      LcdMenuConfiguracionSalidas();
      controlMovEncoder();

      if (buttonPressed == 1  && fila == 0 )
      {
        lcd.clear();
        Mode = 4;
        buttonPressed = 0;
      }
      else if ( buttonPressed == 1   && fila == 1 )
      {
        lcd.clear();
        Mode = 5;
        buttonPressed = 0;
        //encoderPos=0;
      }
      else if ( buttonPressed == 1   && fila == 2 )
      {
        lcd.clear();
        Mode = 6;
        buttonPressed = 0;
        //encoderPos=0;
      }
      else if ( buttonPressed == 1  && fila == 3 )
      {
          lcd.clear();
          Mode = 1;
          buttonPressed = 0;
          //encoderPos=0;
      }
      else Mode =2;
      break;

///////////////////////////////////////CASE 3//////////////////////////

    case 3:
      LcdMenuConfiguracionTiempo();
      controlMovEncoder();

      if (buttonPressed == 1 && fila == 0)
      {
        buttonPressed = 0;
        lcd.clear();
        //Mode = 3;        
        Fecha();   
       }
      else if ( buttonPressed == 1   && fila == 1 )
      {
        lcd.clear();
        //Mode = 3;
        buttonPressed = 0;
       Hora();
      }
      else if ( buttonPressed == 1   && fila == 2 )
      {
        lcd.clear();
        Mode = 3;
        buttonPressed = 0;
        
      }
      else if ( buttonPressed == 1  && fila == 3 )
      {
          lcd.clear();
          Mode = 1;
          buttonPressed = 0;
          
      }
      else Mode =3;
      break;


///////////////////////////////////////CASE 4//////////////////////////


    case 4:

      LcdMenuConfiguracionReles();
      controlMovEncoder();

      if (buttonPressed == 1 && fila == 0)
      {
        lcd.clear();
        Mode = 4;
        buttonPressed = 0;
       }
      else if ( buttonPressed == 1   && fila == 1 )
      {
        lcd.clear();
        Mode = 4;
        buttonPressed = 0;
        //encoderPos=0;
      }
      else if ( buttonPressed == 1   && fila == 2 )
      {
        lcd.clear();
        Mode = 4;
        buttonPressed = 0;
        //encoderPos=0;
      }
      else if ( buttonPressed == 1  && fila == 3 )
      {
        lcd.clear();
        Mode = 2;
        buttonPressed = 0;
          //encoderPos=0;
      }
      else Mode =4;

      break;

///////////////////////////////////////CASE 5//////////////////////////


    case 5:

      LcdMenuConfiguracionReles();
      controlMovEncoder();

      if (buttonPressed == 1 && fila == 0)
      {
        lcd.clear();
        Mode = 5;
        buttonPressed = 0;
       }
      else if ( buttonPressed == 1   && fila == 1 )
      {
        lcd.clear();
        Mode = 5;
        buttonPressed = 0;
        //encoderPos=0;
      }
      else if ( buttonPressed == 1   && fila == 2 )
      {
        lcd.clear();
        Mode = 5;
        buttonPressed = 0;
        //encoderPos=0;
      }
      else if ( buttonPressed == 1  && fila == 3 )
      {
        lcd.clear();
        Mode = 2;
        buttonPressed = 0;
          //encoderPos=0;
      }
      else Mode =5;
      break;

///////////////////////////////////////CASE 6//////////////////////////

    case 6:

      LcdMenuConfiguracionReles();
      controlMovEncoder();

      if (buttonPressed == 1 && fila == 0)
      {
        lcd.clear();
        Mode = 6;
        buttonPressed = 0;
       }
      else if ( buttonPressed == 1   && fila == 1 )
      {
        lcd.clear();
        Mode = 6;
        buttonPressed = 0;
        //encoderPos=0;
      }
      else if ( buttonPressed == 1   && fila == 2 )
      {
        lcd.clear();
        Mode = 6;
        buttonPressed = 0;
        //encoderPos=0;
      }
      else if ( buttonPressed == 1  && fila == 3 )
      {
        lcd.clear();
        Mode = 2;
        buttonPressed = 0;
          //encoderPos=0;
      }
      else Mode =6;
      break;
}

}

///##################################### Fin rotary menu #############################################

void controlButtonState(){

  buttonState = digitalRead (buttonPin);

  if (buttonState != oldButtonState){
    if (millis () - buttonPressTime >= debounceTime){ // debounce
      buttonPressTime = millis ();  // when we closed the switch
      oldButtonState =  buttonState;  // remember for next time
      if (buttonState == LOW){
        buttonPressed = 1;
      }
      else{
        buttonPressed = 0;
      }
    }
  }//end del primer if
}

void controlescapeState(){

  escapeState = digitalRead (escapePin);

  if (escapeState != oldescapeState){
    if (millis () - escapePressTime >= debounceTime){ // debounce
      escapePressTime = millis ();  // when we closed the switch
      oldescapeState =  escapeState;  // remember for next time
      if (escapeState == LOW){
        escapePressed = 1;
      }
      else{
        escapePressed = 0;
      
      }
    }
  }
}

////////##################################################################

void controlMovEncoder(){

  if ( encoderPos <= 3 ){fila=0;}
  else if ( encoderPos > 3  && encoderPos <= 6){fila =1;}
  else if ( encoderPos > 6  && encoderPos <= 9){fila =2;}
  else if ( encoderPos > 9  && encoderPos <= 12){fila =3;}
  else if (encoderPos > 12 && encoderPos <= 15 ) {encoderPos = 0;}
  //else if (encoderPos < 255 && encoderPos > 240) {encoderPos = 13;}
   else if (encoderPos < 255) {encoderPos = 10;}
    lcd.setCursor(0,oldfila);
    lcd.print(" ");
    oldfila=fila;
    lcd.setCursor(0,fila);
    lcd.write(byte(0));
}

//int AcotEncoder(){
//int pepe;
//  if (encoderPos>60){pepe=0;}
//  else if ( encoderPos <= 255 ){encoderPos=60;}
//  return pepe;
//}

void LcdInicio()
{
  lcd.setCursor(7, 0); lcd.print("Arduino");
  lcd.setCursor(5, 1); lcd.print("Invernadero");
  lcd.setCursor(7, 2); lcd.print("Digital");
  lcd.setCursor(8, 3); lcd.print("2018");
}


void LcdMenuPrincipal()
{


  lcd.setCursor(0, 0); lcd.print("Ciclo:10D  Temp:18 C");
  lcd.setCursor(0, 1); lcd.print("HumAmb:33 %");
  lcd.setCursor(0, 2); lcd.print("HumSuelo:");
  lcd.setCursor(0, 3); lcd.print("Luz:Off     Vent:On ");
}

void LcdMenuConfiguracion()
{

  lcd.setCursor(1, 0); lcd.print("Configurar Salidas");
  lcd.setCursor(1, 1); lcd.print("Configurar Tiempo");
  lcd.setCursor(1, 3); lcd.print("Volver <<");
}
void LcdMenuConfiguracionSalidas()
{


  lcd.setCursor(1, 0); lcd.print("Rele Ventilacion");
  lcd.setCursor(1, 1); lcd.print("Rele Iluminacion");
  lcd.setCursor(1, 2); lcd.print("Rele Extra");
  lcd.setCursor(1, 3); lcd.print("Volver <<");
}
void LcdMenuConfiguracionTiempo()
{

  lcd.setCursor(1, 0); lcd.print("Fecha:");
  lcd.setCursor(1, 1); lcd.print("Hora:");
  lcd.setCursor(1, 2); lcd.print("Ciclo:");
  lcd.setCursor(1, 3); lcd.print("Volver <<");
}

void LcdMenuConfiguracionReles()
{

  lcd.setCursor(1, 0); lcd.print("Estado:");
  lcd.setCursor(1, 1); lcd.print("On:");
  lcd.setCursor(1, 2); lcd.print("Off:");
  lcd.setCursor(1, 3); lcd.print("Volver <<");
}



//Rotary encoder interrupt service routine for one encoder pin
void PinA(){
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
  if(reading == B00001100 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos --; //decrement the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei(); //restart interrupts

}

//Rotary encoder interrupt service routine for the other encoder pin
void PinB(){
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos ++; //increment the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei(); //restart interrupts

}



/////////////////////############ FUNCIONES DE CONFIGURACION DE TIEMPO #################################//////////////////////////////////7


void Fecha(){

mostrarFecha();

while (escapePressed==0){

editarFecha();

controlMovEncoder();

if (buttonPressed == 1 && fila == 1){
   
  //llamar funcion que incrementa Dia en display y setea en RTC
  incDia();
  buttonPressed = 0;
}//end if

else if ( buttonPressed == 1  && fila == 2 ){
  //llamar funcion que incrementa Mes en display y setea en RTC  
  incMes();
  buttonPressed = 0;
  }
else if ( buttonPressed == 1  && fila == 3 ){  
//llamar funcion que incrementa Año en display y setea en RTC
incAnio();
buttonPressed = 0;
}
//else{ encoderPos=4; }
controlButtonState(); 
controlescapeState();
}//end while
escapePressed=0;
//delay(5000);
lcd.clear();
Mode=3;
  
 }//end function

 void Hora(){
 
 mostrarHora();

while (escapePressed==0){

editarHora();

controlMovEncoder();

if (buttonPressed == 1 && fila == 1){
   
  //llamar funcion que incrementa Hora en display y setea en RTC
  incHora();
  buttonPressed = 0;
}//end if

else if ( buttonPressed == 1  && fila == 2 ){
  //llamar funcion que incrementa Minutos en display y setea en RTC  
  incMin();
  buttonPressed = 0;
  }
else if ( buttonPressed == 1  && fila == 3 ){  
//llamar funcion que incrementa Segundos en display y setea en RTC
incSec();
buttonPressed = 0;
}
//else{ encoderPos=4; }
controlButtonState(); 
controlescapeState();
}//end while
escapePressed=0;
//delay(5000);
lcd.clear();
Mode=3; 
 }//end function
 



void incDia(){  

 while(escapePressed==0){

//   AcotEncoder();
	 int incDia=encoderPos;
	 
if ( incDia >= 32 && incDia < 63 ) {incDia=incDia-31;}
else if ( incDia >= 63 && incDia < 94 ){incDia=incDia-62;}
else if ( incDia >= 94 && incDia < 125){incDia=incDia-93;}
else if ( incDia >= 125 && incDia < 156){incDia=incDia-124;}
else if ( incDia >= 156 && incDia < 187){incDia=incDia-155;}
//else if ( incDia >= 144 && incDia < 168){incDia=incDia-144;}
// else if ( incDia >= 168 && incDia < 192){incDia=incDia-168;}
// else if ( incDia >= 192 && incDia < 216){incDia=incDia-192;}
// else if ( incDia >= 216 && incDia < 240){incDia=incDia-216;}
// else if ( incDia >= 240 && incDia < 255){incDia=incDia-240;}
 
  F_Dia = incDia;

  editarFecha();
  
  controlescapeState();
  }//end while
 escapePressed=0;
}

  void incMes(){  
while(escapePressed==0){
	int incMes=encoderPos;
  
if ( incMes >= 13 && incMes < 26 ) {incMes=incMes-12;}
else if ( incMes >= 25 && incMes < 37 ){incMes=incMes-24;}
else if ( incMes >= 37 && incMes < 49){incMes=incMes-36;}
else if ( incMes >= 49 && incMes < 61){incMes=incMes-48;}
else if ( incMes >= 61 && incMes < 73){incMes=incMes-60;}
else if ( incMes >= 73 && incMes < 85){incMes=incMes-72;} 
else if ( incMes >= 85 && incMes < 97){incMes=incMes-84;} 
else if ( incMes <= 255 && incMes > 225){incMes=incMes-224;}
else if ( incMes <= 224 && incMes > 194){incMes=incMes-193;}
else if ( incMes <= 194 && incMes > 163){incMes=incMes-162;}

  F_Mes = incMes;

  editarFecha();
  controlescapeState();
  }//end while
  escapePressed=0;

  }

void incAnio(){  

 while(escapePressed==0){
	 int incAnio=encoderPos+2018; 
if ( incAnio >= 2029 && incAnio < 2040){incAnio=incAnio-11;}
else if ( incAnio >=2040 && incAnio < 2051   ){incAnio=incAnio-22;} 
else if ( incAnio >=2051 && incAnio < 2062   ){incAnio=incAnio-33;}
else if ( incAnio >=2062 && incAnio < 2073   ){incAnio=incAnio-44;} 
else if ( incAnio <=2273 && incAnio > 2262   ){incAnio=incAnio-245;}
else if ( incAnio <=2262 && incAnio > 2251   ){incAnio=incAnio-234;}
else if ( incAnio <=2251 && incAnio > 2240   ){incAnio=incAnio-223;}
else if ( incAnio <=2240 && incAnio > 2229   ){incAnio=incAnio-212;}

  F_Anio = incAnio;

  editarFecha();
  controlescapeState();
  }//end while
  escapePressed=0;
}
  
void incHora(){  

 while(escapePressed==0){
	 
	 int incHora=encoderPos;
   
if ( incHora >= 24 && incHora < 48 ) {incHora=incHora-24;}
else if ( incHora >= 48 && incHora < 72 ){incHora=incHora-48;}
 else if ( incHora >= 72 && incHora < 96 ){incHora=incHora-72;}
   else if ( incHora >= 96 && incHora < 120 ){incHora=incHora-96;}
   else if ( incHora <= 255 && incHora > 231 ){incHora=incHora-255;}
   else if ( incHora >= 231 && incHora > 207 ){incHora=incHora-231;}
   
  F_Hora = incHora;

  editarHora();
  
  controlescapeState();
  }//end while
 escapePressed=0;
}

void incMin(){  

 while(escapePressed==0){
   
   int incMin=encoderPos;
   
if ( incMin >= 60 && incMin < 120 ) {incMin=incMin-60;}
else if ( incMin >= 120 && incMin < 240 ) {incMin=incMin-120;}
else if ( incMin <= 255 && incMin > 196 ) {incMin=incMin-196;}
else if ( incMin <= 196 && incMin > 137 ) {incMin=incMin-137;}
  F_Min = incMin;

  editarHora();
  
  controlescapeState();
  }//end while
 escapePressed=0;
}  

void incSec(){  

 while(escapePressed==0){
   
   int incSec=encoderPos;
   
if ( incSec >= 60 && incSec < 120 ) {incSec=incSec-60;}
else if ( incSec >= 120 && incSec < 240 ) {incSec=incSec-120;}
 else if ( incSec <= 255 && incSec > 195 ) {incSec=incSec-196;}
 else if ( incSec <= 196 && incSec > 137 ) {incSec=incSec-137;}
  F_Sec = incSec;

  editarHora();
  
  controlescapeState();
  }//end while
 escapePressed=0;
}  
void editarFecha(){
  
lcd.setCursor(1,1);
lcd.print("Dia: ");
lcd.setCursor(6,1);
lcd.print("  ");
lcd.setCursor(6,1);
lcd.print(F_Dia);

lcd.setCursor(1,2);
lcd.print("Mes: ");
lcd.setCursor(6,2);
lcd.print("  ");
lcd.setCursor(6,2);
lcd.print(F_Mes);

lcd.setCursor(1,3);
lcd.print("Anio: ");
lcd.setCursor(6,3);
lcd.print(F_Anio);
}
void editarHora(){
	
lcd.setCursor(1,1);
lcd.print("Hora: ");
lcd.setCursor(6,1);
lcd.print("  ");
lcd.setCursor(6,1);
lcd.print(F_Hora);

lcd.setCursor(1,2);
lcd.print("Min: ");
lcd.setCursor(6,2);
lcd.print("  ");
lcd.setCursor(6,2);
lcd.print(F_Min);

lcd.setCursor(1,3);
lcd.print("Sec: ");
lcd.setCursor(6,3);
lcd.print("  ");
lcd.setCursor(6,3);
lcd.print(F_Sec);
}


void mostrarFecha() //Funcion que lee los datos de modulo RTC y despues los imprime en el display
{
RTC.adjust(DateTime(F_Anio,F_Mes,F_Dia,F_Hora,F_Min,F_Sec));	
DateTime now = RTC.now(); //obtiene datos del modulo RTC
lcd.clear();
lcd.setCursor(1,0);
  if(now.day() < 10)
  {
  lcd.print("0");
  }
lcd.print(now.day(), DEC); //imprime dia
lcd.print('/');
  if(now.month() < 10)
  {
  lcd.print("0");
  }
lcd.print(now.month(), DEC); //imprime mes
lcd.print('/');
lcd.print(now.year(), DEC);  //imprime el año

//lcd.setCursor(0,2); 
//lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);

}

void mostrarHora(){
	
RTC.adjust(DateTime(F_Anio,F_Mes,F_Dia,F_Hora,F_Min,F_Sec));	
DateTime now = RTC.now(); //obtiene datos del modulo RTC
lcd.clear();
lcd.setCursor(1,0);
 if(now.hour() < 10)
 {
 lcd.print("0");
 }
 // Pasamos a la linea de abajo
lcd.setCursor(1,0);  
lcd.print(now.hour(), DEC); //imprime hora
lcd.print(':');
 if(now.minute() < 10)
 {
 lcd.print("0");
 }
lcd.print(now.minute(), DEC); //imprime minutos
lcd.print(':');
 if(now.second() < 10)
 {
 lcd.print("0");
 }
lcd.print(now.second(), DEC); //imprime segundos

}
