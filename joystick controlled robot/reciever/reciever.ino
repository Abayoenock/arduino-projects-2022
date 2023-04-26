// receiver Programming

#include <VirtualWire.h>


const int receive_pin = 3;

String message; 
String myString;

// for joystick
int xvalue; 
int yvalue;
int buttonf;
int xdata; 
int ydata; 
int bfdata; // buttonflag 

// for L298N motor driver
int ena = 5; 
int enb = 6; 

int in1 = 8; 
int in2 = 9; 
int in3 = 10; 
int in4 = 11;
 

void setup()
{
    delay(1000);
    Serial.begin(9600); // Debugging only
  pinMode(ena, OUTPUT); 
  pinMode(enb, OUTPUT); 

  pinMode(in1, OUTPUT); 
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
 analogWrite(ena, 0); 
 analogWrite(enb, 0);
 delay(1000);

    
    Serial.println("setup");

    // Initialise the IO and ISR

    vw_set_rx_pin(receive_pin);

    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);  // Bits per sec

    vw_rx_start();       // Start the receiver PLL running

}

void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
    int i;

     
    // Message with a good checksum received, dump it.
    //Serial.print("Got: ");
    
    for (i = 0; i < buflen; i++)
    {
        char c = (buf[i]);
            message = message + c ; // make a message from the received characters         
    }
  myString = message; 
    Serial.println(message);

String l = getValue(myString, ',', 0); // xvalue
String m = getValue(myString, ',', 1); // yvalue
String n = getValue(myString, ',', 2); // buttonf


xdata = l.toInt();
ydata = m.toInt();
bfdata = n.toInt();

/*
Serial.println("*********from Transmitter************"); 
Serial.println(xdata);  
Serial.println(ydata);
Serial.println(bfdata);
*/

/*
 when the joystic is at normal position we get 
 x = 450 to 550
 y= 450 to 550
 for the these values we want the robot not to move
 */

 if ( ((xdata >= 480)&&(xdata <= 510)) && ( (ydata>=480) && (ydata<=510) ) )
 {
  Serial.println("Stopped"); 
   analogWrite(ena, 0); 
   analogWrite(enb, 0);
  digitalWrite(in1, LOW); 
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
 }

 // if button on the joystick is pressed

 if (bfdata == 0)
 {
   analogWrite(ena, 0); 
   analogWrite(enb, 0);
  digitalWrite(in1, LOW); 
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  
 }

 /* for the forward movement
  *  the value of the x increases above 550 to 1023 while the value of the y statys between 450 and 550
  */

 if ( ((xdata > 510)&&(xdata <= 1023)) && ( (ydata>=480) && (ydata<=510) ) )
 {
   Serial.println("forward"); 
   int xmapped = map(xmapped, xdata, 1023, 255,0);
   int ymapped = map(ymapped, xdata, 1023, 255,0);
   analogWrite(ena, xmapped); 
   analogWrite(enb, ymapped);

  digitalWrite(in1, HIGH); 
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
 }


 /* for the Reverse movement
  *  the value of the x increases above 550 to 1023 while the value of the y statys between 450 and 550
  */

 if ( ((xdata >= 0)&&(xdata < 480)) && ( (ydata>=480) && (ydata<=510) ) )
 {
   Serial.println("reverse");
   int xmapped = map(xmapped, xdata, 479, 255,0);
   int ymapped = map(ymapped, xdata, 479, 255,0);
   analogWrite(ena, xmapped); 
   analogWrite(enb, ymapped);

  digitalWrite(in1, LOW); 
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

 }

 // for right movement
 if (  ( (ydata> 510) && (ydata <= 1023) ) && ( bfdata == 1) )
 {
   Serial.println("Right");
   int ymapped = map(ymapped, ydata, 1023, 255,0);
   analogWrite(ena, ymapped); 
   analogWrite(enb, 0);

  digitalWrite(in1, HIGH); 
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

 }

  // for left movement
 if (  ( (ydata >= 0) && (ydata <= 479) && ( bfdata == 1) ) )
 {
   Serial.println("left");
   int ymapped = map(ymapped, ydata, 479, 255,0);
   analogWrite(ena, 0); 
   analogWrite(enb, ymapped);

digitalWrite(in1, LOW); 
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
 

 }

 
     message = "";
    }
    

}


String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}