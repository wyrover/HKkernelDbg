#include "DbgOut.h"
VOID MyDbgString(LPCSTR format,...)
{
	va_list arglist;
	char buffer[1024];
	va_start(arglist,format);
	vsprintf_s(buffer,format,arglist);
	va_end(arglist);
	strcat_s(buffer,"\n");
	OutputDebugStringA(buffer);
}

void a2w(char *charstr,wchar_t *deswcharstr)
{
	int charlen=strlen(charstr);


	memset(deswcharstr,'\0',(charlen+1)*2);


	for(int i=0;i<charlen;i++)
	{
		*((char *)deswcharstr+i*2)=charstr[i];
	}

}