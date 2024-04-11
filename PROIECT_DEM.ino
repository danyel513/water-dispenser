#include <LiquidCrystal.h>
#include <math.h>

#define ECHO_SENS1 A2  // iesire semnal senzor ultrasonic
#define TRIG_SENS1 A3  // intrare semnal senzor ultrasonic
#define ECHO_SENS2 A5  //iesire semnal senzor ultrasonic
#define TRIG_SENS2 A4  // intrare semnal senzor ultrasonic
#define W_LED_R 3 // led de functionare a aparatului
#define W_LED_G 5 // led de functinare pompa activa
#define W_LED_B 6 // led albastru
#define BUZZ 4 // pin pt buzzer
#define GLASS_ECHO_SENS A0 // iesire senzor pahar
#define GLASS_TRIG_SENS A1 //intrare senzor pahar
#define PUMP 2 // pin pompa apa
 
LiquidCrystal lcd(7, 8 , 9, 10, 11, 12);//pentru conectarea ecranului LCD

// UNITATE DE MASURA: centimetrii
// EROARE SENZOR: 3mm = 0.3cm

long duration; // citire sezor 
float distance; // distanta de la senzor la nivelul apei
float distance2; // dinstata de la sensorul cand se umple pajarul
int volume; // volum actual de apa in ml si vol anterior

const int min_dist = 3; // distanta minima pana cand e umplut paharul
const int dist_glass = 7; // distanta de activare cand e pahararul pus in zona de umplere
const int min_water = 3; // minimul de apa la care se activa rezervor gol
const int bottle_height = 23; //cm inaltimea sticlei
const int bottle_width = 7.50; //cm latura sticlei (patrat)
const float sens_dist = 2; // cm distanta la care e amplasat senzorul
const float sens_error =  0.3; // cm, eroarea senzorului
float water_lvl; //cm de apa din sticla

void setup() 
{
  pinMode(TRIG_SENS1, OUTPUT);
  pinMode(ECHO_SENS1, INPUT);
  pinMode(TRIG_SENS2, OUTPUT);
  pinMode(ECHO_SENS2, INPUT);        //-> ASIGNARE PINURI
  pinMode(W_LED_R, OUTPUT);
  pinMode(W_LED_G, OUTPUT);
  pinMode(W_LED_B, OUTPUT);
  pinMode(BUZZ, OUTPUT);
  pinMode(GLASS_TRIG_SENS, OUTPUT);
  pinMode(GLASS_ECHO_SENS, INPUT);
  pinMode(PUMP,OUTPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);     // APRINDERE ECRAN
  lcd.print("CANTITATE APA: "); // AFISARE MESAJ INITIAL
}

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH); // lasam senzorul sa citeasca distanta 15 ms
  delayMicroseconds(15);
  digitalWrite(triggerPin, LOW);

  return pulseIn(echoPin, HIGH);// returnam distanta masurata
}

void loop() {
  
  duration = readUltrasonicDistance(TRIG_SENS1, ECHO_SENS1); // citim senzorii
  distance = duration / 58; // calculam distanta
  water_lvl = bottle_height - (distance - sens_dist); // cantitate actuala de apa
  volume = bottle_width * bottle_width * water_lvl ;// volumul de apa in ml (convertire din cm in ml)
  // volumul = aria * inaltime (cm cub = ml)
  // aria = l * l;
  
  // verificare nivel de apa

  if(water_lvl <= min_water)
  {
    lcd.setCursor(0,1);
    lcd.print("  REZERVOR GOL   ");
    analogWrite(W_LED_R, LOW); //oprire LED ROSU
    tone(BUZZ, 262, 150);
    delay(2000);
    noTone(BUZZ);
  }
  else if (water_lvl < bottle_height - sens_error)
    {
      analogWrite(W_LED_R, HIGH); //PORNIRE LED ROSU
      lcd.setCursor(0,1);
      lcd.print("    ");
      lcd.print(volume);
      lcd.print("  ml     ");
    }
    else
    {
      lcd.setCursor(0,1);
      lcd.print("  REZERVOR PLIN");
      analogWrite(W_LED_R, HIGH); //PORNIRE LED ROSU
      
    }
  delay(100);  // mini delay pentru sa nu calculeze brusc

  duration = readUltrasonicDistance(GLASS_TRIG_SENS, GLASS_ECHO_SENS); // citim senzorul
  distance = duration / 58;

  duration = readUltrasonicDistance(TRIG_SENS2, ECHO_SENS2);
  distance2 = duration / 58;


  while ((distance <= dist_glass)  && (water_lvl > sens_error + min_water) &&  (distance2 > min_dist))
  {
    analogWrite(W_LED_R, LOW); // oprire led rosu
    lcd.setCursor(0,0);
    lcd.print("   ALIMENTARE   ");
    lcd.setCursor(0,1);
    lcd.print("    RECIPIENT       ");
    analogWrite(W_LED_G, HIGH); // LED VERDE FUNCTIONARE POMPA
    delay(500);
    digitalWrite(PUMP, HIGH); // pornim pompa
    delay(150);
    duration = readUltrasonicDistance(TRIG_SENS1, ECHO_SENS1);  // citim senzorii
    distance = duration / 58; // calculam distanta
    water_lvl = bottle_height - (distance - sens_dist); // cantitate actuala de apa
    
    duration = readUltrasonicDistance(GLASS_TRIG_SENS, GLASS_ECHO_SENS); // citim sensorul
    distance = duration / 58;

    duration = readUltrasonicDistance(TRIG_SENS2, ECHO_SENS2);
    distance2 = duration / 58;

  }
  digitalWrite(PUMP, LOW); // oprim pompa
  delay(100);
  analogWrite(W_LED_G, LOW); //oprim led 
  analogWrite(W_LED_R, HIGH); // PORNIM LED ROSU - APARAT FUNCTIONAL
  lcd.setCursor(0,0);
  
  delay(50);
  lcd.print(" CANTITATE APA: "); // resetam ecranul

}
