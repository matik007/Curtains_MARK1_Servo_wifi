#include "MyControl.h"

#define MAX_SERVO_SPEED_UP 0
#define MAX_SERVO_SPEED_DOWN 170

// Hall sensor to read position
#define UP_POSITION 600
#define DOWN_POSITION 490

uint8_t state = STOP;
uint8_t LAST_STATE;

volatile int16_t count = 0;
int16_t MAX_COUNT = 0;

bool holdUpFlag = 0, holdDownFlag = 0;
bool calibrationFlag = 0;

GButton buttUp(BTN_UP_PIN);
GButton buttDown(BTN_DOWN_PIN);

Servo servo;


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

void MyContorlInit(){
    pinMode(LED_PIN, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);
    pinMode(BATTERY_PIN, INPUT);

    buttUp.setType(LOW_PULL);
    buttDown.setType(LOW_PULL);
    attachInterrupt(SENSOR_PIN, MyInterupt, CHANGE);
}

IRAM_ATTR void  MyInterupt(){
  if (calibrationFlag){
    count++;
  } else if(!holdDownFlag && !holdUpFlag){
    if(state == UP){
      count++;
    } else if(state == DOWN){
      count--;
    }
  }
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



void ButtonControl(){
  buttUp.tick();
  buttDown.tick();

// Проверка на один клик
  if (!holdUpFlag || !holdDownFlag) {
    if (buttUp.isSingle()) {
      if (state == UP || state == DOWN) {
        state = STOP;
        Serial.println("Click Up btn. stop");
      } else {
        Serial.println("Clicked Up Button. Go UP");
        state = UP;
      }
    }
    if (buttDown.isSingle()) {
      

      if (state == UP || state == DOWN) {
        state = STOP;
        Serial.println("Click Down btn. stop");
      } else {
        Serial.println("Clicked Down Button. Go DOWN");
        state = DOWN;
      }
    }

// Проверка на тройной клик
    if(buttUp.isTriple()){
      state = CALIBRATION;
      Serial.println("3 click. stop");
    }

// Проверка на удержание
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



void Movement() {

// Если не калибровка и не удержание, но движение и если достигнуто максимальное занчение счётчика, стоп
  if(!calibrationFlag && !holdDownFlag && !holdUpFlag && (state == UP || state == DOWN)){
    
    if(count >= MAX_COUNT && state == UP){
      state = STOP;
    } else if(count <= 0 && state == DOWN){
      state = STOP;
    }
  }

  //Serial.println(count);
  if(LAST_STATE != state){
    LAST_STATE = state;
    switch (state) {
      case STOP:
        Serial.println("STOP");
        Serial.print("count = ");
        Serial.println(count);
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
        break;
      case DOWN:
        servo.attach(SERVO_PIN);
        Serial.println("DOWN");
        // Плавный пуск для компинсации просадки напряжения
        for(int i = 90; i < MAX_SERVO_SPEED_DOWN; i++){
          servo.write(i);
          delay(1);
        }
      
        
        break;
      case UP:
        servo.attach(SERVO_PIN);
        Serial.println("UP");
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