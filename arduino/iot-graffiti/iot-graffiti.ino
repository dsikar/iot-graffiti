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

// Graffiti-able area constants in cm
#define iXLength 110
#define iYLength 80
// margins
#define iMarginX 15 
#define iMarginY 15
// initial XY positions
#define iInitX 55
#define iInitY 40

// Delimiter character for move commands
String strDelim = "|";

// Serial debug. Set to 1 to debug.
#define SERIAL_DEBUG 1
// Servo objects
Servo sprayServo;
// Servo pins
int sprayPin = 5; // D1

// Stepper pins
int iDirX = 4; // D2
int iStepX = 0; // D3
int iDirY = 2; // D4
int iStepY = 14; // D5

// LED pin - web access feedback
int iLEDPin = 16; // NodeMCU LED pin

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

// Current Position - SET
int iCurrPosX = 64; //cm
int iCurrPosY = 64; //cm

// multiplier ~ turn cm into steps
int iCoordMult = 47; // (46.94836 steps move 1cm)

void setup() {

    pinMode(iDirX, OUTPUT);
    pinMode(iStepX, OUTPUT);
    pinMode(iDirY, OUTPUT);
    pinMode(iStepY, OUTPUT);
  
    sprayServo.attach(sprayPin);
    // switch off
    servoOff(sprayServo);
 
 
    // ESP8266 init routines
    USE_SERIAL.begin(115200);
    // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
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
    setHigh(iStepDelay);
    delay(500);
    setLow(iStepDelay);
}

/*
void parse(String strCmd) {
 if(strCmd == "LT") {
   USE_SERIAL.println("Left Turn");
   int iNextPos = pos - 10;
  USE_SERIAL.println("pos before call =");
  USE_SERIAL.println(pos);
   // for(iCurrPos; iCurrPos <= iNextPos; iCurrPos -= 1) // goes from 0 degrees to 180 degrees 
  for(pos; pos>=iNextPos; pos-=1)
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(40);                       // waits 15ms for the servo to reach the position 
  } 
  USE_SERIAL.println("pos after call =");
  USE_SERIAL.println(pos);
 } 
 if(strCmd == "RT") {
   //USE_SERIAL.println("Right Turn");
   int iNextPos = pos + 10;
   // for(iCurrPos; iCurrPos <= iNextPos; iCurrPos -= 1) // goes from 0 degrees to 180 degrees 
  for(pos; pos<=iNextPos; pos+=1)
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(40);                       // waits 15ms for the servo to reach the position 
  }  
  //USE_SERIAL.println("pos =");
  //USE_SERIAL.println(pos);  
 }
 if(strCmd == "FW") {
   //USE_SERIAL.println("Forward");
   myservo2.writeMicroseconds(iGo);
   delay(15); 
   
 }
 if(strCmd == "RV") {
   USE_SERIAL.println("Reverse");
   myservo2.writeMicroseconds(iReverse);
   delay(15); 
   
 } 
 if(strCmd == "ST") {
   USE_SERIAL.println("Stop");
   myservo2.writeMicroseconds(iStop);
   delay(15); 
 } 
}
*/

void servoOn(Servo myServo) {
  /*
  int pos;
  for(pos = 0; pos <= 90; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myServo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  */
  int pos = 90;
  myServo.write(pos);
  delay(15);
}

void servoOff(Servo myServo) {
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
  processMsg - process and delete message
*/

void processMsg(String &txtMsg) {
   txtMsg.trim();
  if(SERIAL_DEBUG) {
    USE_SERIAL.println(txtMsg);
  }   
   if(txtMsg == "S1ON") {
    servoOn(sprayServo);
    txtMsg = "";
    return;
   }  
   if(txtMsg == "S1OFF") {
    servoOff(sprayServo);
    txtMsg = "";
    return;
   }  
   if(txtMsg.substring(0,1) == "H" || txtMsg.substring(0,1) == "V") {
     setval(txtMsg);
     txtMsg = "";
     return;
   }
   // x,y coordinates
   if(txtMsg.indexOf(",") > 0) { // we have a coordinate pair, move stepper
     parseXY(txtMsg);
     txtMsg = "";
   }
   if(txtMsg == "ART1") {
    parse("53,53\nS1ON\n75,53\n75,75\n53,75\n53,53\nS1OFF\n64,64\n");
    txtMsg = "";
    return;   
   }     
   txtMsg = ""; 
}

/*
  parse - parse message and send to be processed
*/

void parse(String strGraffiti) {
  if(SERIAL_DEBUG) {
    USE_SERIAL.println(strGraffiti);
  }
  
  if(isMove(strGraffiti)) {
    doMove(strGraffiti); 
  }
  
  /*
  int iLen = strGraffiti.length();
  String sLineEnd = "\n";
  
  for(int i = 0; i < iLen;) {
    int iPos = strGraffiti.indexOf(sLineEnd, i);
    if(iPos < 0) {break;} // last character expected to be newline, if not, stop
    String txtMsg = strGraffiti.substring(i, iPos);
    processMsg(txtMsg);
    //   break;
    i = iPos + 1;    
  }
  */
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

void doMove(String strGraffiti) {
  // format is XX|YY|(1|0), parse
  strGraffiti.trim(); // remove end of line character
  
  int iStart = 0;
  int iEnd = strGraffiti.indexOf(strDelim);
  String strX = strGraffiti.substring(iStart, iEnd);
  iStart = iEnd + 1;
  iEnd = strGraffiti.indexOf(strDelim, iStart);
  String strY = strGraffiti.substring(iStart, iEnd);
  iStart = iEnd + 1;
  iEnd = strGraffiti.length();
  String strSpray = strGraffiti.substring(iStart, iEnd);
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
  }   
}
/*
  parseXY - parse XY coordinates in format XX,YY
*/

void parseXY(String txtMsg) {
  String strDelim = ","; // TODO move to config file
  int iPos = txtMsg.indexOf(strDelim);
  String strCoord = txtMsg.substring(0, iPos);
  int iX = strCoord.toInt();
  strCoord =  txtMsg.substring(iPos+1);
  int iY = strCoord.toInt();
  setPos(iX, iY);
}

/*
  setval - legacy function from cinema4d rig
*/

void setval(String txtMsg)
{
    String strStepper = txtMsg.substring(0,1);
    String pos = txtMsg.substring(1);
    if (strStepper == "H") {
      //myservoH.write(data);
      iH = pos.toInt();    
      cnt ++;
    }
    if (strStepper == "V"){
    //myservoV.write(data);
    iV = pos.toInt();
    cnt ++;
    }
    if (cnt % 2 == 0)
    {
    setPos(iH, iV);
    delay(15);
    }
}

/*
  setPos - set x and y timing belt lengths
*/

void setPos(int iX, int iY) {

  // Origin (0,0) set at top left.
  // Mirror to top right if canvas is mounted on
  // a wall.
  // Do not mirror is canvas is mounted on rig.
  
  if(SERIAL_DEBUG) {
    USE_SERIAL.print("x = ");
    USE_SERIAL.println(iX);
    USE_SERIAL.print("y = ");
    USE_SERIAL.println(iY); 
  }
  
  int iHypX = getHypX(iX, iY);
  int iHypY  = getHypY(iX, iY);
 
   if(SERIAL_DEBUG) {
    USE_SERIAL.print("iHypX = ");
    USE_SERIAL.println(iHypX);
    USE_SERIAL.print("iHypY = ");
    USE_SERIAL.println(iHypY); 
  }
  
  windX(iHypX);
  windY(iHypY);
}

int getHypX(int iX, int iY) {
        // my ($i, $r) = @_;
        int b = iMarginX + iX;
        int a = iMarginY + (iYLength - iY);
        // add 0.5 to round to nearest integer
        int iHypX = (sqrt(a * a + b * b) + 0.5);
        return iHypX;
}

int getHypY(int iX, int iY) {
        // my ($i, $r) = @_;
        int b = iMarginX + (iXLength - iX);
        int a = iMarginY + (iYLength - iY);
        // add 0.5 to round to nearest integer
        int iHypY = (sqrt(a * a + b * b) + 0.5);
        return iHypY;
}

void moveX(int iX) {
  // Subtract the required position from the current position
  // e.g. current = 25, required = 10 ~ move = required - current = -15
  windX(iX - iCurrPosX);
  iCurrPosX = abs(iX); // e.g. current position after move ~ 
}

void windX(int iX) {
  int iMove = iX - iCurrPosX;
   if(SERIAL_DEBUG) {
    USE_SERIAL.print("windX(int iX) ~ iX = ");
    USE_SERIAL.println(iX);
    USE_SERIAL.print("abs(iX - iCurrPosX) = ");
    USE_SERIAL.println(abs(iX - iCurrPosX)); 
  }
  // NB Inverted logic for X
  if(iMove > 0) {
    if(SERIAL_DEBUG) {
      USE_SERIAL.print("setHigh(iDirX)"); 
    }     
    setLow(iDirX);
  } else {
    if(SERIAL_DEBUG) {
      USE_SERIAL.print("setLow(iDirX)"); 
    }     
    setHigh(iDirX);
  }
  int iTotalSteps = abs(iMove) * iCoordMult;
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
    setHigh(iDirY);
  } else {
    setLow(iDirY);
  }
  int iTotalSteps = abs(iMove) * iCoordMult;
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
