#include <IRLib.h>

//=================================================================================================================
// Arduino Nano Pinouts:
// 
// Stepper motor ULN2003 controller board:
// in1 —> Arduino pin D7 (digital out)
// in2 —> Arduino pin D6 (digital out)
// in3 —> Arduino pin D5 (digital out)
// in4 —> Arduino pin D4 (digital out)
// 
// IR sensor breakout board:
// Signal (Pin1) —> Arduino pin D10 (digital in)
// VCC (Pin2) —> Arduino pin D11 (digital out, HIGH)
// GND (Pin3) —> Arduino pin D12 (digital out, LOW)
// 
// Dual colour led on the keypad:
// Green LED pin —> Arduino pin D9 (digital out)
// Red LED pin —> Arduino Pin D8 (digital out)
// 
// Keypad pins:
// Button1 (clockwise) pin1 —> Arduino pin D14 (digital out)
// Button1 (clockwise) pin2 —> Arduino pin D15 (digital in pullup)
// Button2 (counter clockwise) pin1 —> Arduino pin D16 (digital out)
// Button2 (counter clockwise) pin2 —> Arduino pin D17 (digital in pullup)
// 
// Built in LED:
// Arduino pin D13
//=================================================================================================================


//=================================================================================================================
unsigned long currentMillis         = 0;
unsigned long lastMillis1           = 0;              // will store last time LED was updated
unsigned long lastMillis2           = 0;
unsigned long lastMillis3           = 0;
unsigned int  i;
//=================================================================================================================
const long    COUNTERCLOCKWISE      = -1;
const long    CLOCKWISE             = 1;
const long    STOPPED               = 0;
const long    SW_IDLE               = 2;
long          stepperInterval       = 3;
unsigned long stepperState          = 0;
long          stepperDirection      = 0;
long          stepperSpeed          = 5;
int           lastMotorState        = STOPPED;
int           toggleLED             = 0;
int           LED_GREEN             = 9; //Two color LED pin - Green --> arduino pin D9
int           LED_RED               = 8; //Two color LED pin - Green --> arduino pin D8
int           in1                   = 7; //stepper motor controller board pin in1 --> arduino pin D7
int           in2                   = 6; //stepper motor controller board pin in2 --> arduino pin D6
int           in3                   = 5; //stepper motor controller board pin in3 --> arduino pin D5
int           in4                   = 4; //stepper motor controller board pin in4 --> arduino pin D4
int           FULLSTEPS             = 8;
int           HALFSTEPS             = 0;
byte          stepsOffset           = HALFSTEPS;
int           stepperSpeedIndex     = 0;
//=================================================================================================================
int           ledPin                = 13;
unsigned long loopCount             = 0;
//=================================================================================================================
int           RECV_PIN              = 10; // Signal pin of the IR Sensor (Pin1) (digital in)
int           RECV_VCC              = 11; // Power pin of the IR Sensor  (Pin2) (digital out, HIGHT)
int           RECV_GND              = 12; // Ground pin of the IR Sensor (Pin3) (digital out, LOW)
IRrecv        My_Receiver(RECV_PIN);
IRdecode      My_Decoder;
IRdecodeHash  My_Hash_Decoder;
unsigned long hash;
String        command;
String        repeat;
//=================================================================================================================
int           SW_CLKWISE_OUT       = 14;
int           SW_CLKWISE_IN        = 15;
int           SW_CCLKWISE_OUT      = 16;
int           SW_CCLKWISE_IN       = 17;
int           last_sw_state        = SW_IDLE;
unsigned long lastButtonMillis     = 0;
//=================================================================================================================


void Change_Speed(int cmd)
{
  //-------------------------------------
  if((cmd == 1) && (stepperSpeedIndex < 5))
  {
    stepperSpeedIndex++;
  }
  //-------------------------------------
  if((cmd == -1) && (stepperSpeedIndex > -5))
  {
    stepperSpeedIndex--;
  }
  if(cmd == 0)
  {
    stepperSpeedIndex=0;
  }
  //-------------------------------------
  if(stepperSpeedIndex > 0)
  {
    stepperDirection = COUNTERCLOCKWISE;  
  }
  else if(stepperSpeedIndex < 0)
  {
    stepperDirection = CLOCKWISE;
  }
  else
  {
    stepperDirection = STOPPED;
  }
  //-------------------------------------
  stepsOffset = FULLSTEPS;
  //-------------------------------------
  switch(stepperSpeedIndex)
  {
    case  5: stepperSpeed = 1;      break;
    case  4: stepperSpeed = 5;      break;
    case  3: stepperSpeed = 10;     break;
    case  2: stepperSpeed = 50;     break;
    case  1: stepperSpeed = 100;    break;
    case  0: stepperSpeed = 1;      break;
    case -1: stepperSpeed = 100;    break;
    case -2: stepperSpeed = 50;     break;
    case -3: stepperSpeed = 10;     break;
    case -4: stepperSpeed = 5;      break;
    case -5: stepperSpeed = 1;      break;
  }
  //-------------------------------------
}


//=================================================================================================================
//The following codes are sent by my old (2008 model) LG 42LG6000 TV Remote.
//You can adjust the 32bit commands using your own 
void LG_TV_Remote()
{
  if (My_Receiver.GetResults(&My_Decoder)) 
  {//Puts results in My_Decoder
    My_Hash_Decoder.copyBuf(&My_Decoder);//copy the results to the hash decoder
    My_Decoder.decode();
    hash=My_Hash_Decoder.hash;
    My_Hash_Decoder.decode();
    repeat="";
    switch(hash)
    {
      //<TV Commands>
      case 0x180BD9FF: command=" Power";          break;
      case 0x6121DC9F: command=" Input";          break;
      case 0x5DC688C8: command=" TV/Radio";       break;

      case 0x6F019A9F: command=" Q.Menu";         break;
      case 0xACE7C23F: command=" Menu";           break;
      case 0x4DEDA43A: command=" Guide";          break;

      case 0x63D1BF9F: command=" Up";             break;
      case 0xBD7F17A3: command=" Down";           break;
      case 0x7C6035BF: command=" Left";           break;
      case 0x8B7AFB3B: command=" Right";          break;
      case 0xDE8D48A3: command=" Ok";             break;

      case 0x3FF5D1E3: command=" Return";         break;
      case 0xDC244108: command=" Info";           break;
      case 0xF761A783: command=" A/V Mode";       break;

      case 0x9E0A10FF: command=" Volume UP";      break;
      case 0x8EEF4B83: command=" Volume DOWN";    break;

      case 0x8225FADB: command=" Fav";            break;
      case 0x71B93203: command=" Mute";           break;
      
      case 0x86288A23: command=" Program UP";     break;
      case 0x169CDC1F: command=" Program DOWN";   break;

      case 0x2ADE08A3: command=" 1";              break;
      case 0x7F613A3B: command=" 2";              break;
      case 0x1A46B33F: command=" 3";              break;

      case 0x9F3E1063: command=" 4";              break;
      case 0x2FB2625F: command=" 5";              break;
      case 0x4CEB59FF: command=" 6";              break;
  
      case 0xE7D0D303: command=" 7";              break;
      case 0xC984587B: command=" 8";              break;
      case 0x2331B07F: command=" 9";              break;
  
      case 0xD130B09F: command=" 0";              break;

      case 0x453CEDC3: command=" List";           break;
      case 0x67EB0C5F: command=" Q.View";         break;

      case 0xD5257FDB: command=" Red";            break;
      case 0x80A24E43: command=" Green";          break;
      case 0xF3C645B:  command=" Yellow";         break;
      case 0x1FD3B9BF: command=" Blue";           break;

      case 0x969312FF: command=" Text";           break;
      case 0xF0406B03: command=" T.Opt";          break;
      case 0x4962A763: command=" Subtitles";      break;
      case 0xB834019E: command=" Rec";            break;

      case 0xD7EA407B: command=" SimpLink";       break;
      //</TV Commands>
 
      //<Set Top Box Commands>
      case 0x8939B93A: command=" Power";          break;
      case 0xA944F45C: command=" Input";          break;
      case 0x765C89FC: command=" Menu";           break;

      case 0x3D73D39A: command=" Up";             break;
      case 0xEEAEF960: command=" Down";           break;
      case 0xEA65BD7C: command=" Left";           break;
      case 0x15CA97FE: command=" Right";          break;
      case 0xC2B84A96: command=" Ok";             break;

      case 0x33B823A:  command=" Volume UP";      break;
      case 0xB7EFDD40: command=" Volume DOWN";    break;
      case 0x219763C0: command=" Mute";           break;
      
      case 0x1B1D0916: command=" Program UP";     break;
      case 0xA75887DC: command=" Program DOWN";   break;

      case 0x449B4A60: command=" 1";              break;
      case 0x21E458FE: command=" 2";              break;
      case 0xCC48DAFC: command=" 3";              break;

      case 0x20782D6:  command=" 4";              break;
      case 0x8E749A1C: command=" 5";              break;
      case 0x545A393A: command=" 6";              break;
  
      case 0xDED7F0C0: command=" 7";              break;
      case 0xD7C13ABE: command=" 8";              break;
      case 0x5C11783C: command=" 9";              break;
  
      case 0xD014E29A: command=" 0";              break;
      //</Set Top Box Commands>

      case 0x50C5D1F: repeat=" Repeat";           break;
      default:        command="";
    }


    if(hash != 0)
    {
      if(  (hash != 0x50C5D1F) ||
          ((hash == 0x50C5D1F) && (command==" Volume UP"))   ||
          ((hash == 0x50C5D1F) && (command==" Volume DOWN")) ||
          ((hash == 0x50C5D1F) && (command==" Program UP"))  ||
          ((hash == 0x50C5D1F) && (command==" Program DOWN")) )
      {
        Serial.print("0x");
        Serial.print(hash, HEX); // Do something interesting with this value
        Serial.println(command);
        if((command==" Program DOWN") || (command==" Volume DOWN"))
        {
          if (hash != 0x50C5D1F)
          {
            Change_Speed(COUNTERCLOCKWISE);
          }
        }
        if((command==" Program UP") || (command==" Volume UP"))
        {
          if (hash != 0x50C5D1F)
          {
            Change_Speed(CLOCKWISE);
          }
        }
        
        if(command==" 9")
        {
             stepperSpeed = 1;
             stepsOffset = FULLSTEPS;
        }
        if(command==" 8")
        {
             stepperSpeed = 1;
             stepsOffset = HALFSTEPS;
        }
        if(command==" 7")
        {
             stepperSpeed = 5;
             stepsOffset = FULLSTEPS;
        }
        if(command==" 6")
        {
             stepperSpeed = 5;
             stepsOffset = HALFSTEPS;
        }
        if(command==" 5")
        {
             stepperSpeed = 10;
             stepsOffset = FULLSTEPS;
        }
        if(command==" 4")
        {
             stepperSpeed = 10;
             stepsOffset = HALFSTEPS;
        }
        if(command==" 3")
        {
             stepperSpeed = 100;
             stepsOffset = FULLSTEPS;
        }
        if(command==" 2")
        {
             stepperSpeed = 100;
             stepsOffset = HALFSTEPS;
        }
        if(command==" 1")
        {
             stepperSpeed = 500;
             stepsOffset = FULLSTEPS;
        }
        if(command==" 0")
        {
             stepperSpeed = 500;
             stepsOffset = HALFSTEPS;
        }


        if( (command==" Mute") || 
            (command==" Fav") || 
            (command==" Return") || 
            (command==" Info") || 
            (command==" A/V Mode") || 
            (command==" Power") )
        {
          Change_Speed(STOPPED);
        }
      }
    }
    else
    {
      stepperDirection = STOPPED;
      Change_Speed(STOPPED);
    }
    My_Receiver.resume(); 
  }
}
//=================================================================================================================




//=================================================================================================================
void StepperMottorControl()
{
  byte stepperStates[16] = { B00001100,  //half steps (start)
                             B00000100,
                             B00000110,
                             B00000010,
                             B00000011,
                             B00000001,
                             B00001001,
                             B00001000,  //half steps (end)
                             
                             B00001000,  //full steps (start)
                             B00000100, 
                             B00000010, 
                             B00000001,  //full steps (end)
                             B00001000,  //full steps (repeat)
                             B00000100, 
                             B00000010, 
                             B00000001}; //full steps (end)

  lastMotorState = stepperDirection;

  if (stepperDirection != STOPPED)
  {
    digitalWrite(in1, stepperStates[stepperState] & 0x01);
    digitalWrite(in2, stepperStates[stepperState] & 0x02);
    digitalWrite(in3, stepperStates[stepperState] & 0x04);
    digitalWrite(in4, stepperStates[stepperState] & 0x08);
    stepperState = (stepperState + stepperDirection) % 8 + stepsOffset;
  }
  else
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
}
//=================================================================================================================
//With this function, I'm checking the state of just two push buttons attached to the following data pins on the Arduino
//SW_CLKWISE_OUT       = 14; //this could be replaced by any ground pin
//SW_CLKWISE_IN        = 15; //this pin is an input with internal pull-up for the clockwise direction push button
//
//SW_CCLKWISE_OUT      = 16; //this could be replaced by any ground pin
//SW_CCLKWISE_IN       = 17; //this pin is an input with internal pull-up for the counter-clockwise direction push button
//
//It was easier using two adjucent pins on the Arduino for each switch than connecting wires to the very few available ground pins when I was testing it initially so I left it like that
//
//The stepper motor initially starts in stopped mode
//Each time 

void handle_button_events()
{
  if( (digitalRead(SW_CLKWISE_IN) == LOW) &&
      (digitalRead(SW_CCLKWISE_IN) == HIGH) &&
      (last_sw_state == SW_IDLE) )
  {
    last_sw_state = CLOCKWISE;
    Change_Speed(CLOCKWISE);
  }
  
  if( (digitalRead(SW_CLKWISE_IN) == HIGH) &&
      (digitalRead(SW_CCLKWISE_IN) == LOW) &&
      (last_sw_state == SW_IDLE) )
  {
    last_sw_state = COUNTERCLOCKWISE;
    Change_Speed(COUNTERCLOCKWISE);
  }

  if( (digitalRead(SW_CLKWISE_IN) == LOW) &&
      (digitalRead(SW_CCLKWISE_IN) == LOW) )
  {
    last_sw_state = STOPPED;
    Change_Speed(STOPPED);
  }

  if( (digitalRead(SW_CLKWISE_IN) == HIGH) &&
      (digitalRead(SW_CCLKWISE_IN) == HIGH) )
  {
    last_sw_state = SW_IDLE;
  }
}


//=================================================================================================================
void setup() 
{
  // set the digital pins 4..7 as output for the Stepper Motor:
  pinMode(in1, OUTPUT);//in1
  pinMode(in2, OUTPUT);//in2
  pinMode(in3, OUTPUT);//in3
  pinMode(in4, OUTPUT);//in4

  pinMode(LED_GREEN, OUTPUT); //Green LED on keypad
  pinMode(LED_RED,   OUTPUT); //Red LED on keypad
  
  Serial.begin(9600);

  // set the power supply pins for the IR Sensor
  pinMode(RECV_VCC, OUTPUT);
  pinMode(RECV_GND, OUTPUT);
  digitalWrite(RECV_VCC, HIGH);  // Pin 2 on IR Sensor
  digitalWrite(RECV_GND, LOW);   // Pin 3 on IR Sensor

  //on-board led
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  My_Receiver.enableIRIn(); // Start the receiver

  //switches
  pinMode(SW_CLKWISE_OUT,  OUTPUT);
  pinMode(SW_CLKWISE_IN,   INPUT_PULLUP);
  pinMode(SW_CCLKWISE_OUT, OUTPUT);
  pinMode(SW_CCLKWISE_IN,  INPUT_PULLUP);
  digitalWrite(SW_CLKWISE_OUT,  LOW);
  digitalWrite(SW_CCLKWISE_OUT, LOW);

  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED,   HIGH);
}
//=================================================================================================================

void toggleKeypadLed(int led)
{
  if(toggleLED==LOW)
  {
    toggleLED=HIGH;
  }
  else
  {
    toggleLED=LOW;
  }
  if(led==LED_GREEN)
  {
    digitalWrite(LED_GREEN, toggleLED);
    digitalWrite(LED_RED,   LOW);
  }
  if(led==LED_RED)
  {
    digitalWrite(LED_RED,   toggleLED);
    digitalWrite(LED_GREEN, LOW);
  }
}




//=================================================================================================================
void loop()
{
  LG_TV_Remote();
  currentMillis  = millis();
  
  
  if(currentMillis >= lastButtonMillis + 30)//30ms debounce
  {
    lastButtonMillis = currentMillis;
    handle_button_events();
  }

  switch(stepperSpeedIndex)
  {
    case  5: if(currentMillis >= lastMillis2 +   50){ toggleKeypadLed(LED_RED); lastMillis2=currentMillis; }      break;
    case  4: if(currentMillis >= lastMillis2 +  100){ toggleKeypadLed(LED_RED); lastMillis2=currentMillis; }      break;
    case  3: if(currentMillis >= lastMillis2 +  200){ toggleKeypadLed(LED_RED); lastMillis2=currentMillis; }      break;
    case  2: if(currentMillis >= lastMillis2 +  400){ toggleKeypadLed(LED_RED); lastMillis2=currentMillis; }      break;
    case  1: if(currentMillis >= lastMillis2 +  800){ toggleKeypadLed(LED_RED); lastMillis2=currentMillis; }      break;
    case  0: digitalWrite(LED_GREEN, HIGH); digitalWrite(LED_RED, HIGH);                                          break;
    case -1: if(currentMillis >= lastMillis2 +  800){ toggleKeypadLed(LED_GREEN); lastMillis2=currentMillis; }    break;
    case -2: if(currentMillis >= lastMillis2 +  400){ toggleKeypadLed(LED_GREEN); lastMillis2=currentMillis; }    break;
    case -3: if(currentMillis >= lastMillis2 +  200){ toggleKeypadLed(LED_GREEN); lastMillis2=currentMillis; }    break;
    case -4: if(currentMillis >= lastMillis2 +  100){ toggleKeypadLed(LED_GREEN); lastMillis2=currentMillis; }    break;
    case -5: if(currentMillis >= lastMillis2 +   5ww0){ toggleKeypadLed(LED_GREEN); lastMillis2=currentMillis; }    break;
  }

  
  if (currentMillis - lastMillis3 >= stepperInterval * stepperSpeed)
  {
    lastMillis3 = currentMillis;
    StepperMottorControl();
    loopCount++;
    if((loopCount % 8) == 0)
    {
      if(digitalRead(ledPin) == LOW)  
      {
        digitalWrite(ledPin, HIGH);
      }
      else
      {
        digitalWrite(ledPin, LOW);
      }
    }
  }
}
//=================================================================================================================
