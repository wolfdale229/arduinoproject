#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "max6675.h"
#include <SPI.h> //library for the sdcard
#include <SD.h>//library for the sdcard

File plcfile;
/********  NOTE   *******/
/**
   # The delay time of all sensors in this project is 3000 sec , whis is equlvalent to 3 seconds.
   except the delay when the temperature is at the max before the outlet Value is actuated
   # All variables are of global scope , hence they should not be renamed
   # All the libraries should be used for the project to work
   # For the SD card , connect the following pins accordingly
     Name   MEGA     UNO
     MOSI = pin 51	13
     MISO = pin 50	12
     CLK = pin 52	11
     CS = pin 4 , 53	10,4
*/
/************** Relay pin ***************/
int heater = 11; // handles the turning ON and OFF of the relay which powers the heater

/****************Thermocouple variables and pin number ************/
int thermoDO = 7;
int thermoCS = 8;
int thermoCLK = 10;

MAX6675 thermocouple (thermoCLK, thermoCS, thermoDO);
/*********** range finder variables**************/
const int trigPin = 12;// trig of the ultrasonic sensor
const int echoPin = 13;//echopin of the ultrasonic sensor
int tankheight = 40 ; // lenght of the tank in centimeters
int Waterlevel;
int minimum_level = 5 ; // minimum level of water
int maximum_level = 35; // maximum level of water
const float max_temp = 100.00; // this is a cinstant value
float tempvalue ; //The value of this variable is the value gotten from the the ktc.readcelsius() of the max6675 thermocouple

/************ solenoid valve pin *************/
int input_valve = 6; // this handle sthe input valve
int output_valve = 2; // this pin handles the output valve

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // SET I2C Address

void setup() {
  lcd.begin(16, 2); // Initialinze LCD.
  lcd.backlight(); // makes Backligh ON.
  lcd.clear();     // Clears LCD
  lcd.home();      // Sets cursor at Column 0 and Line 0
  Serial.println("Omefe Chika!");
  lcd.print("Omefe Chika!");
  lcd.setCursor(0, 1); // Sets cursor at Column 0 and Line 1
  Serial.println("admission number");
  lcd.print("admission number");
  delay(2000);
  lcd.clear();
  pinMode(input_valve , OUTPUT);
  pinMode(output_valve, OUTPUT);
   pinMode(heater, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Starts the serial communication

  /*while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.print("Setting Up SDcard.");
    delay(500);

    if (!SD.begin(53)) {
    lcd.println("SD Card Error!");
    delay(1000);
    return;
    }
    lcd.println("Card SetUp done.");
    delay(1000);*/

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  digitalWrite(input_valve, LOW);
  digitalWrite(output_valve, LOW);

}
void loop() {
  Display();

  do {
    digitalWrite(input_valve, HIGH);//if the water level is at it's minimum depth turn ON the input valve
  }
  while (Waterlevel != maximum_level && Waterlevel <= minimum_level/*stated minimum level*/);
    // The while loop here would make sure that both the input_valve and output_valve would not trigger until the water has boiled up to the required temperature
    if (tempvalue < max_temp ){
      digitalWrite(heater , HIGH); // if the temperature is still lower than the required value keep the heater ON
    //  digitalWrite(input_valve , LOW);
     // digitalWrite(output_valve, LOW);
    }
     if (tempvalue >= max_temp){
      digitalWrite(heater , LOW); // when at the require value stop the heater
    }
    else{
      digitalWrite(heater,HIGH);
    }
    delay(10000); // delay for 10 seconds
  do{
    digitalWrite(output_valve, HIGH) ;//trigger the outlet valve
  }
  while(Waterlevel > minimum_level);
  datafile(); // gathers the relevant data

} // end loop function
/************************ Function Temperature *****************************/
int temperature() {
  // basic readout test
  // basic readout test, just print the current temp
  tempvalue = thermocouple.readCelsius();
  delayMicroseconds(5);
  return tempvalue;

  Serial.print("Temperature : ");
  Serial.print(tempvalue);
  Serial.print("\n");
} // end temperature

/************************** Function Tdatafile ********************************/
void datafile() {
  plcfile = SD.open("plc_data.csv", FILE_WRITE);

  // if the file opened okay, write to it:
  if (plcfile) {
    Serial.println("Writing to Data.csv...");

    plcfile.print("Water level in centimeters = ");
    plcfile.print(Waterlevel);
    plcfile.println();

    plcfile.print("Temperature in degree celsius = ");
    plcfile.print("");
    plcfile.print(tempvalue);
    plcfile.println();
    plcfile.println();

    // close the file:
    plcfile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening Data.csv");
  }
} // end function

/************************* Function levelfinder ********************************/
int levelfinder() {
  long duration;
  int distance;
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;
  Waterlevel =  tankheight - distance;
  return Waterlevel;
  // Prints the distance on the Serial Monitor
  Serial.print("\n");
  Serial.print("Water Level = ");
  Serial.println(Waterlevel);
} //end levelfinder

/*************************** Function display **********************************/
void Display() {
  lcd.begin(16, 2); // Initialize LCD.
  lcd.clear();     // Clears LCD
  lcd.home();      // Sets cursor at Column 0 and Line 0
  lcd.print("Water Level:");
  lcd.print(levelfinder());
  lcd.print("cm");
  lcd.setCursor(0, 1); // Sets cursor at Column 0 and Line 1
  lcd.print("Temperature:");
  lcd.print(temperature());
  lcd.print("C");
  delayMicroseconds(5);
  // lcd.clear();
} // end display
