//Based on YWROBOT's LiquidCrystal_I2C library, Library version:1.1
//Also based on the Debounce.ino sketch that comes with Arduino IDE

// Turbidity sensor A3
// Surrounding temperature A5
// pH sensor A0
//Ultrasonic sensor (trigger D3, echo D4)
// Relay D7
//
//
#include <LiquidCrystal_I2C.h> 
#include <SoftwareSerial.h>
#include <String.h>
#include <DHT.h> 
 #include <EEPROM.h>
#include <DallasTemperature.h>

#include <Wire.h>
 #define ONE_WIRE_BUS A1
#define DHTPIN 25
OneWire oneWire(ONE_WIRE_BUS);
DHT dht(DHTPIN, DHT11);
LiquidCrystal_I2C lcd(0x27, 20, 4);
DallasTemperature sensors(&oneWire);

SoftwareSerial gprs(2,3);
float ph_act, turbidity;
long duration, inches;
int set_val, percentage;
bool state, pump;
int phval = 0; // the pH meter Analog output is connected with the Arduino’s A0
unsigned long int avgval; //Store the average value of the sensor feedback
int buffer_arr[10], temp;
float calibration_value = 22.8;
#define sensor_pin A3
int read_ADC;
int ntu;
int led1 = 8;
int led2 = 9;
int led3 = 10;
int led4 = 11;
int led5 = 12;
int buzzer = 2;

int trigpin= 3;
int echopin =4;
int sw1 = 5;
int sw2 = 6;
int waterpump=7;

int WhichScreen =1;   // This variable stores the current Screen number
boolean hasChanged = true;
const int buttonPin = 13;    // the number of the pushbutton pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 10;    // the debounce time; increase if the output flickers

void setup()
{
              // the GPRS baud rate   
  Serial.begin(9600);    // the GPRS baud rate 
  gprs.begin(9600);
    sensors.begin();
  dht.begin();
  delay(1000);
    lcd.begin();// or lcd.begin();
  lcd.backlight();
   pinMode(buttonPin, INPUT);
   pinMode(sensor_pin, INPUT);

   pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  pinMode(buzzer, OUTPUT);

  
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  digitalWrite(led3, LOW);
  digitalWrite(led4, LOW);
  digitalWrite(led5, LOW);
  digitalWrite(buzzer, LOW);

 pinMode(trigpin, OUTPUT);
pinMode(echopin, INPUT);
  pinMode(waterpump, OUTPUT);
    pinMode(sw1, INPUT_PULLUP);
  pinMode(sw2, INPUT_PULLUP);

  

    set_val = EEPROM.read(0);
  if (set_val > 30)set_val = 35.2 ; 
}
void loop()
{     long duration, distance;
       sensors.requestTemperatures();
    digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  duration = pulseIn(echopin, HIGH);
  
  distance = (duration/2) / 29.1;
  percentage=(34-distance)*100/29;
  
  read_ADC = analogRead(sensor_pin);


        lcd.setCursor(0, 0);
    lcd.print("WATER LEVEL:");
  lcd.setCursor(0, 1);
  lcd.print("PUMP:OFF MANUAL ");
  lcd.setCursor(13, 0);
  if (percentage < 0)percentage = 0;
  lcd.print(percentage);  
  lcd.print("%   ");
     lcd.setCursor(5, 1);
   if(pump==1)lcd.print("ON ");
   else if(pump==0) lcd.print("OFF");
   

   lcd.setCursor(9, 1);
   if(!digitalRead(6))lcd.print("(MANUAL) "); 
   else lcd.print("(AUTO)   ");  
  lcd.setCursor(0, 3);
  lcd.print("TEMP:");
  lcd.setCursor(5, 3);
  lcd.print(sensors.getTempCByIndex(0));
  lcd.setCursor(9, 3);
  lcd.print("*C");
   

   if(read_ADC>208)read_ADC=208;

    ntu = map(read_ADC, 0, 208, 300, 0); 
 
    lcd.setCursor(0,2);
    lcd.print("TUR:");
    lcd.setCursor(4,2);
    lcd.print(ntu);
    lcd.print("  ");

    lcd.setCursor(12,2);//set cursor (column by row) indexing from 0
if(ntu<10)            lcd.print("(Clear)");
if(ntu>=10 && ntu<30) lcd.print("(Cloudy)");
if(ntu>=30)           lcd.print("(Dirty)");
    


     for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  {
    buffer_arr[i] = analogRead(A0);
    delay(30); 
  }
  for (int i = 0; i < 9; i++) //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++) //take the average value of 6 center sample
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1024 / 6; //convert the analog into millivolt
  float ph_act = -5.65 * volt + calibration_value;//convert the millivolt into pH value

    lcd.setCursor(13, 3);
  lcd.print("pH:");
  lcd.setCursor(16, 3);
  lcd.print(ph_act);


  lcd.setCursor(15, 3);
  if (percentage < 30 & digitalRead(6))pump = 1;
  if (percentage > 99)pump = 0;
  digitalWrite(7, !pump);

 if (  percentage < 10 )
{
       digitalWrite(buzzer, HIGH);

} else
 if ( percentage > 10 )
    {
    
     digitalWrite(buzzer, LOW);
     
      }
      
  if (  (percentage > 0) && (percentage <= 24)  )
  {

    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);
  
  } else



    
    if (  (percentage > 25) && (percentage <= 49)  )
    {

      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
    
    } else

      if (  (percentage > 50) && (percentage <= 74)  )
      {

        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, HIGH);
        digitalWrite(led4, LOW);
        digitalWrite(led5, LOW);
      } else

        if (  (percentage > 75) && (percentage <= 89)  )
        {

          digitalWrite(led1, LOW);
          digitalWrite(led2, LOW);
          digitalWrite(led3, LOW);
          digitalWrite(led4, HIGH);
          digitalWrite(led5, LOW);
          
        } else

          if (  (percentage > 90) && (percentage <= 100)  )
          {

            digitalWrite(led1, LOW);
            digitalWrite(led2, LOW);
            digitalWrite(led3, LOW);
            digitalWrite(led4, LOW);
            digitalWrite(led5, HIGH);
            
          }
  
  
   lcd.setCursor(5, 1);
   if(pump==1)lcd.print("");
   else if(pump==0) lcd.print("");

  lcd.setCursor(9, 1);
  if (!digitalRead(6))lcd.print("");
  lcd.print("");

  if (!digitalRead(5) & !state & digitalRead(6)) {
    state = 1;
    set_val = inches;
    EEPROM.write(0, set_val);
  }

  if (!digitalRead(5) & !state & !digitalRead(6)) {
    state = 1;
    
    pump = !pump;

  }

  if (digitalRead(5))state = 0;


 
// push button control line
  if (hasChanged == true) {
    
  switch(WhichScreen) {
    case 1:
    {
      firstScreen();
    }
      delay(2000);

    
    case 2:
      {
        secondScreen();
        }
      break;
    
    case 4:
      {
        fourthScreen();
      }
      break;
    
   case 5:
      {
        fifthScreen();
      }
      break;
      
    }
}

    //-------------------------------
    // BEGIN of the switch debouncing code
    int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        hasChanged = true;
        WhichScreen++;
        
        
      }
    } else {
      hasChanged = false;
    }
  }
  lastButtonState = reading;
  // END of the switch Debouncing code
  // --------------------------------------
  if (WhichScreen > 5){
    WhichScreen = 1;
  }
}







void firstScreen()
  {
     

  // Analog input for ultraasonic sensor water level
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  duration = pulseIn(echopin, HIGH);

  //water level and water pump line of code
  lcd.clear();
  lcd.print("WATER LEVEL:");
  lcd.setCursor(0, 1);
  lcd.print("PUMP:OFF MANUAL ");
  lcd.setCursor(12, 0);
  if (percentage < 0)percentage = 0;
  lcd.print(percentage);
  lcd.print("%   ");


  if (percentage < 30 & digitalRead(6))pump = 1;
  if (percentage > 99)pump = 0;
  digitalWrite(7, !pump);

  
if (  (percentage > 0) )
  {
    digitalWrite(buzzer, LOW);
  }

  if (  (percentage > 0) && (percentage <= 10)  )
  {

    digitalWrite(led1, HIGH);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);
    delay(50);
    digitalWrite(buzzer, HIGH);
  } else

    if (  (percentage > 24) && (percentage <= 49)  )
    {

      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led5, LOW);
      delay(50);
    } else

      if (  (percentage > 49) && (percentage <= 74)  )
      {

        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, HIGH);
        digitalWrite(led4, LOW);
        digitalWrite(led5, LOW);
        delay(50);
      } else

        if (  (percentage > 74) && (percentage <= 99)  )
        {

          digitalWrite(led1, LOW);
          digitalWrite(led2, LOW);
          digitalWrite(led3, LOW);
          digitalWrite(led4, HIGH);
          digitalWrite(led5, HIGH);
          delay(50);
        } else

          if (  (percentage > 99) && (percentage <= 111)  )
          {

            digitalWrite(led1, LOW);
            digitalWrite(led2, LOW);
            digitalWrite(led3, LOW);
            digitalWrite(led4, LOW);
            digitalWrite(led5, HIGH);

            delay(50);
          }

   lcd.clear();
  lcd.print("WATER LEVEL:");
  lcd.setCursor(0, 1);
  lcd.print("PUMP:OFF MANUAL ");
  lcd.setCursor(13, 0);
  if (percentage < 0)percentage = 0;
  lcd.print(percentage);
  lcd.print("%   ");
       

  if (percentage < 30 & digitalRead(6))pump = 1;
  if (percentage > 98)pump = 0;
  digitalWrite(7, !pump);
  
   lcd.setCursor(5, 1);
   if(pump==1)lcd.print("ON "); 
   else if(pump==0) lcd.print("OFF");
   

   lcd.setCursor(9, 1);
   if(!digitalRead(6))lcd.print("MANUAL "); 
   else lcd.print("AUTO   ");

  if (!digitalRead(5) & !state & digitalRead(6)) {
    state = 1;
    set_val = inches;
    EEPROM.write(0, set_val);
  }

  if (!digitalRead(5) & !state & !digitalRead(6)) {
    state = 1;
    pump = !pump;
 
  }

  if (digitalRead(5))state = 0;

  }
  long microsecondsToInches(long microseconds) {
   return microseconds / 29 / 2;
}

void secondScreen(){
 
     sensors.requestTemperatures();
      // water temperature line of code

  }
void fourthScreen()
  {
     //pH sensor

   for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  {
    buffer_arr[i] = analogRead(A0);
    delay(30); 
  }
  for (int i = 0; i < 9; i++) //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++) //take the average value of 6 center sample
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1024 / 6; //convert the analog into millivolt
  float ph_act = -5.65 * volt + calibration_value; //convert the millivolt into pH value

  lcd.print("TURBIDITY:");
  lcd.print("   ");
  lcd.setCursor(10, 3);
  lcd.print(turbidity);

  }
void fifthScreen()
  {
    // Analog input for turbidityint sensorValue = analogRead(sensorPin);
read_ADC = analogRead(sensor_pin);
if(read_ADC>208)read_ADC=208;

ntu = map(read_ADC, 0, 208, 300, 0);
 
  }


void watersensor(){
 
  sensors.requestTemperatures();

  // Analog input for ultraasonic sensor water level
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  duration = pulseIn(echopin, HIGH);



  // Analog input for turbidityint sensorValue = analogRead(sensorPin);

if(read_ADC>208)read_ADC=208;

ntu = map(read_ADC, 0, 208, 300, 0); 
 
  delay(1000);


  // surrounding temperature sensor line of code
  int reading = analogRead(A5);
  float voltage = reading * 5.0;
  voltage /= 1024.0;
  

  // now print out the surrounding temperature
  float temperatureC = (voltage - 0.5) * 100 ;//converting from 10 mv per degree wit 500 mV offset
  //to degrees ((voltage - 500mV) times 100)
 


  //pH sensor line of code
  for (int i = 0; i < 10; i++) //Get 10 sample value from the sensor for smooth the value
  {
    buffer_arr[i] = analogRead(A0);
    delay(30); 
  }
  for (int i = 0; i < 9; i++) //sort the analog from small to large
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++) //take the average value of 6 center sample
    avgval += buffer_arr[i];
  float volt = (float)avgval * 5.0 / 1024 / 6; //convert the analog into millivolt
  float ph_act = -5.70 * volt + calibration_value; //convert the millivolt into pH value



}
