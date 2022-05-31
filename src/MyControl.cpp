#include "MyControl.h"

#define MAX_SERVO_SPEED_UP 0
#define MAX_SERVO_SPEED_DOWN 170

// Hall sensor to read position
#define UP_POSITION 600
#define DOWN_POSITION 490

bool blinkFlag = 0;
uint32_t lastBlinkTime = 0;

void Blink(){
  if(blinkFlag){
    if(millis() - lastBlinkTime >= 100){
      lastBlinkTime = millis();
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }
  }
}

uint8_t state = STOP;
uint8_t LAST_STATE;

volatile int16_t count = 0;
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
    if (buttUp.isSingle()) {
      Serial.println("Click up btn. stop");         // проверка на один клик

      if (state == UP || state == DOWN) {
        state = STOP;
      } else {
        Serial.println("Clicked Up Button. Go UP");
        state = UP;
      }
    }

    if (buttDown.isSingle()) {
      Serial.println("Click down btn. stop");         // проверка на один клик

      if (state == UP || state == DOWN) {
        state = STOP;
      } else {
        Serial.println("Clicked Down Button. Go DOWN");
        state = DOWN;
      }
    }

    if(buttUp.isTriple() || buttDown.isTriple()){
      state = CALIBRATION;
      Serial.println("3 click. stop");
    }

    if (buttUp.isHolded()) { // если кнопка удерживается
      state = UP;
      holdUpFlag = true;
      Serial.println("Hold UP btn");
    } else if (buttDown.isHolded()) { // если кнопка удерживается
      state = DOWN;
      holdDownFlag = true;
      Serial.println("Hold DOWN btn");
    }
  
  
    

    if (holdUpFlag || holdDownFlag) {
      if (buttUp.isRelease()) {
        state = STOP;
        holdUpFlag = false;
        Serial.println("holdUpFlag = false");
      }
      if (buttDown.isRelease()) {
        state = STOP;
        holdDownFlag = false;
        Serial.println("holdDownFlag = false");
      }
    }
  }
}

bool calibrationFlag = 0;

void Movement() {

// Если не калибровка и не удержание, но движение и если достигнуто максимальное занчение счётчика, стоп
  if(!calibrationFlag && !holdDownFlag && !holdUpFlag && (state == UP || state == DOWN)){
    if(count >= MAX_COUNT){
      Serial.println("count = max count");
      Serial.print("holdFlag = ");
      Serial.println(holdDownFlag);
      state = STOP;
    }
  }

  //Serial.println(count);
  if(LAST_STATE != state){
    LAST_STATE = state;
    switch (state) {
      case STOP:
        Serial.println("STOP");
        servo.write(90);
        servo.detach();
        if(calibrationFlag){ // если это была колибровка
          digitalWrite(LED_PIN, 0);
          blinkFlag = 0;
          calibrationFlag = 0;
          MAX_COUNT = count;
          Serial.print("MAX_COUNT = ");
          Serial.println(MAX_COUNT);
        }
        count = 0; // необходимо обнулять в любом случае 
        break;
      case DOWN:
        servo.attach(SERVO_PIN);
        Serial.println("DOWN");
        count = 0;
        // Плавный пуск для компинсации просадки напряжения
        for(int i = 90; i < MAX_SERVO_SPEED_DOWN; i++){
          servo.write(i);
          delay(1);
        }
      
        
        break;
      case UP:
        servo.attach(SERVO_PIN);
        Serial.println("UP");
        count = 0;
        // Плавный пуск для компинсации просадки напряжения
        for(int i = 90; i > MAX_SERVO_SPEED_UP; i--){
          servo.write(i);
          delay(1);
        }
      
        break;
      case CALIBRATION:
        Serial.println("CALIBRATION");
        blinkFlag = 1;
        calibrationFlag = 1;
        count = 0;
        break;
    }
  }
}