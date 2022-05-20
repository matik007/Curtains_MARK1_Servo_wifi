#ifndef MY_MESH_h
#define MY_MESH_h
#include <Arduino.h>
#include <painlessMesh.h>
#include "MyGlobal.h"

#define   MESH_PREFIX     "MDR"
#define   MESH_PASSWORD   "MakeDreamReal"
#define   MESH_PORT       5555 // Относится к TCP-порту, на котором долна работать ячеистая топология. По умолчанию 5555

void sendMessage() ;
String GetReadings(); 
void receivedCallback( uint32_t from, String &msg );
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback();
void nodeTimeAdjustedCallback(int32_t offset);

void MyMeshInit();
void MyMeshUpdate();

#endif