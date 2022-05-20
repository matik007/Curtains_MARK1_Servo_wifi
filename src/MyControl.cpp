#include "MyControl.h"

// Servo pin
#define SERVO_PIN 12
//Buttons pins
#define BTN_UP_PIN 14
#define BTN_DOWN_PIN 5
// LED
#define LED_PIN 13
// Sensor input
#define SENSOR_PIN 4
// Battary 
#define BATTERY_PIN A0

#define MAX_SERVO_SPEED_UP 0
#define MAX_SERVO_SPEED_DOWN 170

// Hall sensor to read position
#define UP_POSITION 600
#define DOWN_POSITION 490


uint8_t state = STOP;
uint8_t LAST_STATE;

int16_t count = 0;
int16_t MAX_COUNT = 0;

GButton buttUp(BTN_UP_PIN);
GButton buttDown(BTN_DOWN_PIN);

Servo servo;

void MyContorlInit(){
    pinMode(LED_PIN, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);
    pinMode(BATTERY_PIN, INPUT);

    buttUp.setType(LOW_PULL);
    buttDown.setType(LOW_PULL);
    attachInterrupt(SENSOR_PIN, MyInterupt, RISING);
}

IRAM_ATTR void  MyInterupt(){
  count++;
}

void Blink(uint16_t sp){

}

void Receive() {
  if(Serial.available()){
    if (Serial.available()) { // принемаем команду для изменения состояния
      char data = Serial.read();
      Serial.println(data);
      Serial.print("Принял: ");
      Serial.println(data);
      switch (data) {
        case 's':
          state = STOP;
          break;
        case 'a':
          state = UP;
          break;
        case 'd':
          state = DOWN;
          break;
        case 'c':
          state = CALIBRATION;
          break;
      }
    }
  }
}

bool holdUpFlag = 0, holdDownFlag = 0;

void ButtonControl(){
  buttUp.tick();
  buttDown.tick();

  if (!holdUpFlag || !holdDownFlag) {
    if (buttUp.isClick()) {
      Serial.println("Click up btn");         // проверка на один клик

      if (state == UP || state == DOWN) {
        state = STOP;
      } else {
        state = UP;
      }
    }

    if (buttDown.isClick()) {
      Serial.println("Click down btn");         // проверка на один клик

      if (state == UP || state == DOWN) {
        state = STOP;
      } else {
        state = DOWN;
      }
    }
  }
  if(buttUp.isHold() || buttDown.isHold()){
    if(buttUp.isHold() && buttDown.isHold()){
      holdUpFlag = true;
      holdDownFlag = true;
      state = CALIBRATION;
    } else if (buttUp.isHold()) { // если кнопка удерживается
      state = UP;
      holdUpFlag = true;
      Serial.println("Hold UP btn");
    } else if (buttDown.isHold()) { // если кнопка удерживается
      state = DOWN;
      holdDownFlag = true;
      Serial.println("Hold DOWN btn");
    }
    

    if (holdUpFlag || holdDownFlag) {
      if (buttUp.isRelease()) {
        state = STOP;
        holdUpFlag = false;
      }
      if (buttDown.isRelease()) {
        state = STOP;
        holdDownFlag = false;
      }
    }
  }
}

bool calibrationFlag = 0;

void Movement() {
  Serial.println(count);
  if(LAST_STATE != state){
    LAST_STATE = state;
    switch (state) {
      case STOP:
        Serial.println("STOP");
        servo.write(90);
        servo.detach();
        if(calibrationFlag){
          calibrationFlag = 0;
          MAX_COUNT = count;
          Serial.print("MAX_COUNT = ");
          Serial.println(MAX_COUNT);
        }
        break;
      case DOWN:
        servo.attach(SERVO_PIN);
        Serial.println("DOWN");
        // Плавный пуск для компинсации просадки напряжения
        for(int i = 90; i < MAX_SERVO_SPEED_DOWN; i++){
          servo.write(i);
          delay(2);
        }
        
        break;
      case UP:
        servo.attach(SERVO_PIN);
        Serial.println("UP");
        // Плавный пуск для компинсации просадки напряжения
        for(int i = 90; i > MAX_SERVO_SPEED_UP; i--){
          servo.write(i);
          delay(2);
        }
        break;
      case CALIBRATION:
        Serial.println("CALIBRATION");
        calibrationFlag = 1;
        count = 0;
        break;
    }
  }
}