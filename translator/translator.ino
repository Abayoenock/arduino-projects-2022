#include <SoftwareSerial.h>
SoftwareSerial mySerial(5, 2);
int xpin = A7;
int xadc = 0;
int ypin = A6;
int yadc = 0;

int FLEX_PIN1 = A1;
int FLEX_PIN2 = A2;
int FLEX_PIN3 = A3;
int FLEX_PIN4 = A5;
int FLEX_PIN5 = A4;
int flexADC5 = 0;
unsigned int current_m=0;
unsigned int last_m=0;
String last_sent="";
void setup()
{
  mySerial.begin(9600);
  Serial.begin(9600);
  //  while (!Serial)
  //  {
  //    ; // wait for serial port to connect. Needed for native USB port only
  //  }



}


void printfun(String cp) //to avoid printing repeating symbols
{

  Serial.println(cp);
  current_m=millis();
  if(current_m-last_m<2000 && cp==last_sent ){
  }
  else{ 
  mySerial.println(cp);
  last_sent=cp;
  last_m=millis();
  
  }
}

void loop()
{

  // reading sensor value
  float angle1 = analogRead(FLEX_PIN1);
  float angle2 = analogRead(FLEX_PIN2);
  float angle3 = analogRead(FLEX_PIN3);
  float angle4 = analogRead(FLEX_PIN4);
  float angle5 = analogRead(FLEX_PIN5);

  Serial.println("--------------- analog values ---------------");
  Serial.println(angle1);
  Serial.println(angle2);
  Serial.println(angle3);
  Serial.println(angle4);
  Serial.println(angle5);
  Serial.println("---------------adxl ---------------");

  xadc = analogRead(xpin);
  yadc = analogRead(ypin);
  Serial.println(xadc);
  Serial.println(yadc);

  if (((angle1 >= 996) && (angle1 <= 1002)) && ((angle2 >= 1016) && (angle2 <= 1018)) && ((angle3 >= 1007) && (angle3 <= 1010)) && ((angle4 >= 1011) && (angle4 <= 1015)) && ((angle5 >= 1015) && (angle5 <= 1023)) && (((xadc >= 327) && (xadc <= 337 )) && ((yadc >= 374) && (yadc <= 386))))
    printfun("A");   
  if (((angle1 >= 1005) && (angle1 <= 1007)) && ((angle2 >= 1005) && (angle2 <= 1008)) && ((angle3 >= 985) && (angle3 <= 990)) && ((angle4 >=995) && (angle4 <= 1000)) && ((angle5 >=999) && (angle5 <= 1005)) && (((xadc >= 300) && (xadc <= 324 )) && ((yadc >= 379) && (yadc <= 386))))
    printfun("B");
  if (((angle1 >= 995) && (angle1 <= 1003)) && ((angle2 >= 1009) && (angle2 <= 1014)) && ((angle3 >= 999) && (angle3 <= 1003)) && ((angle4 >= 1002) && (angle4 <= 1009)) && ((angle5 >= 1008) && (angle5 <= 1012)) && (((xadc >= 320) && (xadc <= 340 )) && ((yadc >= 368) && (yadc <= 386))))
    printfun("C");
  if (((angle1 >= 1005) && (angle1 <= 1009)) && ((angle2 >= 1005) && (angle2 <= 1009)) && ((angle3 >= 1006) && (angle3 <= 1008)) && ((angle4 >= 1009) && (angle4 <= 1013)) && ((angle5 >= 1012) && (angle5 <= 1017)) && (((xadc >= 328) && (xadc <= 344 )) && ((yadc >= 380) && (yadc <= 389))))
    printfun("D");
  if (((angle1 >= 1003) && (angle1 <= 1007)) && ((angle2 >= 1015) && (angle2 <= 1019)) && ((angle3 >= 1008) && (angle3 <= 1012)) && ((angle4 >= 1010) && (angle4 <= 1014)) && ((angle5 >= 1013) && (angle5 <= 1017)) && (((xadc >= 340) && (xadc <= 362 )) && ((yadc >= 373) && (yadc <= 390))))
    printfun("E");

  delay(500);

}
//----------------------END-----------------------------