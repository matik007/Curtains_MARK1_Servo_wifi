/*
 * Скетч-пример, демонстрирующий,
 * как перевести ESP8266 в режим глубокого сна
 * Автор – Руи Сантос 
 * Более подробно о проекте на: http://randomnerdtutorials.com
 */

#include <Arduino.h>
 
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Ждем инициализации последовательной коммуникации:
  while(!Serial) { }
  
  // Переводим ESP8266 в режим глубокого сна на 30 секунд;
  // ESP8266 будет пробуждать себя сама,
  // пока контакт GPIO16 (D0 на плате NodeMCU)
  // будет подключен к контакту RST:
  Serial.println("I'm awake, but I'm going into deep sleep mode for 1 seconds");
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
             //  "Я проснулся, но перехожу 
             //   в режим глубокого сна на 30 секунд"
  ESP.deepSleep(1e6); 
  
  // Режим глубокого сна будет длиться, 
  // пока на контакт RST не будет подан сигнал «LOW»
  // (например, с помощью кнопки или геркона):
  //Serial.println("I'm awake, but I'm going into deep sleep mode until RESET pin is connected to a LOW signal");
               //  "Я проснулся, но перейду в режим глубокого сна,
               //   и буду там находиться,
               //   пока на контакт RST не будет подан сигнал «LOW»"
  //ESP.deepSleep(0); 
}

void loop() {
}