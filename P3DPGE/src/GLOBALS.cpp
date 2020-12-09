#include "GLOBALS.h"



bool GLOBAL_DEBUG = true;

float g_fixedDeltaTime;

int buffer_size = -1;
ContainerManager<std::string> g_cBuffer;
ContainerManager<std::string> g_cBuffer_last;