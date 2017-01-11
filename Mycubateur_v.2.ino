
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x22              // Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN     3

LiquidCrystal_I2C  lcd(I2C_ADDR,2,1,0,4,5,6,7);

#include <DHT.h>
#define DHTPIN            A1          // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11       // Uncomment the type of sensor in use:
DHT dht(DHTPIN, DHTTYPE);

#define backlight 2
#define heater 4 
#define humidifier 5

int tempCible = 24;
int lastTempCible = 24;
int heaterHysteresis = 2;
int humidexCible = 75;
int lastHumidexCible = 75;
int humidifierHysteresis = 6;

int greenhouseTemperature;
int lastGreenhouseTemperature;
int greenhouseHumidity;
int lastGreenhouseHumidity;

int heaterState=0;
int lastHeaterState=0;
int humidifierState=0;
int lastHumidifierState=0;

int variable = 0;
//buttonstate
byte buttonState = 0;
byte lastButtonState = 0;

const int ON = LOW;
const int OFF = HIGH;

void setup() {
  
 pinMode(humidifier,OUTPUT);
 pinMode(heater,OUTPUT);
 digitalWrite(humidifier, OFF);
 digitalWrite(heater, OFF);
 
 Serial.begin(9600);
 dht.begin();
 pinMode(backlight, INPUT_PULLUP);
 lcd.begin(20, 4);
 lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
 lcd.setBacklight(HIGH);
 
 greenhouseTemperature = dht.readTemperature();
 greenhouseHumidity = dht.readHumidity();
 lcd.home();
 lcd.print("*****MYCUBATEUR*****");
 lcd.setCursor(0,1);
 lcd.print("Temp.: ");
 lcd.print(greenhouseTemperature);
 lcd.print("C");
 lcd.setCursor(15,1);
 lcd.print("(");
 lcd.print(tempCible);
 lcd.print(")");
 lcd.setCursor(0,2);
 lcd.print("Humidex: ");
 lcd.print(greenhouseHumidity);
 lcd.print("%");
 lcd.setCursor(15,2);
 lcd.print("(");
 lcd.print(humidexCible);
 lcd.print(")");
 lcd.setCursor(0,3);
 lcd.print("HEAT:");
 if(heaterState == 1){lcd.print(" ON");}
 else{lcd.print(" OFF");}
 lcd.setCursor(10,3);
 lcd.print("| HUM:");
 if(humidifierState == 1){lcd.print(" ON");}
 else{lcd.print(" OFF");}

}

void loop() {

if(digitalRead(backlight) == HIGH){lcd.setBacklight(HIGH);}
else{lcd.setBacklight(LOW);}

checkChanges();
greenhouseTemperature = dht.readTemperature();
greenhouseHumidity = dht.readHumidity();

if(greenhouseHumidity > (humidexCible + humidifierHysteresis/2)){
  digitalWrite(humidifier, OFF);
  humidifierState = 0;}
else if(greenhouseHumidity < (humidexCible - humidifierHysteresis/2)){
  digitalWrite(humidifier, ON);
  humidifierState = 1;}
  
if(greenhouseTemperature > (tempCible + heaterHysteresis/2)){
  digitalWrite(heater, OFF);
  heaterState = 0;
}
else if(greenhouseTemperature < (tempCible - heaterHysteresis/2)){
  digitalWrite(heater, ON);
  heaterState = 1;
}

  
if(greenhouseTemperature != lastGreenhouseTemperature){
  lcd.noBlink();
  lcd.setCursor(0,1);
  lcd.print("Temp.: ");
  lcd.print(greenhouseTemperature);
  lcd.print("C");
}
if(tempCible != lastTempCible){
  lcd.noBlink();
  lcd.setCursor(15,1);
  lcd.print("(");
  lcd.print(tempCible);
  lcd.print(")");
}
if(greenhouseHumidity != lastGreenhouseHumidity){  
  lcd.noBlink();
  lcd.setCursor(0,2);
  lcd.print("Humidex: ");
  lcd.print(greenhouseHumidity);
  lcd.print("%");
}
if(humidexCible != lastHumidexCible){
  lcd.noBlink();
  lcd.setCursor(15,2);
  lcd.print("(");
  lcd.print(humidexCible);
  lcd.print(")");
}
if (heaterState != lastHeaterState){
  lcd.noBlink();
  lcd.setCursor(0,3);
  lcd.print("HEAT:");
  if(heaterState == 1){lcd.print(" ON ");}
  else{lcd.print(" OFF");}
}
if (humidifierState != lastHumidifierState){
  lcd.noBlink();
  lcd.setCursor(10,3);
  lcd.print("| HUM:");
  if(humidifierState == 1){lcd.print(" ON ");}
  else{lcd.print(" OFF");}
}

if(variable > 1){variable = 0;}

lcd.blink();
switch(variable){
    case 0:lcd.setCursor(19,1);break;
    case 1:lcd.setCursor(19,2);break;
}

lastGreenhouseTemperature = greenhouseTemperature;
lastTempCible = tempCible;
lastGreenhouseHumidity = greenhouseHumidity;
lastHumidexCible = humidexCible;
lastHeaterState = heaterState;
lastHumidifierState = humidifierState;

delay(50);
}

void checkChanges(){
  int a = analogRead(0);
  int currentMenuItem;
  checkButtonState(a);
  switch(variable){
    case 0:currentMenuItem = tempCible;break;
    case 1:currentMenuItem = humidexCible;break;
  }
  
  if ((buttonState == 1) && (buttonState != lastButtonState)){
      currentMenuItem -= 1;
      switch(variable){
        case 0:tempCible = currentMenuItem;break;
        case 1:humidexCible = currentMenuItem;break;
        }
  } else if ((buttonState == 2) && (buttonState != lastButtonState)){
      currentMenuItem += 1;
      switch(variable){
        case 0:tempCible = currentMenuItem;break;
        case 1:humidexCible = currentMenuItem;break;
        }
  } else if ((buttonState == 3) && (buttonState != lastButtonState)){
      variable += 1;   
      if(variable > 1){variable = 0;}
  }

  lastButtonState = buttonState;
}


void checkButtonState(int x){
  if(x < 800){buttonState = 0;} 
  else if(x < 900){buttonState = 1;} 
  else if (x < 950){buttonState = 2;}
  else if (x < 1000){buttonState = 3;}
}

