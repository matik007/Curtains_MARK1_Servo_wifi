#ifndef MY_TELEGRAM_h
#define MY_TELEGRAM_h

#include <Arduino.h>
#include "UniversalTelegramBot.h"
#include <WiFiClientSecure.h>  
#include <ESP8266WiFi.h>
#include "MyControl.h"

void PrintWiFiStatus();
void MyTelegramInit();
void handleNewMessagesButton(int numNewMessages);
void handleNewMessages(int numNewMessages);
void TelegramTextMessage();
void TelegramButton();

#endif