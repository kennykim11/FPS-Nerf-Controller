
/* 
Code by Kenny Kim
This is to make a PC FPS controller with a NERF blaster! Uses 1 Arduino Mini.

Wiring:
RAW - 
GND - GYRO - GND, THUMB - LEFT
RST - 
VCC - GYRO - VCC
A3 - RELOAD - IN
A2 - TRIGGER - IN
A1 - THUMB - RIGHT
A0 - THUMB - FARLEFT
15 - 
14 - SWITCH - HIGH
16 - SWITCH - INPUT
10 - SWITCH - LOW

TXD - 
RX1 - 
GND - THUMB - FARRIGHT
GND - TRIGGER - GND, RELOAD - GND
2 - GYRO - SDA
3 - GYRO - SCL
4 - SIDE - INPUT
5 - SIDE _ HIGH
6 - SIDE _ LOW
7 - GYRO - INT
8 - 
9 - 

References: 
https://maker.pro/education/imu-interfacing-tutorial-get-started-with-arduino-and-the-mpu-6050-sensor
"The Micro has a total of 12 analog inputs, pins from A0 to A5 are labelled directly on the pins and the 
other ones that you can access in code using the constants from A6 trough A11 are shared respectively on digital pins 4, 6, 8, 9, 10, and 12."
*/

// === IMPORTS ===
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Mouse.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
#include "Keyboard.h"
#include "Time.h"


MPU6050 accelgyro;
int16_t gx, gy, gz;

int16_t highest = 0;
int16_t lowest = 0;

int lastAim = 0;

void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
  //38400 chosen because it works as well at 8MHz as it does at 16MHz
  Serial.begin(38400);

  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();

  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  //Switch Pins
  pinMode(14, OUTPUT);
  digitalWrite(14, HIGH);
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW);
  pinMode(16, INPUT);

  //Side Pins
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
  pinMode(4, INPUT);
    
}



void loop() {
  // === GYRO ===
  //read raw accel/gyro measurements from device
  accelgyro.getRotation(&gx, &gy, &gz);

  /*
  //For finiding Max/Min of accelerometer
  if (gy > highest){highest = gy;}
  if (gy < lowest){lowest = gy;}
  Serial.print(gy); Serial.print("\t");
  Serial.print(highest); Serial.print("\t");
  Serial.print(lowest); Serial.println("\t");
  */
  //Min, Normal, Max
  //Still
  //0, 500, 800
  //Wild
  //-32768, 500, 32767
  if (digitalRead(16) == HIGH){
    if (gz > 1500 || gz < -1500){
      Serial.print(gz); Serial.print("\t");
      Serial.print((float)gz/32767*100); Serial.print("\t");
      Mouse.move(0, (float)gz/32767*100);
    }
    if (gy > 1500 || gy < -1500){
      Serial.print(gy); Serial.print("\t");
      Serial.print((float)gy/32767*100); Serial.println("\t");
      Mouse.move((float)gy/32767*100, 0);
    }
  }

  // === THUMBPAD ===
  int yVal = analogRead(A0);
  int xVal = analogRead(A1);
  if (yVal > 700){
    Keyboard.press('w');
  }
  else{
    Keyboard.release('w');
  }
  if (yVal < 300){
    Keyboard.press('s');
  }
  else{
    Keyboard.release('s');
  }
  if (xVal > 700){
    Keyboard.press('a');
  }
  else{
    Keyboard.release('a');
  }
  if (xVal < 300){
    Keyboard.press('d');
  }
  else{
    Keyboard.release('d');
  }

  // === RELOAD ===
  if (analogRead(A3) == 0){
    Keyboard.print('r');
  }

  // === FIRE ===
  else if (analogRead(A2) == 0){
    Mouse.click();
  }

  // === CALIBRATION SWITCH ===
  //018, 118
  if (lastAim != 0){
    if (lastAim >= 50){
      //Only occurs 250 ms after last left click
      lastAim = 0;
    }
    else{
      lastAim += 1;
    }
  }
  if (digitalRead(4) == HIGH){
    if (lastAim == 0){
      Mouse.click(MOUSE_RIGHT);
      lastAim = 1;
    }
  }

  
  delay(5);
}
