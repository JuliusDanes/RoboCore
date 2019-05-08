
#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include <Servo.h> 
#include <ros.h>
#include <std_msgs/UInt16.h>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float32MultiArray.h"

ros::NodeHandle  nh;

Servo esc;
Servo servo;  //UP DOWN
Servo servo1; //LEFT RIGHT

int x;
int y;
int pwmmaju1,pwmmaju2,pwmkan,pwmkir;

int dir1=5;
int dir2=14;
int pwm=8;

int dir3=15;
int dir4=16;
int pwm2=9;

int dir5=17;
int dir6=31;
int pwm3=10;

int dir7=33;
int dir8=35;
int pwm4=12;

int dir9=28;
int dir10=6;
int pwm5=13;

int prox=A3;
int nilaiprox;

int start=36,stop=34,inmaju=32,inmundur=A12,inrotkan=44,inrotkir=A10,inkanan=A9,inkiri=A8,inkompas=A7,bldc=42, inprox=A5;
int valstart,valstop,valmaju,valmundur,valrotkan,valrotkir,valkanan,valkiri,valkompas,valprox;
int relay2=A13;

int a=1;

void servo_cb( const std_msgs::Float32MultiArray cmd_msg){
  x=cmd_msg.data[0];
  y=cmd_msg.data[1];
}

//std_msgs::UInt16 nyoba;
ros::Subscriber<std_msgs::Float32MultiArray> sub("servo", servo_cb);
//ros::Publisher chatter("chatter", &nyoba);

void setup(){
  nh.initNode();
  nh.subscribe(sub);
  //nh.advertise(chatter);
  Serial.begin(57600);
  pinMode(relay2,OUTPUT);
  digitalWrite(relay2,HIGH);
  pinMode(dir1,OUTPUT);
  pinMode(dir2,OUTPUT);
  pinMode(pwm,OUTPUT);
  pinMode(dir3,OUTPUT);
  pinMode(dir4,OUTPUT);
  pinMode(pwm2,OUTPUT);
  pinMode(dir5,OUTPUT);
  pinMode(dir6,OUTPUT);
  pinMode(pwm3,OUTPUT);
  pinMode(dir7,OUTPUT);
  pinMode(dir8,OUTPUT);
  pinMode(pwm4,OUTPUT);
  pinMode(dir9,OUTPUT);
  pinMode(dir10,OUTPUT);
  pinMode(pwm5,OUTPUT);
  pinMode(prox,INPUT);

  pinMode(bldc,OUTPUT);
  pinMode(bldc,HIGH);
  
  pinMode(start,INPUT_PULLUP);
  pinMode(stop,INPUT_PULLUP);
  pinMode(inmaju,INPUT_PULLUP);
  pinMode(inmundur,INPUT_PULLUP);
  pinMode(inrotkan,INPUT_PULLUP);
  pinMode(inrotkir,INPUT_PULLUP);
  pinMode(inkanan,INPUT_PULLUP);
  pinMode(inkiri,INPUT_PULLUP);
  pinMode(inkompas,INPUT_PULLUP);
  pinMode(inprox,INPUT_PULLUP);
    
}
void loop(){
   nh.spinOnce();
  //nyoba.data = pwmmaju1;
  //chatter.publish(&nyoba);
  //cmps();
 
  a=1;
  valprox = digitalRead(inprox);
  
  digitalWrite(bldc,HIGH);
    valstart = digitalRead(start);
    valstop = digitalRead(stop);
    valmaju = digitalRead(inmaju);
    valmundur = digitalRead(inmundur);
    valrotkan = digitalRead(inrotkan);
    valrotkir = digitalRead(inrotkir);
    valkanan = digitalRead(inkanan);
    valkiri = digitalRead(inkiri);
    valkompas = digitalRead(inkompas);
    

if (valstart==LOW){
  valprox = digitalRead(inprox);
 // bacaprox();
  if (valprox==HIGH){
      valkompas = digitalRead(inkompas);
      digitalWrite(bldc,HIGH);
      a=2;
      dribble();
      int x1=x;
      int y1=y;
      pwmmaju1=map(y1, 474, 1, 93, 173);//keecepatan harus lebih tinggi
      pwmmaju2=map(y1, 474, 1, 90, 170);
      pwmkan=map(x1, 332 , 638, 40, 50);//(((x-nr)/3)+5); ////30-70
      pwmkir=map(x1, 1, 287, 50, 30); //(((nl-x)+30)/3);//20-116 ////70-30
      
      if (x1>=288 && x1<=331 ){//300-400//263-466, 286-353
        maju();
      }
      
      else if(x1>=1 && x1<=287){
        kiri();
      }
    
      else if(x1>=332 && x1<=640){
        kanan();
      }
    
      else if(x1==0){
        rotkan();
      }

  }

else if (valprox==LOW){
      a=4;
      dribblekir();
      //int h;
      //h++;
      //while(h>0){

      valkompas = digitalRead(inkompas);
      if (valkompas==LOW){
        a=0;
        dribblekir();
        majuman();
        delay(2000);
        digitalWrite(bldc,LOW);
        delay(5);
        digitalWrite(bldc,HIGH);
        berhenti();
        delay(3500);
        //break;
      }
     else {
        digitalWrite(bldc,HIGH);
        a=3;
        rotkan();
        //break;
        }
      //}
  }
  
}



else if (valmaju==LOW){
  dribbleoff();
  majuman();
  }
else if (valmundur==LOW){
  dribbleoff();
  mundur();
  }
else if (valrotkan==LOW){
  dribbleoff();
  rotkan();
  }
else if (valrotkir==LOW){
  dribbleoff();
  rotkir();
  }
else if (valkanan==LOW){
  dribbleoff();
  kananman();
  }
else if (valkiri==LOW){
  dribbleoff();
  kiriman();
  }
else if (valstop==LOW){
  dribbleoff();
  berhenti();
}

else berhenti();
//
//  Serial.print(valprox);
//  Serial.print(",");
//  Serial.print(valkompas);
//  Serial.print(",");
//  Serial.println(a);
}

void majuman(){
  digitalWrite(dir5,HIGH);
  digitalWrite(dir6,LOW);
  analogWrite(pwm3,112);//72//kanan
  digitalWrite(dir7,LOW);
  digitalWrite(dir8,HIGH);
  analogWrite(pwm4,110);//70//kiri
  digitalWrite(dir9,HIGH);
  digitalWrite(dir10,HIGH);
  digitalWrite(pwm5,0);
  }

void maju(){
  digitalWrite(dir5,HIGH);
  digitalWrite(dir6,LOW);
  analogWrite(pwm3,pwmmaju1);//72//kanan
  digitalWrite(dir7,LOW);
  digitalWrite(dir8,HIGH);
  analogWrite(pwm4,pwmmaju2);//70//kiri
  digitalWrite(dir9,HIGH);
  digitalWrite(dir10,HIGH);
  digitalWrite(pwm5,0);
  }

void mundur(){
  digitalWrite(dir5,LOW);
  digitalWrite(dir6,HIGH);
  analogWrite(pwm3,112);//72//kanan
  digitalWrite(dir7,HIGH);
  digitalWrite(dir8,LOW);
  analogWrite(pwm4,110);//70//kiri
  digitalWrite(dir9,HIGH);
  digitalWrite(dir10,HIGH);
  digitalWrite(pwm5,0);
  }
  
void kanan(){
  digitalWrite(dir5,HIGH);
  digitalWrite(dir6,LOW);
  analogWrite(pwm3,pwmmaju1);//pwmmaju1//tinggi
  digitalWrite(dir7,LOW);
  digitalWrite(dir8,HIGH);
  analogWrite(pwm4,pwmmaju2);//pwmmaju2
  digitalWrite(dir9,HIGH);
  digitalWrite(dir10,LOW);
  analogWrite(pwm5,pwmkan);
  }

void kananman(){
  digitalWrite(dir5,LOW);
  digitalWrite(dir6,HIGH);
  analogWrite(pwm3,40);//pwmmaju1//tinggi
  digitalWrite(dir7,LOW);
  digitalWrite(dir8,HIGH);
  analogWrite(pwm4,80);//pwmmaju2
  digitalWrite(dir9,LOW);
  digitalWrite(dir10,HIGH);
  analogWrite(pwm5,135);
  }

void kiri(){
  digitalWrite(dir5,HIGH);
  digitalWrite(dir6,LOW);
  analogWrite(pwm3,pwmmaju1);//kanan
  digitalWrite(dir7,LOW);
  digitalWrite(dir8,HIGH);
  analogWrite(pwm4,pwmmaju2);//kiri
  digitalWrite(dir9,LOW);
  digitalWrite(dir10,HIGH);
  analogWrite(pwm5,pwmkir);//tengah
  }

void kiriman(){
  digitalWrite(dir5,HIGH);
  digitalWrite(dir6,LOW);
  analogWrite(pwm3,80);//kanan
  digitalWrite(dir7,HIGH);
  digitalWrite(dir8,LOW);
  analogWrite(pwm4,40);//kiri
  digitalWrite(dir9,HIGH);
  digitalWrite(dir10,LOW);
  analogWrite(pwm5,135);//tengah
  }

void rotkir(){
  digitalWrite(dir5,HIGH);
  digitalWrite(dir6,LOW);
  analogWrite(pwm3,30);
  digitalWrite(dir7,HIGH);
  digitalWrite(dir8,LOW);
  analogWrite(pwm4,30);
  digitalWrite(dir9,LOW);
  digitalWrite(dir10,HIGH);
  analogWrite(pwm5,30);
  }

 void rotkan(){
  digitalWrite(dir5,LOW);
  digitalWrite(dir6,HIGH);
  analogWrite(pwm3,30);
  digitalWrite(dir7,LOW);
  digitalWrite(dir8,HIGH);
  analogWrite(pwm4,30);
  digitalWrite(dir9,HIGH);
  digitalWrite(dir10,LOW);
  analogWrite(pwm5,30);
  }

void dribblekan(){
  digitalWrite(dir1,HIGH);
  digitalWrite(dir2,LOW);
  analogWrite(pwm,90);
  digitalWrite(dir3,LOW);
  digitalWrite(dir4,HIGH);
  analogWrite(pwm2,10);
}

void bacaprox(){
  nilaiprox=digitalRead(prox);
  }

void dribblekir(){
  digitalWrite(dir1,LOW);
  digitalWrite(dir2,HIGH);
  analogWrite(pwm,10);
  digitalWrite(dir3,HIGH);
  digitalWrite(dir4,LOW);
  analogWrite(pwm2,110);
}

void dribble(){
  digitalWrite(dir1,HIGH);
  digitalWrite(dir2,LOW);
  analogWrite(pwm,50);
  digitalWrite(dir3,HIGH);
  digitalWrite(dir4,LOW);
  analogWrite(pwm2,50);
}

void dribbleoff(){
  digitalWrite(dir1,LOW);
  digitalWrite(dir2,LOW);
  analogWrite(pwm,0);
  digitalWrite(dir3,LOW);
  digitalWrite(dir4,LOW);
  analogWrite(pwm2,0);
}

void berhenti(){
  digitalWrite(dir5,HIGH);
  digitalWrite(dir6,HIGH);
  analogWrite(pwm3,0);
  digitalWrite(dir7,HIGH);
  digitalWrite(dir8,HIGH);
  analogWrite(pwm4,0);
  digitalWrite(dir9,HIGH);
  digitalWrite(dir10,HIGH);
  analogWrite(pwm5,0);
  }
