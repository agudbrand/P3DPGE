#pragma once

#include "Debug.h"
#include "../math/Vector3.h"

//should probably reorganize some of these eventually so they're not global

static int unique_id;

extern float g_fixedDeltaTime;
extern float g_totalTime;

extern int screenHeight;
extern int screenWidth;

extern Vector3 g_MouseWorldPos;

//master buffer size indicator
//this is VERY prone to errors so probably implement a better way to handle it 
//maybe let console manager handle it 
extern int buffer_size;
extern ContainerManager<std::string> g_cBuffer;
extern ContainerManager<std::string> g_cBuffer_last;