#include <math.h>

//Screen
#include <Arduino.h>
#include <Wire.h>
#include <MicroLCD.h>

//LCD_SH1106 lcd; /* for SH1106 OLED module */
LCD_SSD1306 lcd; /* for SSD1306 OLED module */


//MOTOR DRIVER
const int motorAPin1 = 9;   // AIN1
const int motorAPin2 = 8;   // AIN2
const int enablePin = 2;    // H-bridge enable pin
const int ledPin = 12;      // LED 
const int PWMA = 3;         // Pulse Width Modulation Motor A
float Temp;
float psi;
float psiold;
float psicounter;
unsigned long starttime1 =0;
unsigned long starttime =0;
unsigned long starttime2 =0;
unsigned long starttime3 =0;
int rxnstart =0;
String state;

unsigned long timer = 0;
unsigned long timer2 = 0;
unsigned long timer3 =0;
unsigned long timer4 =0;

int offstate = 0;
//MOSFET CONFIGURATION
#define switchPin 12
#define switchPin2 13

//Thermistor
#define sensorPin A0


//purging
#define purgePin 13

void setup() {
  // put your setup code here, to run once:


  // set MOTOR DRIVER outputs:
  pinMode(motorAPin1, OUTPUT); 
  pinMode(motorAPin2, OUTPUT); 
  pinMode(enablePin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(11,OUTPUT);
  // set enablePin high so that motor can turn on:
  
  pinMode(switchPin, OUTPUT);
  pinMode(switchPin2,OUTPUT);
  
  digitalWrite(switchPin,LOW);
  digitalWrite(switchPin2,LOW);
  digitalWrite(purgePin,LOW);
  
  digitalWrite(enablePin, LOW);
  digitalWrite(motorAPin1, LOW);
  digitalWrite(motorAPin2, HIGH);
  analogWrite(PWMA, 100);
  digitalWrite(purgePin,LOW);
  digitalWrite(11,HIGH);
  lcd.begin();
  Serial.begin(9600);
}

void loop() {

      
  // put your main code here, to run repeatedly:
  //FIND THE PRESSURE INSIDE THE REACTOR TANK
  //Find pressure in analog Value
  
  float P = analogRead(A2);
  
  //Serial.print("Analog");kl
  //Serial.println(P,4);
  //Find voltage
  float V = P/1024*5 ;
  //Serial.print("Voltage=");
  //Serial.println(V,4);
  //Convert voltage to psi
  psi = P/1024 * 30;
  //Serial.print("psi=");
  //Serial.println(psi,4);
  
  
 
  
    //Find thermistor resistance
   //Serial.print("Analog=");
  //Serial.println((analogRead(sensorPin)));
  float b =analogRead(sensorPin)/float(1024);
  
  
  float resistance = ((10/b) - 10);
  //Serial.print("Thermistor resistance");
  //Serial.println(resistance);
 
 
  //Temperature
  if (resistance>2.085){
  Temp = (0.0003*(pow(resistance,4)) - 0.025*(pow(resistance,3))+0.7629*(pow(resistance,2))-11.246*(resistance)+82.423);
  //Temp = 50;
  //Serial.print("T");
  //Serial.println(Temp);
  }
  else{
  Temp = -33.45*log(resistance)+ 87.956;
  //Temp = 50;
  //Serial.print("T");
  //Serial.println(Temp); 
  }
while (analogRead(A1) < 5){
 digitalWrite(enablePin,LOW);
  lcd.clear();
  lcd.setFontSize(FONT_SIZE_MEDIUM);
   lcd.println("Start?");
   delay(1000);
}


while (analogRead(A1) >40){
  digitalWrite(enablePin,HIGH);
    lcd.clear();
  lcd.setFontSize(FONT_SIZE_MEDIUM);
   lcd.println("Flushing..");
   delay(1000);
}



  if (offstate == 1){
    digitalWrite(enablePin, LOW);
    //Serial.print("offliao ");
    state = "offliao";
    prints();
  }
  
  if (psi <= 4 && Temp <100 && offstate == 0){
    timer2=0;
    if (timer == 0){
    starttime1 = millis();
    timer = 1;
    }
    if (timer <= 100){
      digitalWrite(enablePin, HIGH);
      digitalWrite(purgePin, LOW);
      //Serial.print("stateon1 ");
      state = "stateon1";
      prints();
      timer = millis()-starttime1 + 1;
    }
    if (timer >100 && timer <= 2500){
      digitalWrite(enablePin, LOW);
      digitalWrite(purgePin, LOW);
      //Serial.print("stateoff1 ");
      state="stateoff1";
      prints();
      timer = millis()-starttime1 + 1;
    }
    if (timer >2500){
      timer = 0;
    }
    
    
  }
  
  if (psi > 4){
    rxnstart = 1;
  }
 
  if (psi <= 10 && psi > 4 && Temp <100 && offstate == 0){
    long unsigned starttime = millis();
    timer = 0;
    timer3 = 0;
    if (timer2 == 0){
    starttime2 = millis();
    timer2 = 1;
    }
    if (timer2 <= 50){
      digitalWrite(enablePin, HIGH);
      digitalWrite(purgePin, LOW);
      //Serial.print("stateon2 ");
      state="stateon2";
      prints();
     timer2 = millis()-starttime2 + 1; 
    }
    if (timer2 >50 && timer2 <= 2500){
      digitalWrite(enablePin, LOW);
      digitalWrite(purgePin, LOW);
      //Serial.print("stateoff2 ");
      state="stateoff2";
      prints();
      timer2 = millis()-starttime2 +1;
    }
    if (timer2 >2500){
      timer2 = 0;
    }
  }
  
  
  if (psi > 10 || Temp > 100 ){
    timer =0;
    timer2 =0;
   digitalWrite(purgePin, HIGH);
   digitalWrite(enablePin, LOW);
   state="purging";
    //Serial.print("purging");
    prints();
  }
  
 // if (psi <= 4 && offstate == 0 && rxnstart == 1 && Temp < 100){ 
    //if (timer3 == 0){
    //starttime = millis();
    //timer3 = 1;
    //}
    //if (timer3 <= 10000 && timer3 > 0){
      //timer3 = millis() - starttime + 1;
      //Serial.print("pendingstop");
      //state="pendingstop";
    //}
    //if (timer3 > 10000){
      //offstate = 1;
    //}
  }
  
  

  
     
void prints (){
   if (timer4 ==0){
    starttime3 =millis();
   timer4 =1; 
   }
   if (timer4 <= 1000){
    timer4 = millis() - starttime3 +1; 
   }
   if (timer4 > 1000){
        Serial.print(state);
        Serial.print("\t");
        Serial.print(millis());
        Serial.print("\t");
        Serial.print("psi=");
        Serial.print(psi,4);
        Serial.print("\t");
        Serial.print("T=");
        Serial.println(Temp);
        
        lcd.clear();
        lcd.setFontSize(FONT_SIZE_MEDIUM);
	lcd.print("PSI= ");
        lcd.println(psi,4);
        lcd.print("Temp= ");
        lcd.println(Temp);
        lcd.println(analogRead(A1));
      
                //Serial.print(timer);
           //Serial.print("\t");
        //Serial.print(timer2);
           //Serial.print("\t");
        //Serial.print(timer3);
       //Serial.print(offstate);

        timer4 =0;
   }
           
     }
