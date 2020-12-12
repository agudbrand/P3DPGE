#include "GLOBALS.h"



bool GLOBAL_DEBUG = true;

float g_fixedDeltaTime;
float g_totalTime;

int screenHeight;
int screenWidth;

Vector3 g_MouseWorldPos;

int buffer_size = 0;
ContainerManager<std::string> g_cBuffer;
ContainerManager<std::string> g_cBuffer_last;