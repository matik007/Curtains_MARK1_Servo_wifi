#include <Arduino.h>

#include <Servo.h>
#include <GyverButton.h>
#include "MyGlobal.h"
#include "MyMesh.h"

#define MAX_SERVO_SPEED_UP 0
#define MAX_SERVO_SPEED_DOWN 170

// Movement states
#define UP 1
#define DOWN 2
#define STOP 0
#define SPEED 255
uint8_t STATE = STOP;
uint8_t LAST_STATE;

// Hall sensor to read position
#define UP_POSITION 600
#define DOWN_POSITION 490

GButton buttUp(BTN_UP_PIN);
GButton buttDown(BTN_DOWN_PIN);

extern painlessMesh  mesh;

Servo servo;

void Receive();
void ButtonControl();
void Movement() ;
void BatteryCharge();

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(15, INPUT_PULLUP);
  
  buttUp.setType(LOW_PULL);
  buttDown.setType(LOW_PULL);

  MeshInit();
}

void loop() {
  if(digitalRead(15) == 0){
    servo.attach(SERVO_PIN);
        Serial.println("UP");
        // Плавный пуск для компинсации просадки напряжения
        for(int i = 90; i > MAX_SERVO_SPEED_UP; i--){
          servo.write(i);
          delay(2);
        }
        delay(500);
        servo.detach();
  }
  // mesh.update(); // чтобы сеть оставалась рабочей   
  Receive();
  //ButtonControl();
  Movement();
  //BatteryCharge();
}

void Receive() {
  if(Serial.available()){
    if (Serial.available()) { // принемаем команду для изменения состояния
      char data = Serial.parseInt();
      Serial.println(data);
      servo.write(data);
      servo.attach(SERVO_PIN);
      delay(1000);
      servo.detach();
      /*
      Serial.print("Принял: ");
      Serial.println(data);
      switch (data) {
        case 's':
          STATE = STOP;
          break;
        case 'a':
          STATE = UP;
          break;
        case 'd':
          STATE = DOWN;
          break;
      }
      */
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

      if (STATE == UP || STATE == DOWN) {
        STATE = STOP;
      } else {
        STATE = UP;
      }
    }

    if (buttDown.isClick()) {
      Serial.println("Click down btn");         // проверка на один клик

      if (STATE == UP || STATE == DOWN) {
        STATE = STOP;
      } else {
        STATE = DOWN;
      }
    }
  }
  if (buttUp.isHold()) { // если кнопка удерживается
    STATE = UP;
    holdUpFlag = true;
    Serial.println("Hold UP btn");
  }
  if (buttDown.isHold()) { // если кнопка удерживается
    STATE = DOWN;
    holdDownFlag = true;
    Serial.println("Hold DOWN btn");
  }

  if (holdUpFlag || holdDownFlag) {
    if (buttUp.isRelease()) {
      STATE = STOP;
      holdUpFlag = false;
    }
    if (buttDown.isRelease()) {
      STATE = STOP;
      holdDownFlag = false;
    }
  }
}

void Movement() {

  if(LAST_STATE != STATE){
    LAST_STATE = STATE;
    switch (STATE) {
      case STOP:
        Serial.println("STOP");
        servo.write(90);
        servo.detach();
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
    }
  }
}

void BatteryCharge(){
  digitalWrite(LED_PIN, !digitalRead(BATTERY_PIN));
}
