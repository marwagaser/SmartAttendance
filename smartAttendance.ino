/**************************************************************************/
/*! 
    @file     iso14443a_uid.pde
    @author   Adafruit Industries
  @license  BSD (see license.txt)
 
    This example will attempt to connect to an ISO14443A
    card or tag and retrieve some basic information about it
    that can be used to determine what type of card it is.   
   
    Note that you need the baud rate to be 115200 because we need to print
  out the data and read from the card at the same time!
 
This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
This library works with the Adafruit NFC breakout 
  ----> https://w...content-available-to-author-only...t.com/products/364
 
Check out the links above for our tutorials and wiring diagrams 
These chips use SPI or I2C to communicate.
 
Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!
 
*/
/**************************************************************************/
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
 
#define PN532_SCK  (8)
#define PN532_MOSI (7)
#define PN532_SS   (10)
#define PN532_MISO (13)
#include <LiquidCrystal.h>

 
// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (8)
#define PN532_RESET (7)
 
 
// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:
 
// Use this line for a breakout with a SPI connection:
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
 
// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
//Adafruit_PN532 nfc(PN532_SS);
 
// Or use this line for a breakout or shield with an I2C connection:
//Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
 
//int redLed = 12;
//int greenLed = 11;
int buzzer = 9;
int smokeA0 = A3;
char ch;
int Contrast=15;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// Your threshold value
int sensorThres = 58;
 
 
int finish=0;
int finishedfirstrun=0;
int LCDtotal=0;
 
//TA VARIABLES
int isTAEnter=0;
int TAScanned=0;
long startSession;
long endsession;
 
//Student1 variables
int student1Scanned=0;
int isStudent1Enter=0;
long student1starttime;
long student1endtime;
int student1isearly=0;
 
//Student2 variables
int student2Scanned=0;
int isStudent2Enter=0;
long student2starttime;
long student2endtime;
int student2isearly=0;
//Attendance related variables
int sessiontime=0;
int student1spent=0;
int student2spent=0;
long student1attendance=0;
long student2attendance=0;
 
 int counter=0;
 
String theID= "";


int lcdoutput=A4;

void setup(void) {

  pinMode(smokeA0, INPUT);
  pinMode(A0, OUTPUT);//red
  pinMode(A1, OUTPUT);//green
  pinMode(A2, OUTPUT); //buzzer //missing callin the buzzer
  Serial.begin(9600);
  Serial.begin(115200);
  Serial.println("Hello!");
 
  nfc.begin();
 
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
 
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
 
  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);
 
  // configure board to read RFID tags
  nfc.SAMConfig();
 
  Serial.println("Waiting for an ISO14443A card");
  Serial.begin(9600);
 Serial.println("LCD test with PWM contrast adjustment");
 pinMode(lcdoutput,OUTPUT); 
 lcd.begin(16, 2);
 lcd.print("Smart Attendance");
}
 
void loop(void) {
  //Serial.println(millis());
  digitalWrite(lcdoutput,LOW);
  delay(1000);
 digitalWrite(lcdoutput,HIGH);
 lcd.setCursor(0, 1); //print the number once in 2nd line
 int cx = isStudent1Enter+isStudent2Enter;
 lcd.print(cx);
 
int analogSensor = analogRead(smokeA0);
 
 Serial.print("Pin A0: ");
Serial.println(analogSensor);
if (analogSensor > sensorThres)
  {
//    digitalWrite(redLed, LOW);
//    digitalWrite(greenLed, HIGH);
//    tone(buzzer, 1000);
    digitalWrite(A1,HIGH );
    digitalWrite(A0, LOW);
    digitalWrite(A2, HIGH);
    tone(A2, 1000);
  }
  else
  {
//    digitalWrite(redLed, HIGH);
//    digitalWrite(greenLed, LOW);
//    noTone(buzzer);
    digitalWrite(A1, LOW);
    digitalWrite(A0, HIGH);
    noTone(A2);
  }
 
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
 
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
 
  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      Serial.print(" 0x");Serial.print(uid[i]); 
      theID+=uid[i];
    }
     Serial.println("");      
 
    if(theID.equals("94158187121")){ //scan TA
       theID="";
     //When the TA enters we start session then we check if there are any early students and then we give them the same time as the start session if they were early
     if(TAScanned==0){
     
     TAScanned=TAScanned+1;
     isTAEnter=1;
     startSession=millis();
Serial.println("The TA has entered");
    digitalWrite(A1, HIGH); //yellow
    digitalWrite(A0, LOW); //ted
      delay(2000);
    digitalWrite(A1, LOW); //red
    digitalWrite(A0, HIGH); //yellow
 
     if(student1isearly==1)
     {
      Serial.println("Student 1 who came early has the start time of the TA now");
      student1starttime=startSession;
     }
     if(student2isearly==1)
     {
       Serial.println("Student 2 who came early has the start time of the TA now");
      student2starttime=startSession;
     }
      }
      else
      {//Here we are handling when the TA leaves the class, we will check if there are students that did not leave and if they did not leave we will end their time with the TA, and then we reset
        //all values
        
        endsession=millis();
         Serial.println("The TA has left the class");
    digitalWrite(A1, HIGH);
    digitalWrite(A0, LOW);
      delay(2000);
     digitalWrite(A1, LOW);
     digitalWrite(A0, HIGH);
 
        if(isStudent1Enter==1)
        {
          Serial.println("Student 1 should leave the class in 10 seconds his endtime is the same as the TA now");
        student1endtime=endsession;
        }
        if(isStudent2Enter==1)
        {
         Serial.println("Student 2 should leave the class in 10 seconds his endtime is the same as the TA now");
        student2endtime=endsession;
        }
        digitalWrite(A1, HIGH);
    digitalWrite(A0, LOW);
      delay(10000);
     digitalWrite(A1, LOW);
     digitalWrite(A0, HIGH);

        attendance();
         Serial.println("Attendance has been done a new session can start");
        
       
               
      }
    }
 
//Student 1 starts
 //Student 1 add the ID when we get the card
     if(theID.equals("160226232162"))
     {
      theID="";
      //If student is entering
      if(student1Scanned==0)
      {
       student1Scanned=student1Scanned+1;
      isStudent1Enter=isStudent1Enter+1;
 
      //if TA has already entered before the student
      if(isTAEnter==1)
      {
        student1starttime=millis();
        Serial.println("Student 1 has entered the class (after the TA)");
      digitalWrite(A1, HIGH);
      digitalWrite(A0, LOW);
      delay(2000);
     digitalWrite(A1, LOW);
     digitalWrite(A0, HIGH);
 
      }
      //if the TA did not enter and student arrived to class early
      else
      {
        Serial.println("Student 1 has entered the class (before the TA)");
        student1isearly=1;
        digitalWrite(A1, HIGH);
        digitalWrite(A0, LOW);
        delay(2000);
        digitalWrite(A1, LOW);
        digitalWrite(A0, HIGH);
        
      }
     }
     //If student is leaving
     else
     {
      theID="";
      isStudent1Enter=0;
      student1Scanned=0;
      student1isearly=0;
      if(isTAEnter=1)
      {
        // Serial.println("Student 1 is leaving the class before TA (TA is still in the class)");
        student1endtime=millis();
      }
        if(isTAEnter==0)
        {
          //Serial.println("Student 2 has left the class after TA left (TA is not in the class)");
        }
        digitalWrite(A1, HIGH);
        digitalWrite(A0, LOW);
        delay(2000);
         digitalWrite(A1, LOW);
         digitalWrite(A0, HIGH);
 
     }
 
     }
 
     //Student1 ends
 
      //Student2 starts
      if(!theID.equals(""))
      {
      theID="";
      //If student is entering
      if(student2Scanned==0)
      {
       student2Scanned=student2Scanned+1;
      isStudent2Enter=isStudent2Enter+1;
      //if TA has already entered before the student
      if(isTAEnter==1)
      {
        student2starttime=millis();
        // Serial.println("Student 2 has entered the class (after the TA)");
        digitalWrite(A1, HIGH);
        digitalWrite(A0, LOW);
      delay(2000);
         digitalWrite(A1, LOW);
         digitalWrite(A0, HIGH);
      }
      //if the TA did not enter and student arrived to class early
      else
      {
        //Serial.println("Student 2 has entered the class (before the TA)");
        student2isearly=1;
        digitalWrite(A1, HIGH);
        digitalWrite(A0, LOW);
      delay(2000);
         digitalWrite(A1, LOW);
         digitalWrite(A0, HIGH);
 
      }
     }
     //If student is leaving
     else
     {
      isStudent2Enter=0;
      student2Scanned=0;
      student2isearly=0;
      if(isTAEnter==1)
      {
        student2endtime=millis();
        Serial.println("Student 2 is leaving the class before TA (TA is still in the class)");
      }
      if(isTAEnter==0)
 {Serial.println("Student 2 has left the class after TA left (TA is not in the class)");}
        digitalWrite(A1, HIGH);
        digitalWrite(A0, LOW);
      delay(2000);
         digitalWrite(A1, LOW);
         digitalWrite(A0, HIGH);
 
     }
 
    }
 
    //Student2 ends
 
    
 
}
}

void attendance()
{
    if(endsession>0 && startSession>0 && student1endtime>0 && student1starttime>0 && student2endtime>0 && student2starttime>0)
     {
      sessiontime=endsession-startSession;
     student1spent=student1endtime-student1starttime;
     student2spent=student2endtime-student2starttime;
 
     double student1deserves=student1spent/sessiontime;
     double student2deserves=student2spent/sessiontime;
     if(student1deserves>=0.75)
     {
      student1attendance=student1attendance+1;
     }
     if(student2deserves>=0.75)
     {
      student2attendance=student2attendance+1;
     }
     endsession=0;
     startSession=0;
     student1endtime=0;
     student1starttime=0;
     student2endtime=0;
     student2starttime=0;
     }
 
     if(endsession>0 && startSession>0 && student1endtime>0 && student1starttime>0)
     {
      
      sessiontime=endsession-startSession;
     student1spent=student1endtime-student1starttime;
     student2spent=student2endtime-student2starttime;
 
     double student1deserves=student1spent/sessiontime;
     double student2deserves=student2spent/sessiontime;
     if(student1deserves>=0.75)
     {
      student1attendance=student1attendance+1;
     }
     if(student2deserves>=0.75)
     {
      student2attendance=student2attendance+1;
 
     }
     endsession=0;
     startSession=0;
     student1endtime=0;
     student1starttime=0;
     }
 
     if(endsession>0 && startSession>0 && student2endtime>0 && student2starttime>0)
     {
      sessiontime=endsession-startSession;
     student1spent=student1endtime-student1starttime;
     student2spent=student2endtime-student2starttime;
 
     double student1deserves=student1spent/sessiontime;
     double student2deserves=student2spent/sessiontime;
     if(student1deserves>=0.75)
     {
      student1attendance=student1attendance+1;
     }
     if(student2deserves>=0.75)
     {
      student2attendance=student2attendance+1;
     }
     endsession=0;
     startSession=0;
     student2endtime=0;
     student2starttime=0;
     }
     student1Scanned=0;
 isStudent1Enter=0;
 student1starttime=0;
 student1endtime=0;
 student1isearly=0;

 student2Scanned=0;
isStudent2Enter=0;
student2starttime=0;
student2endtime=0;
student2isearly=0;

isTAEnter=0;
TAScanned=0;
startSession=0;
endsession=0;

Serial.println(student1attendance);
Serial.println(student2attendance);



}






////LCD TEST CODE

//  
//#include <LiquidCrystal.h>
//char ch;
//int Contrast=15;
//// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//
//void setup() 
//{
//  Serial.begin(9600);
//  Serial.println("LCD test with PWM contrast adjustment");
//  pinMode(13,OUTPUT);
////  analogWrite(6,Contrast);
//  
//  // set up the LCD's number of columns and rows: 
//  lcd.begin(16, 2);
//  // Print a message to the LCD.
//  lcd.print("LCD test!!");
//}
//
//void loop() 
//{
//  digitalWrite(13,LOW);
//   delay(1000);
//  digitalWrite(13,HIGH);
//  // set the cursor to column 0, line 1
//  // (note: line 1 is the second row, since counting begins with 0):
//  lcd.setCursor(0, 1);
//  // print the number of seconds since reset:
//  lcd.print(millis()/1000);
//}
//
//void serialEvent()
//{
//     if (Serial.available())
//  {
//    ch= Serial.read();
//    if(ch=='A' && Contrast<255)
//    {
//      Contrast=Contrast+1;
//    }
//    if(ch=='B' && Contrast>0)
//    {
//      Contrast=Contrast-1;
//    }
//        if(ch=='N')
//    {
//      analogWrite(9,28836);
//    }
//       if(ch=='F')
//    {
//      analogWrite(9,0);
//    }
//    analogWrite(6,Contrast);
//    Serial.println("Current contrast");
//    Serial.println(Contrast);
//  }
//}
