#ifndef MY_CONTROL_H
#define MY_CONTROL_H

#include <Arduino.h>
#include <Servo.h>
#include <GyverButton.h>

// Movement states
#define STOP 0
#define UP 1
#define DOWN 2
#define CALIBRATION 3

extern uint8_t state;

void MyContorlInit();
IRAM_ATTR void  MyInterupt();
void Receive();
void ButtonControl();
void Movement() ;
void BatteryCharge();

#endif