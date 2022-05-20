#include "MyTelegram.h"

// Инициализация Telegram бота
#define BOT_TOKEN "1775831329:AAH0PUu4X1VPID4sseGA-R7Pb6yB8yCM9T0"  // Ваш Токен
#define CHAT_ID "1034615273"     

const unsigned long BOT_MTBS = 1000; // среднее время между сообщениями сканирования

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

unsigned long bot_lasttime = 0; // последнее сканирование сообщений было сделано

int Relay1Status = 0;       //Переменная для хранения статуса Реле1

int delayBetweenChecks = 1000;      // среднее время между сообщениями сканирования
unsigned long lastTimeChecked;          // когда было сделано последнее сканирование сообщений


unsigned long lightTimerExpires;
boolean lightTimerActive = false;

void MyTelegramInit(){
      // attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");         // Сервер точного времени UTC NTP
  secured_client.setTrustAnchors(&cert);    // Добавляем сертификат api.telegram.org
}

void handleNewMessagesButton(int numNewMessages) {

  for (int i = 0; i < numNewMessages; i++) {

    // If the type is a "callback_query", a inline keyboard button was pressed
    if (bot.messages[i].type ==  F("callback_query")) {
      String text = bot.messages[i].text;
      Serial.print("Кнопка: ");
      Serial.println(text);

      if (text == F("ON")) {
        Serial1.write('0');
        Serial.write('0');
     // digitalWrite(Relay1, LOW);  // включить Свет (НИЗКИЙ уровень реле)
      Relay1Status = 1;           // меняем статус на 1
      } else if (text == F("OFF")) {
        Serial1.write('1');
        Serial.write('1');
     // digitalWrite(Relay1, LOW);  // включить Свет (НИЗКИЙ уровень реле)
      Relay1Status = 0;           // меняем статус на 1
      } else if (text.startsWith("TIME")) {
        text.replace("TIME", "");
        int timeRequested = text.toInt();
        
        Serial1.write('1');
        lightTimerActive = true;
        lightTimerExpires = millis() + (timeRequested * 1000 * 60);
      }
    } else {
      String chat_id = String(bot.messages[i].chat_id);
      String text = bot.messages[i].text;

      String from_name = bot.messages[i].from_name;
      if (from_name == "")
        from_name = "Guest";
        
        if (text == "/start")
        {
          String keyboardJson = F("[[{ \"text\" : \"перейти на мой канал\", \"url\" : \"https://youtube.com/ардуинодляначинающих\" }],");
                 keyboardJson += F("[{ \"text\" : \"ВКЛ\", \"callback_data\" : \"ON\" },{ \"text\" : \"ВЫКЛ\", \"callback_data\" : \"OFF\"}],");
                 keyboardJson += F("[{ \"text\" : \"1 Mин\", \"callback_data\" : \"TIME1\" }, { \"text\" : \"2 Mин\", \"callback_data\" : \"TIME2\" }, { \"text\" : \"10 Mин\", \"callback_data\" : \"TIME10\" }]]");
          bot.sendMessageWithInlineKeyboard(chat_id, "Выберите любую кнопку", "", keyboardJson);
        }
    }
  }
}


void handleNewMessages(int numNewMessages)
{
  Serial.print("обработка Новых Сообщений ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/up")
    {
      state = UP;   
      bot.sendMessage(chat_id, "Открываем", "");
    }

    if (text == "/down")
    {
      state = DOWN;   
      bot.sendMessage(chat_id, "Открываем", "");
    }

    if (text == "/stop")
    {
      state = STOP;   
      bot.sendMessage(chat_id, "Остановка", "");
    }
     
    
    if (text == "/status")
    {
      if (Relay1Status)bot.sendMessage(chat_id, "Свет включен", "");
      else bot.sendMessage(chat_id, "Свет выключен", "");
    }

    /*if (text == "/start")
    {
      String welcome = "Добро пожаловать, " + from_name + ".\n";
      welcome += "Включить-выключить фонарики.\n\n";
      welcome += "------------------------------\n";
      welcome += "/on : включить фонарики\n";
      welcome += "------------------------------\n";
      welcome += "/off : выключить фонарики\n";
      welcome += "------------------------------\n";
      welcome += "/status : показывает включено или выключено\n";
      welcome += "------------------------------\n";
      bot.sendMessage(chat_id, welcome, "");
    }*/
  }
}

void TelegramTextMessage(){
 if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages)
    {
      Serial.println("Ответ получен");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}

void PrintWiFiStatus(){
  Serial.print("Wi-Fi status: ");
  byte wifiStatus = WiFi.status();
  switch(wifiStatus){
    case WL_CONNECTED:
    Serial.println("Подключение выполнено успешно");
      break;
    case WL_NO_SSID_AVAIL:
    Serial.println("Заданный SSID находится вне зоны доступа");
      break;
    case WL_CONNECT_FAILED:
    Serial.println("Неправильный пароль");
      break;
    case WL_IDLE_STATUS:
    Serial.println("WiFi-сеть переключается с одного статуса на другой");
      break;
    case WL_DISCONNECTED:
    Serial.println("Модуль не находится в режиме станции");
      break;
    default:
      Serial.println(wifiStatus);
  }
}


void TelegramButton(){
  if (millis() > lastTimeChecked + delayBetweenChecks)  {

    

    //  возвращает 1 если есть новое сообщение от Telegram
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    if (numNewMessages) {
      Serial.println("got response");
      handleNewMessagesButton(numNewMessages);
    }

    lastTimeChecked = millis();

  }
}