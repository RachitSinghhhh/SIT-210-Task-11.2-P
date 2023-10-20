// Including Servo Motor Library
#include <Servo.h>
// Including LiquidCrystal I2C Library
#include <LiquidCrystal_I2C.h>


// Defining the PINS
#define ULS_1ECHO 10
#define ULS_1TRIG 9
#define ULS_2ECHO 7
#define ULS_2TRIG 8
#define CONV_MOT_1 5
#define CONV_MOT_2 9
#define SERVO_1 2
#define SERVO_2 12
#define SERVO_3 11

//Color sensor
#define LED 13
#define S0 14
#define S1 15
#define S2 17
#define S3 16
#define sensorOut 20

//var color sensor
int Red=0,Blue=0,Green=0;

// Making objects of Servo Motor
Servo s1,s2,s3;

int DistanceIn;
int DistanceOut;
int countIn=0 , countOut=0;
bool flagOn = false;
int color=1;

LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {

// Attaching Objects with PINS
s1.attach(SERVO_1);
s2.attach(SERVO_2);
s3.attach(SERVO_3);


lcd.init();
lcd.backlight();
pinMode(ULS_1ECHO,INPUT);
pinMode(ULS_1TRIG,OUTPUT);
pinMode(ULS_2ECHO,INPUT);
pinMode(ULS_2TRIG,OUTPUT);
pinMode(CONV_MOT_1,OUTPUT);
pinMode(CONV_MOT_2,OUTPUT);
  

  //color sensor
pinMode(S0, OUTPUT);
pinMode(S1, OUTPUT);
pinMode(S2, OUTPUT);
pinMode(S3, OUTPUT);
pinMode(sensorOut, INPUT);
digitalWrite(S0, HIGH);
digitalWrite(S1, HIGH);

lcd.clear();
lcd.setCursor(0, 0);
lcd.print("**Conveyr Belt**");
Serial.begin(9600);
}

void loop() {
  startMotor();
  int R=0,G=0,B=0,D=0;
  DistanceIn = UltraDist(ULS_1TRIG, ULS_1ECHO);
  if(DistanceIn<5){
      lcd.clear();
      flagOn = true;
      countIn = countIn+1;
      lcd.setCursor(0,0);
      lcd.print("  IN  -  ");
      lcd.print(countIn);
      // startMotor();
      delay(3000);
      stopMotor();
      getColor();
      delay(1000);

    if(flagOn){
        if(color==1) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("      RED      ");
          R=1;
          startMotor();
          delay(3000);
          String msg = String(color)+","+String(countIn)+","+String(countOut);
          sendColorToRaspi(msg);
          stopMotor();
         
          delay(2000);  // Wait for the rasberry pi Servo motor Execution
          lcd.clear();
        }

      else if (color==2) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("     GREEN     ");
          G=1;
          startMotor();
          delay(6000);
          String msg = String(color)+","+String(countIn)+","+String(countOut);
          sendColorToRaspi(msg);
          stopMotor();
          
          delay(2000);  // Wait for the rasberry pi Servo motor Execution
          lcd.clear();
        }

      else if (color==3) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("     BLUE      ");
          B=1;
          startMotor();
          delay(9000);
          String msg = String(color)+","+String(countIn)+","+String(countOut);
          sendColorToRaspi(msg);
          stopMotor();
          
          delay(2000);  // Wait for the rasberry pi Servo motor Execution
          lcd.clear();
        }

      else {
        // Serial.print(color);
        lcd.clear();
        lcd.print("Other Colour");
        startMotor();
        D=1;
        delay(10000);
        // String msg = String(color)+","+String(countIn)+","+String(countOut);
        // sendColorToRaspi(msg);
        stopMotor();
        delay(1000);
        DistanceOut = UltraDist(ULS_2TRIG,ULS_2ECHO);
        if(DistanceOut<5){
        countOut=countOut+1;
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print("    OUT -  ");
        lcd.print(countOut);
        String msg = String(color)+","+String(countIn)+","+String(countOut);
        sendColorToRaspi(msg);
        flagOn=false;
        startMotor();
       
      } 
       
    }
      
  }
  // Serial.println("I "+String(countIn)+"-"+"R "+String(R)+"-"+"G "+String(G)+ "-" +"B " +String(B)+"-" +"D "+String(D)+"-"+"O "+String(countOut)); 
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Total Count-"+String(countIn));
  lcd.setCursor(0, 1);
  lcd.print("Other Color-"+String(countOut));
  delay(100);
  
  //Serial.println("I "+String(countIn)+"-"+"R "+String(0)+"-"+"G "+String(0)+ "-" +"B " +String(0)+"-" +"D "+String(1)+"-"+"O "+String(countOut));
}

int UltraDist(int USUP_TRIG , int USUP_ECHO ) {
  long duration, distance;
  digitalWrite(USUP_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(USUP_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(USUP_TRIG, LOW);
  duration = pulseIn(USUP_ECHO, HIGH);
  distance = (duration / 2) / 29.1;
  return distance;
}

void startMotor(){
  digitalWrite(CONV_MOT_1,HIGH);
  digitalWrite(CONV_MOT_2,HIGH);

}

void stopMotor(){
  digitalWrite(CONV_MOT_1,LOW);
  digitalWrite(CONV_MOT_2,LOW);
  color = getColor();
}

void GetColorsFreq()
{
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  Red = pulseIn(sensorOut, digitalRead(sensorOut) == HIGH ? LOW : HIGH);
  digitalWrite(S3, HIGH);
  Blue = pulseIn(sensorOut, digitalRead(sensorOut) == HIGH ? LOW : HIGH);
  delay(20);
  digitalWrite(S2, HIGH);
  Green = pulseIn(sensorOut, digitalRead(sensorOut) == HIGH ? LOW : HIGH);
  delay(20);
}

int getColor(){
  GetColorsFreq();
  //if  Red value is the lowest one and smaller 
  if (Red<Blue && Red<=Green && Red<20 && Green > 15){
    return 1;
  }

//Same thing for Blue
  else if (Blue<Green && Blue<Red && Blue<20){
    return 3;
  }

//Green it was a little tricky,  you can do it using the same method as above (the lowest), but here I used a reflective  object
//which means the  blue value is very low too, so I decided to check the difference between green and  blue and see if it's acceptable
  else if (Green<Red && Green-Blue<= 8){           
    return 2;
  }
  else if (Green<20 && Blue<20 && Red<15){
    return 0;
  }
  
  else{
    return 0;
  }
} 

void sendColorToRaspi(String msg){
  Serial.println(msg);
  // Serial.println("Color Send to Raspi");
}

