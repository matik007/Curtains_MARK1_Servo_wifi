#include "MyBlink.h"
#include <BlynkSimpleEsp8266.h>

char auth[] = "o5UnUMJsSzXEpz3BFp8zvE9F_NvnB2pm"; //тут токен из e-mail для Blink

// Замените на свои сетевые данные
#define WIFI_SSID "Maga_EnoT"
#define WIFI_PASSWORD "01871086"

void MyBlinkInit(){
    Blynk.begin(auth, WIFI_SSID, WIFI_PASSWORD); // тут включено соединение с точкой доступа
}

void MyBlinkRun(){
    Blynk.run();
}



BLYNK_WRITE(V0) // функция, отслеживающая изменение виртуального пина 0 управляет шторами
{
  int pinValue = param.asInt();  //переменная текущего состояния виртуального пина
  Serial.print("V0: ");
  Serial.println(pinValue);
  if(pinValue == 1){
    state = UP;
    Serial.println("Telegram UP");
  } else {
    state = DOWN;
    Serial.println("Telegram DOWN");
  }
}