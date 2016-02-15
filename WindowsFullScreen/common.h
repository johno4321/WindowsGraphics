#pragma once

#include <windows.h>
#include <stdint.h>

typedef enum { false, true }	qboolean;

#define MAX_NUM_ARGVS 50

typedef uint8_t uint8;
typedef uint32_t uint32;

typedef int8_t int8;
typedef int32_t int32;

extern int32 BufferHeight;
extern int32 BufferWidth;

extern int32 com_argc;
extern uint8 *com_argv[MAX_NUM_ARGVS + 1];

void COM_ParseCmdLine(uint8 *lpCmdLine);
int32 COM_CheckParm(uint8 *parm);