#include <Servo.h>

Servo s1;  // create servo objects to control a servo
Servo s2;  
int s1pin = 9;
int s2pin = 10;
int PIRpin = 12;
int laserPin = 13;

void setup() {
  s1.attach(s1pin,600,2300);  // (pin, min, max)
  s2.attach(s2pin,600,2300);
  pinMode(PIRpin, INPUT);
  pinMode(laserPin, OUTPUT);
  pinMode(A0, OUTPUT);
  Serial.begin(9600);
}


bool detectMotion(){
  int stat = digitalRead(PIRpin);
  if(stat == HIGH){
    return true;
  }else{
    return false;
  }
}

bool trigger(){
  int d=1000; // adjust the sensitivity to motion
  if(detectMotion()){
    delay(d);
    if(detectMotion()){
      delay(d/2);
      if(detectMotion()){
        return true;
      }
    }
  }return false;
}

class LaserPointer{     //object to represent the laser pointer mechanism
  private:              //the object can then have a handful of pre-made 'behaviors' or movement patterns
    int minX = 0;       //min X bound
    int maxX = 180;     //max X bound
    int minY = 0;       //min Y bound
    int maxY = 95;      //max Y bound
    int currentX = 0;   //track current X position
    int currentY = 0;   //track current Y position
    int msc = 5;        //movement speed constant
  public:
    int de = 350;
    void laserOn(){     //turn on laser diode
      digitalWrite(laserPin, HIGH);
    }
    void laserOff(){    //turn off laser diode
      digitalWrite(laserPin, LOW);
    }
    void setX(int x){   //set x position. The X movements will be slowed, and the speed can be controlled by changing the movement speed constant
      if(x>=minX && x<=maxX){
        if(x<currentX){
          for(int i = currentX; i>= x; i--){
            s1.write(i);
            delay(msc);
          }
        }else{
          for (int i = currentX; i<=x; i++){
            s1.write(i);
            delay(msc);
          }
        }
        s1.write(x);
        currentX=x;
      }
    }
    void setY(int y){   //the Y will move instantly, because the Y range of motion is much smaller and doesn't benifit much from being artificially slowed
      if(y>=minY && y<=maxY){
        y = 180-y;
        s2.write(y);
        currentY=y;
      }
    }
    void z(){           //move in a 'Z' pattern (broken with introduction of slowed X movement)
      setX(20);
      setY(0);
      delay(de);
      setY(65);
      delay(de);
      setX(140);
      setY(0);
      delay(de);
      setY(65);
      delay(de);
    };
    void backAndForth(){  // alternatve moving to the left and right, raising the Y by a little bit each time
      int left = 20;
      int right = 160;
      for(int i = 0; i < 10; i++){
        int y = 20+i*5;
        if(i%2){//alternate setting x left or right while incrementing y by 5
          setX(left);
          setY(y);
        }else{
          setX(right);
          setY(y);
        }
        delay(de*1.6);
      }

    }
    void randomMoves(int moves){  //make N random movements
      for (int i = 0; i < moves; i++){
        int x1=random(minX,maxX);
        int y1=random(minY,maxY-25);
        setX(x1);
        setY(y1);
        delay(de*2);
      }      
    }
    void wiggle(){
      setY(30);
      for(int i = 0; i < 5; i++){
        setX(60);
        delay(de*1.2);
        setX(100);
        delay(de*1.2);
      }
    }
    void upDown(){
      int top = 40;
      int bottom = 5;
      for(int i = 0; i < 6; i++){
        setX(40+i*10);
        setY(bottom);
        delay(de*1.5);
        setY(top);
        delay(de*1.5);
      }
    }
};

void testShake(){       //used to test the servo motors
  s1.write(0);
  delay(1000);
  s1.write(180);
  delay(1000);
}

LaserPointer p;

void loop() {      
  if(trigger()){
    Serial.println("Trigger!");
    p.laserOn();
    p.setX(0);
    p.setY(0);
    delay(500);
    int c = 0;
    while(detectMotion()){        //continute activating if there is motion in front of the toy
      c++;
      p.randomMoves(random(2,4));
      int r = random(0,4);        //generate a random number to simulate random behavior
      switch(r){
        case 1:
          p.z();
          break;
        case 2:
          p.backAndForth();
          break;
        case 3:
          p.wiggle();
          break;
        case 4:
          p.upDown();
          break;
      }
    }
    p.laserOff();
  }
}