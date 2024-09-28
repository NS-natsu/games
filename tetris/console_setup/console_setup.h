#ifndef _CONSOLE_SETUP_H_
	#define _CONSOLE_SETUP_H_
	#include <windows.h>

	#define CLEAR_CONSOLE do{printf("\033c");}while(0)

	typedef struct {
		HANDLE console;
		DWORD mode;
		CONSOLE_SCREEN_BUFFER_INFO csbi;

		int chcp;

		int(*beginConsoleSettings)(void*, int, int);
		void(*endConsoleSettings)(void*);
	} CONSOLE_INFO;

	#ifndef _CONSOLE_SETUP_C_
		extern CONSOLE_INFO cInfo;
	#else
		#include <stdio.h>
		#include <stdlib.h>
		#include <unistd.h>

		static void _end(void*);
		static int _begin(void*,int,int);
		CONSOLE_INFO cInfo = {NULL, 0, {0,0}, 932, _begin, _end};

		static void setConsoleSize(CONSOLE_INFO*,int,int);
	#endif
#endif