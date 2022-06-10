#include <Arduino.h>
#include "MyBlink.h"
#include "MyTelegram.h"
#include "MyControl.h"

void setup()
{
  Serial.begin(115200);
  Serial.println();

  MyContorlInit();
  //MyTelegramInit();  
  digitalWrite(LED_PIN, HIGH);
  MyBlinkInit();// тут включено соединение с точкой доступа
  digitalWrite(LED_PIN, LOW);
}

void loop()
{
  MyBlinkRun();
  //TelegramTextMessage();

  Blink();
  Receive(); // Приём данных из ком порта
  Movement();
  ButtonControl();
}

/* https://patorjk.com/software/taag/#p=display&h=0&v=1&f=Crazy&t=WELCOME%20%0Amr%20Kurbatov%20
 __          __ ______  _       _____  ____   __  __  ______         
 \ \        / /|  ____|| |     / ____|/ __ \ |  \/  ||  ____|        
  \ \  /\  / / | |__   | |    | |    | |  | || \  / || |__           
   \ \/  \/ /  |  __|  | |    | |    | |  | || |\/| ||  __|          
    \  /\  /   | |____ | |____| |____| |__| || |  | || |____         
     \/  \/    |______||______|\_____|\____/ |_|  |_||______|        
                    _  __             _             _                
                   | |/ /            | |           | |               
  _ __ ___   _ __  | ' / _   _  _ __ | |__    __ _ | |_  ___ __   __ 
 | '_ ` _ \ | '__| |  < | | | || '__|| '_ \  / _` || __|/ _ \\ \ / / 
 | | | | | || |    | . \| |_| || |   | |_) || (_| || |_| (_) |\ V /  
 |_| |_| |_||_|    |_|\_\\__,_||_|   |_.__/  \__,_| \__|\___/  \_/   

*/