/* Ethylometer for PRismino
  by Patrick Bobbink
  for Robopoly
  last review 19/05/2014
  http://robopoly.epfl.ch
*/

#include "prismino.h"

void led_clock();
void print_high_score();


#define LED_TIME_ON 200// time between a on/off switch for the leds in order not to blind people. us.
#define LED_TIME_OFF 6000
#define FAKE_HIGH 15 // Fake high score (15th led)

#define RED_LED 10
#define ORANGE_LED 9
#define GREEN_LED 8

// Limits to check if IR reads anything.
#define IR_STRAW A5
#define STRAW 500
#define NO_STRAW 350

// Time during which the result of the test will be shown.
#define ALCOHOL_SENSOR A4
#define RESULT_SHOW_TIME 10000
#define ALCOHOL_VALUE 900
#define MEASURES_AMOUNT 100
#define MIN_ALCOHOL 400
#define LOW_ALCOHOL 200
#define HIGH_ALCOHOL 1000


unsigned char i = 0;
unsigned int straw = 0;
unsigned long alcohol = 0;
unsigned int high_score =  FAKE_HIGH;
unsigned int score = 0;
unsigned int empty_sensor = 0;
int k;

int debug = 1;

// pins for the TLC5925I
#define LE 3
#define OE 4
#define SDI 5
#define SDO 6
#define CLK 7




void setup()
{
  if(debug == 1)
    Serial.begin(9600);

    
  pinMode(13,OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(ORANGE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  pinMode(CLK, OUTPUT); 
  pinMode(SDI, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(LE, OUTPUT);
  empty_sensor = analogRead(ALCOHOL_SENSOR);

 digitalWrite(OE, LOW);
 digitalWrite(LE, HIGH);
}



void loop()
{
/*  if(debug == 1)
  {
    Serial.println("alcohol");
    Serial.print(analogRead(ALCOHOL_SENSOR));
    Serial.println("straw");
    Serial.print(analogRead(IR_STRAW));
  }
  */
  print_high_score();
  alcohol = analogRead(ALCOHOL_SENSOR);
  // as long as the sensor is reading more than MIN_ALCOHOL we wait
  while(alcohol > empty_sensor + 100)
  {
    alcohol = analogRead(ALCOHOL_SENSOR);
    print_high_score(); //printing high score as long as the sensor is not available.
  }
  
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  
  // Waiting for a straw to be inserted
  straw = analogRead(IR_STRAW);
  while(straw < NO_STRAW)
  {
    straw = analogRead(IR_STRAW);
    print_high_score();
  } 
   
  if(debug == 1)
  {
    Serial.print("Found a straw ");
    Serial.println(straw);
  }
  digitalWrite(OE, HIGH); // turning the leds off
  // Straw has been inserted, waiting 1 sec before we read the sensor
  delay(1000);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(ORANGE_LED, HIGH);
  delay(1000);

  alcohol = 0;
  for(i = 0; i < MEASURES_AMOUNT; i++)
  {
    alcohol += analogRead(ALCOHOL_SENSOR);
    delay(5);
  }

  if(debug == 1)
  {
    Serial.print("alcohol sum on ");
    Serial.print(MEASURES_AMOUNT);
    Serial.print(" readings ");
    Serial.println(alcohol);
  }

  // measures have been made, turning orange led off, red on
  digitalWrite(ORANGE_LED, LOW);
  digitalWrite(RED_LED, HIGH);
  
  // taking the mean value
  alcohol /= MEASURES_AMOUNT;
  //alcohol -= empty_sensor;
  if(alcohol >= HIGH_ALCOHOL)
    alcohol = HIGH_ALCOHOL;
  if(alcohol <= LOW_ALCOHOL)
    alcohol = LOW_ALCOHOL;

  if(debug == 1)
  {
    Serial.print("Found average alcohol ");
    Serial.println(alcohol);
  }
  // normalize the value over 32 leds
  score = (int)(32*(double)((alcohol - LOW_ALCOHOL))/(HIGH_ALCOHOL - LOW_ALCOHOL));

  if(debug == 1)
  {
    Serial.print("score   ");
    Serial.println(score);
  }
  if(score > high_score)
    high_score = score; // changing high score if needed.
    
  for(k = 0; k < 10000; k++)
  {
    digitalWrite(OE,HIGH);
    for(i = 0; i < 32; i++)
    {
      if(i <= score || i == high_score)
        digitalWrite(SDI, HIGH);
      else
        digitalWrite(SDI, LOW);
       
      led_clock(); 
        
    }
    digitalWrite(OE, LOW);
  } 
}
      



void print_high_score()
{
  digitalWrite(OE,HIGH);
  delayMicroseconds(LED_TIME_OFF);
  for(i = 0; i < 32; i++)
  {
    if(i == high_score)
      digitalWrite(SDI, HIGH);
    else
      digitalWrite(SDI,LOW);

    led_clock();
  }
  digitalWrite(OE, LOW);
  delayMicroseconds(LED_TIME_ON);
}




void led_clock()
{
     // Letting clock tick
    digitalWrite(CLK, HIGH); //noping in case the compiler would screw up, this way the clock ticks and is taken into account by the multiplex
    delayMicroseconds(10);
    digitalWrite(CLK, LOW);
    delayMicroseconds(10);
}

