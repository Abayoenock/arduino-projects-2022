void setup() {
  Serial.begin(9600); //Start Serial Monitor to display current read value on Serial monitor
}

void loop() {
unsigned int x=0;
float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

  for (int x = 0; x < 150; x++){ //Get 150 samples
  AcsValue = analogRead(A0);     //Read current sensor values   
    Samples = Samples + AcsValue;  //Add samples together
  delay (3); // let ADC settle before next sample 3ms
}
AvgAcs=Samples/150.0;//Taking Average of Samples
Serial.print("volate:");
Serial.println(AvgAcs * (5.0 / 1024.0));

//((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
//2.5 is offset(I assumed that arduino is working on 5v so the viout at no current comes
//out to be 2.5 which is out offset. If your arduino is working on different voltage than 
//you must change the offset according to the input voltage)
//0.185v(185mV) is rise in output voltage when 1A current flows at input
AcsValueF = ((AvgAcs * (5.0 / 1024.0))-1.37 )/0.185;

Serial.println(AcsValueF);//Print the read current on Serial monitor
delay(50);
}