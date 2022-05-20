#include "MyMesh.h"

painlessMesh  mesh;
Scheduler userScheduler; // для контроля. Планировщик  для управления задачами

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage ); // Задача отвечающая за вызов функции sendMessage() каждую секунду пока работает программа



int nodeNumber = 2; // пока такой идентификатор для ноды

String readings; // строка для отправки
bool stateLed = LOW;

// Формируем JSON пакет
String GetReadings() {

  stateLed = !stateLed; // Инвертируем отправляемое состояние светодиода
  
  StaticJsonDocument<100> jsonSend; 

  jsonSend["node"] = nodeNumber;

  jsonSend["stateLed"] = stateLed;

  readings = ""; // очищаю буфер, иначе занfчения плюсуются

  serializeJsonPretty(jsonSend, readings);
   
  return readings;
 
}

void sendMessage() {
 
  String msg = GetReadings();
   
  mesh.sendBroadcast(msg);
 
}

// Обратные функции

// Выводим имя отправителя сообщения и содержимое сообщения (msg.c_str ()).
// Разбираем полученый пакет JSON

void receivedCallback( uint32_t from, String &msg ) {
 
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
   
  // Разбираем пакет Json
  StaticJsonDocument<100> jsonGet;
  deserializeJson(jsonGet, readings);

  JsonObject obj = jsonGet.as<JsonObject>();
  
  int node = obj[String("node")];
  bool recivedLedState = obj[String("stateLed")];
   
  Serial.print("Node: ");
  Serial.println(node);
  Serial.print("State: ");
  Serial.println(recivedLedState);
}

// Функция newConnectionCallback () выполняется всякий раз, когда к сети подключается новая нода. 
// Эта функция выводит идентификатор платы.
void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

// Функция changedConnectionCallback() вызывается всякий раз, когда кто-то подключается или отключается от сети.
void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

// Функция nodeTimeAdjustedCallback () выполнется, когда сеть изменяет время, и все ноды синхронизируются.
// Выводит смещение.
void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void MyMeshInit(){
    // выбираем типы сообщения отладки
  // установите перед функцией init() чтобы выдавались приветственные сообщения
  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); 
  mesh.setDebugMsgTypes( ERROR | STARTUP );  

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT ); // Инциализируем сеть
  // Запускаеам обратные функции (приём)
  mesh.onReceive(&receivedCallback); // что-то получили
  mesh.onNewConnection(&newConnectionCallback); // обнаруживаем новое подключение к сети
  mesh.onChangedConnections(&changedConnectionCallback); // кто-то подключился или отключился
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback); // фиксируем изменение времени и синхронезируемся

  // Запускаем планировщик
  userScheduler.addTask(taskSendMessage); // добавляем задачу
  taskSendMessage.enable(); // запускаем задачу
}

void MyMeshUpdate(){
  mesh.update();
}