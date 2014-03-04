#pragma once
#include "windows.h"
#include "stdlib.h"
#include "stdio.h"
VOID MyDbgString(LPCSTR format,...);
void a2w(char *charstr,wchar_t *deswcharstr);
#define ERROR_CODE GetLastError()