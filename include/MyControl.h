#ifndef MY_CONTROL_H
#define MY_CONTROL_H

#include <Arduino.h>
#include <Servo.h>
#include <GyverButton.h>

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

// Movement states
#define STOP 0
#define UP 1
#define DOWN 2
#define CALIBRATION 3

extern uint8_t state;

void Blink();
void MyContorlInit();
IRAM_ATTR void  MyInterupt();
void Receive();
void ButtonControl();
void Movement() ;
void BatteryCharge();

#endif