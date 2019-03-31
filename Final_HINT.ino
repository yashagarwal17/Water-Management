#include <OneWire.h>
#include <DallasTemperature.h>

 

#include <LiquidCrystal.h>
int level1=A2;
int level2=A3;
int motor=6;
int a;
int b;
//************************* User Defined Variables ********************************************************//

int R1= 1000;
int Ra=25; //Resistance of powering Pins
int ECPin= A0;
int ECGround=A1;
int ECPower =A4;
 
float PPMconversion=0.7;
 
//*************Compensating for temperature ************************************//
float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring

//********************** Cell Constant For Ec Measurements *********************//
//Mine was around 2.9 with plugs being a standard size they should all be around the same
//But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
float K=2.88;
  
//************ Temp Probe Related *********************************************//
#define ONE_WIRE_BUS 10          // Data wire For Temp Probe is plugged into pin 10 on the Arduino
const int TempProbePossitive =8;  //Temp Probe power connected to pin 9
const int TempProbeNegative=9;    //Temp Probe Negative connected to pin 8
 
//***************************** END Of Recomended User Inputs *****************************************************************//
OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.
  
float Temperature=25;
float EC=0;
float EC25 =0;
int ppm =0;
 
 
float raw= 0;
float Vin= 5;
float Vdrop= 0;
float Rc= 0;
float buffer=0;
 
//*********************************Setup - runs Once and sets pins etc ******************************************************//
void setup()
{
  Serial.begin(9600);
  pinMode(TempProbeNegative , OUTPUT ); //seting ground pin as output for tmp probe
  digitalWrite(TempProbeNegative , LOW );//Seting it to ground so it can sink current
  pinMode(TempProbePossitive , OUTPUT );//ditto but for positive
  digitalWrite(TempProbePossitive , HIGH );
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);//Setting pin for sourcing current
  pinMode(ECGround,OUTPUT);//setting pin for sinking current
  digitalWrite(ECGround,LOW);//We can leave the ground connected permanantly
 
  delay(100);// gives sensor time to settle
  sensors.begin();
  delay(100);
  //** Adding Digital Pin Resistance to [25 ohm] to the static Resistor *********//
  // Consule Read-Me for Why, or just accept it as true
  R1=(R1+Ra);// Taking into acount Powering Pin Resitance

  ArduinoUno.begin(4800);
  pinMode(3,INPUT);
  pinMode(2,OUTPUT);

  pinMode(level1,INPUT);
  pinMode(level2,INPUT);
  pinMode(motor,OUTPUT);
  digitalWrite(motor,LOW);
};
//******************************************* End of Setup **********************************************************************//
 
//************************************* Main Loop - Runs Forever ***************************************************************//
//Moved Heavy Work To subroutines so you can call them from main loop without cluttering the main loop
void loop()
{
  GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
  PrintReadings();  // Cals Print routine [below main loop]
  delay(5000);
  
  a=analogRead(level1);
  b=analogRead(level2);
  delay(500);
  Serial.println(a);
  //delay(500);
  Serial.println(b);
  //delay(1000);
//      digitalWrite(motor,HIGH);
  int flag=0;
  if(a>10 && b>10 && ppm<500)
  {
    while(b>0)
    {
        Serial.println("FLOW OUT!");
        b=analogRead(level2);
        delay(500);
        digitalWrite(motor,HIGH);
        Serial.println(b);
    }
    flag=1;
    digitalWrite(motor,LOW);
    delay(5000);
    
  }
  else
  {
        digitalWrite(motor,LOW);
  } 
  if(flag==1)
  {
    ArduinoUno.print("1");
    ArduinoUno.println("\n");
  }
  else
  {
    ArduinoUno.print("0");
    ArduinoUno.println("\n");
  }  
}
//************************************** End Of Main Loop **********************************************************************//
 
//************ This Loop Is called From Main Loop************************//
void GetEC(){
  Temperature=25;
  //************Estimates Resistance of Liquid ****************//
  digitalWrite(ECPower,HIGH);
  raw= analogRead(ECPin);
  raw= analogRead(ECPin);// This is not a mistake, First reading will be low beause if charged a capacitor
  digitalWrite(ECPower,LOW);
  
//***************** Converts to EC **************************//
Vdrop= (Vin*raw)/1024.0;
Rc=(Vdrop*R1)/(Vin-Vdrop);
Rc=Rc-Ra; //acounting for Digital Pin Resitance
EC = 1000/(Rc*K);
 
 
//*************Compensating For Temperaure********************//
EC25  =  EC/ (1+ TemperatureCoef*(Temperature-25.0));
ppm=(EC25)*(PPMconversion*1000);
 
 
;}
//************************** End OF EC Function ***************************//
 
//***This Loop Is called From Main Loop- Prints to serial usefull info ***//
void PrintReadings(){
//Serial.print("Rc: ");
ArduinoUno.print(Rc);

//ArduinoUno.print(" EC: ");
//ArduinoUno.print(EC25);
ArduinoUno.println("\n");
//ArduinoUno.println(" Simens  ");
ArduinoUno.print(ppm);
ArduinoUno.println("\n");
//ArduinoUno.println(" ppm  ");
ArduinoUno.print(Temperature);
ArduinoUno.println("\n");
//ArduinoUno.print(" *C ");
//ArduinoUno.println("\n");
 
 
/*
//********** Usued for Debugging ************
Serial.print("Vdrop: ");
Serial.println(Vdrop);
Serial.print("Rc: ");
Serial.println(Rc);
Serial.print(EC);
Serial.println("Siemens");
//********** end of Debugging Prints *********
*/
};