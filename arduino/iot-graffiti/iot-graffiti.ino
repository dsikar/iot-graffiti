/**
 * CloudGraffitiPlotter
 *
 *  Created on: 21.02.2017
 *
 */
 
 // Changes required
 // 1. SSID and Password
 // 2. IP Address of AWS IOT-Trike server


#include <Arduino.h>
#include <Servo.h> 

// ESP8266 includes
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#define USE_SERIAL Serial
ESP8266WiFiMulti WiFiMulti;

// Graffiti-able area constants in mm
#define iXLength 1310
#define iYLength 800
// margins
#define iMarginX 150 
#define iMarginY 150
// initial XY positions
#define iInitX 550
#define iInitY 400

// Delimiter character for move commands
String strDelim = "|";

// Serial debug. Set to 1 to debug.
#define SERIAL_DEBUG 0
#define SERIAL_DEBUG_2 0
// Servo objects
Servo sprayServo;
// Servo pins
int sprayPin = 2; // D4

// Stepper pins
int iDirX = 16; // D0
int iStepX = 5; // D1
int iDirY = 4; // D2
int iStepY = 0; // D3

// Stepper step delay (less delay = faster)
int iStepDelay = 2;
// Serial parsing string
String txtMsg;
// Stepper motor move positions
int cnt;
int iH;
int iV;
int iNextPosX = 0; // or initial position e.g. 50
int iNextPosY = 0; // or initial position e.g. 50

// Current Position - TODO SET in setup();
int iCurrPosX = 1100; //cm
int iCurrPosY = 1100; //cm

// multiplier ~ turn mm into steps
// int iCoordMult = 47; // (46.94836 steps move 1cm)
float fCoordMult = 4.7; // steps to move 1mm

// Http delay - how often to pool web page
int iHttpDelay = 50;

void setup() {

    pinMode(iDirX, OUTPUT);
    pinMode(iStepX, OUTPUT);
    pinMode(iDirY, OUTPUT);
    pinMode(iStepY, OUTPUT);
  
    sprayServo.attach(sprayPin);
    // switch off
    sprayOff(sprayServo);
 
 
    // ESP8266 init routines
    USE_SERIAL.begin(115200);
    // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(3000);
    }

    // Commented out for debugging - COMMENT IN ONCE WORKING
    //WiFiMulti.addAP("zuzunet", "t3rr1v3l!");
    // WiFiMulti.addAP("BTHub6-Z2R7", "HYp7rXgi4Hxm");
    // WiFiMulti.addAP("TP-LINK_C8F400", "80552109");
    WiFiMulti.addAP("Makerversity_2G", "mak3rv3rs1ty");
    // WiFiMulti.addAP("South London Makerspace", "portugal");
    // WiFiMulti.addAP("SKYFF71E", "ARUPSXSY");
}

void initSteppers() {
  setHigh(iDirX);
  setHigh(iDirY);
}

void setHigh(int iPin) {
  digitalWrite(iPin, HIGH);
}

void setLow(int iPin) {
  digitalWrite(iPin, LOW);
}

void loop() {
    
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url

        String httpReq = "http://34.251.26.97/graffiti.php";
        http.begin(httpReq);

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();
        USE_SERIAL.println("httpCode = ");
        USE_SERIAL.println(httpCode);
        USE_SERIAL.println("HTTP_CODE_OK = ");
        USE_SERIAL.println(HTTP_CODE_OK);
        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            //USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);
            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
                parse(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();
    } 
    delay(iHttpDelay);
}

void sprayOn(Servo myServo) {
  /*
  int pos;
  for(pos = 0; pos <= 90; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myServo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  */
  int pos = 65;
  myServo.write(pos);
  delay(15);
}

void sprayOff(Servo myServo) {
  /*
  int pos;
  for(pos = 90; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myServo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  */
  int pos = 0;
  myServo.write(pos);
  delay(15);
}

/*******************

FUNCTIONS

*******************/

/*
  parse - parse message and send to be processed
*/

void parse(String strGraffiti) {
  if(SERIAL_DEBUG) {
    USE_SERIAL.println(strGraffiti);
  }
  
  if(isMove(strGraffiti)) {
    getSetXY(strGraffiti); 
  }
}

bool isMove(String strGraffiti) {
  // expected move format "xx|yy|spray_state"
  int iDelimCount = 0;
  int iStrLength = strGraffiti.length();
  
  for (int i=0; i < iStrLength; i++) {
    if(strGraffiti.substring(i, i+1) == strDelim) {
       iDelimCount++;
    } 
  } 
  if(iDelimCount==2) {
    return true;
  }
  return false;
}

/*
  getSetXY values and move if need be
*/

void getSetXY(String strGraffiti) {
  // format is XX|YY|(1|0), parse
  strGraffiti.trim(); // remove end of line character
  
  int iStart = 0;
  int iEnd = strGraffiti.indexOf(strDelim);
  String strX = strGraffiti.substring(iStart, iEnd);
  int iX = strX.toInt();
  iStart = iEnd + 1;
  iEnd = strGraffiti.indexOf(strDelim, iStart);
  String strY = strGraffiti.substring(iStart, iEnd);
  int iY = strY.toInt();
  iStart = iEnd + 1;
  iEnd = strGraffiti.length();
  String strSpray = strGraffiti.substring(iStart, iEnd);
  int iSpray = strSpray.toInt();
  
  if(SERIAL_DEBUG) {
    USE_SERIAL.print("strX = *");
    USE_SERIAL.print(strX);
    USE_SERIAL.println("*");    
    USE_SERIAL.print("strY = *");
    USE_SERIAL.print(strY);
    USE_SERIAL.println("*");     
    USE_SERIAL.print("strSpray = *");
    USE_SERIAL.print(strSpray); 
    
    USE_SERIAL.println("*");
    USE_SERIAL.print("iX = *");
    USE_SERIAL.print(iX);
    USE_SERIAL.println("*");    
    USE_SERIAL.print("iY = *");
    USE_SERIAL.print(iY);
    USE_SERIAL.println("*");     
    USE_SERIAL.print("iSpray = *");
    USE_SERIAL.print(iSpray); 
    USE_SERIAL.println("*");       
  }
  
  setPos(iX, iY, iSpray);
  
}

/*
  setPos - set x and y timing belt lengths
*/

void setPos(int iX, int iY, int iSpray) {

  // Origin (0,0) set at top left.
  // Mirror to top right if canvas is mounted on
  // a wall - spraying against wall
  // Do not mirror is canvas is mounted on rig -
  // spraying against rig

  // if we are spraying
  if(iSpray) {
   
    // move sqrt to cloud, perfoming poorly on embedded CPU
    // int iHypX = getHypX(iX, iY);
    // int iHypY  = getHypY(iX, iY);
 
    // position spray can only deal with x for now
    windX(iX);
    windY(iY);
    
    // press spray
    sprayOn(sprayServo);
  }
  else {
    // release spray
    // don't bother moving
    sprayOff(sprayServo);
  }
}

void windX(int iX) {
    if(SERIAL_DEBUG) {
      USE_SERIAL.print("windX(int iX)");
    }
      
  int iMove = iX - iCurrPosX;
   if(SERIAL_DEBUG) {
    USE_SERIAL.print("iMove = ");
    USE_SERIAL.println(iMove);
    USE_SERIAL.print("abs(iX - iCurrPosX) = ");
    USE_SERIAL.println(abs(iX - iCurrPosX)); 
  }
  // NB Inverted logic for X
  if(iMove > 0) {
    if(SERIAL_DEBUG) {
      USE_SERIAL.print("setHigh(iDirX)"); 
    }     
    // setLow(iDirX);
    setHigh(iDirX);
  } else {
    if(SERIAL_DEBUG) {
      USE_SERIAL.print("setLow(iDirX)"); 
    }     
    // setHigh(iDirX);
    setLow(iDirX);
  }
  float fTotalSteps = abs(iMove) * fCoordMult;
  int iTotalSteps = fTotalSteps;
   if(SERIAL_DEBUG) {
    USE_SERIAL.print("iTotalSteps = ");
    USE_SERIAL.println(iTotalSteps);
   }  
  for(int i = 0; i <= iTotalSteps; i++) {
    oneStep(iStepX);
  }
  // keep track of where we are
  iCurrPosX = abs(iX);
  if(SERIAL_DEBUG) {
    USE_SERIAL.print("iCurrPosX = ");
    USE_SERIAL.println(iCurrPosX);
  }   
}

void windY(int iY) {
  int iMove = iY - iCurrPosY;  
  if(iMove > 0) {
    // setHigh(iDirY);
    setLow(iDirY);
  } else {
    // setLow(iDirY);
    setHigh(iDirY);
  }
  float fTotalSteps = abs(iMove) *  fCoordMult;
  int iTotalSteps = fTotalSteps;
  for(int i = 0; i <= iTotalSteps; i++) {
    oneStep(iStepY);
  }
  // keep track of where we are
  iCurrPosY = abs(iY);
}

void oneStep(int iStepPin) {
    digitalWrite(iStepPin, HIGH);
    delay(iStepDelay);
    digitalWrite(iStepPin, LOW);
    delay(iStepDelay);  
}
