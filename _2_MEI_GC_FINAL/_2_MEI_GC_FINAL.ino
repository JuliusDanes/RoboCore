 #include <Wire.h>
#include <Servo.h>

#define ADDRESS 0x60
 
unsigned char high_byte, low_byte, angle8;
unsigned int sudut;

Servo esc;
int relay2=A10;

char buff;
String data = "";
int kejarBolaStatus=0;

int start=A15, stop=A14, maju=A13, mundur=A12, rotkan=43, rotkir=23, kanan=25, kiri=27, kompas=29, bldc=45, outprox=33;
int valbldc;

int prox=A6;
int nilai;

int x=1;
int a=1;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(prox,INPUT);
  pinMode(relay2,OUTPUT);
  esc.writeMicroseconds(1500);
  esc.attach(7);
  
  pinMode(start,OUTPUT);
  digitalWrite(start,HIGH);
  pinMode(stop,OUTPUT);
  digitalWrite(stop,HIGH);
  pinMode(maju,OUTPUT);
  digitalWrite(maju,HIGH);
  pinMode(mundur,OUTPUT);
  digitalWrite(mundur,HIGH);
  pinMode(rotkan,OUTPUT);
  digitalWrite(rotkan,HIGH);
  pinMode(rotkir,OUTPUT);
  digitalWrite(rotkir,HIGH);
  pinMode(kanan,OUTPUT);
  digitalWrite(kanan,HIGH);
  pinMode(kiri,OUTPUT);
  digitalWrite(kiri,HIGH);
  pinMode(kompas,OUTPUT);
  digitalWrite(kompas,HIGH);
  pinMode(bldc,INPUT_PULLUP);
  pinMode(outprox,OUTPUT);
  digitalWrite(kompas,HIGH);
}

void cmps(){
  // put your main code here, to run repeatedly:
  // begin communication with CMPS11
  Wire.beginTransmission(ADDRESS); 
  Wire.write(2); // Start read
  Wire.endTransmission();

  // Request 4 bytes from CMPS11
  Wire.requestFrom(ADDRESS, 4);
  
  // Wait for the bytes to arrive
  if (Wire.available()>=4){
  
  // Read the values
  high_byte = Wire.read(); 
  low_byte = Wire.read();

 
  // Calculate full bearing
  sudut = (( high_byte<<8) + low_byte) / 10;
  }
  // Calculate bearing decimalrt delay before next loop
}

void baca(){
  valbldc=digitalRead(bldc);
  nilai=digitalRead(prox);
}

void respone(String message)
{
  
      if(message == "x+"){
      digitalWrite(start,HIGH);
      digitalWrite(stop,HIGH); 
      digitalWrite(maju,LOW);     
      delay(100);
      digitalWrite(maju,HIGH);
      Serial.println(0);
    } 
    else if(message == "x-"){
      digitalWrite(start,HIGH);
      digitalWrite(stop,HIGH); 
      digitalWrite(mundur,LOW);    
      delay(100);
      digitalWrite(mundur,HIGH);
      Serial.println(0);
     }  
    else if(message == "z+"){
      digitalWrite(start,HIGH);
      digitalWrite(stop,HIGH); 
      digitalWrite(rotkan,LOW);    
      delay(100);
      digitalWrite(rotkan,HIGH);
      Serial.println(0);
     } 
    else if(message == "z-"){
      digitalWrite(start,HIGH);
      digitalWrite(stop,HIGH); 
      digitalWrite(rotkir,LOW);    
      delay(100);
      digitalWrite(rotkir,HIGH);
      Serial.println(0);
     }  
    else if(message == "y-"){
      digitalWrite(start,HIGH);
      digitalWrite(stop,HIGH); 
      digitalWrite(kiri,LOW);     
      delay(100);
      digitalWrite(kiri,HIGH);
      Serial.println(0);
     }  
    else if(message == "y+"){
      digitalWrite(start,HIGH);
      digitalWrite(stop,HIGH); 
      digitalWrite(kanan,LOW);     
      delay(100);
      digitalWrite(kanan,HIGH);
      Serial.println(0);
     } 
     else if(message == "S"){  //Stop
      digitalWrite(start,HIGH);
      digitalWrite(stop,LOW);
      Serial.println(0); 
     } 
     else if(message == "s"){
      digitalWrite(stop,HIGH);
      digitalWrite(start,LOW);
      Serial.println(0); //Start
     } 
    Serial.println(0);
  
}

void receive()
{
  if (Serial.available() > 0)
  {
    buff = Serial.read();
    if (buff == '\n')
    {      
//      Serial.println(data);
      respone(data);
      data = "";
    }
    else
      data += buff;
  }
}

void loop() {
  esc.writeMicroseconds(1500);  
  receive();
  cmps();
  if (sudut>=273 && sudut<=279){
        digitalWrite(kompas,LOW);
        a=0;
     }
  else {
      digitalWrite(kompas,HIGH);
    a=1;
  }
 
  
  nilai=digitalRead(prox);
  
//  Serial.print(a);
//  Serial.print(",");
//  Serial.println(sudut);
  if (nilai==LOW){
        digitalWrite(outprox,LOW);
        
     }
     
  else if (nilai==HIGH){
        
        digitalWrite(outprox,HIGH);
     }
     
  valbldc=digitalRead(bldc);
     
  if (valbldc==LOW){
      x=2;
      Serial.println("s");
      Serial.println("s");
      digitalWrite(relay2,HIGH);
      delay(200);
      digitalWrite(relay2,HIGH);
      esc.writeMicroseconds(1500);
      esc.writeMicroseconds(1565);
      delay(800);//800
      esc.writeMicroseconds(1500);
      delay(500);//500//1000
      digitalWrite(relay2,LOW);
      delay(500);//500//1800
      esc.writeMicroseconds(1600);
      delay(700);
      esc.writeMicroseconds(1500);
      delay(700);
      
//      valbldc=HIGH;
//      digitalWrite(bldc,HIGH);
      Serial.println("s");
      Serial.println("s");
  }
  
  else if (valbldc==HIGH){
    x=3;
    digitalWrite(relay2,HIGH);
    esc.writeMicroseconds(1500);
  }
     
//     Serial.print(sudut);
//     Serial.print(",");
     Serial.println(0);
}
