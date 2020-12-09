#pragma once

#include "Debug.h"

//should probably reorganize some of these eventually so they're not global

static int unique_id;

extern float g_fixedDeltaTime;

extern int buffer_size;
extern ContainerManager<std::string> g_cBuffer;
extern ContainerManager<std::string> g_cBuffer_last;