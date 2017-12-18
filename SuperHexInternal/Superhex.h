#pragma once

struct wall_t
{
	int section;
	int distance;
	int width;
	int mystery;
	int enabled;
};

struct gamestate_t
{
	char pad00[20];
	char bumbo;
	char pad0001[16];
	char upArrowState;
	char downArrowState;
	char leftArrowState;
	char rightArrowState;
	char spacebarState;
	char keyState1;
	char keyState2;
	char keyState3;
	char keyState4;
	char keyState5;
	char keyState6;
	char keyState7;
	char keyState8;
	char keyState9;
	char keyState10;
	char escapeKeyState;
	char cpad0;
	char cpad1;
	char cpad2;
	char gameover;
	char pad0[345];
	int gameOverTime;
	int pad01;
	int baseRotation;
	int rotationRadius;
	char oscillator;
	__int16 oscillator2;
	int oscillatorState;
	int axisCount;
	char pad001[36];
	int playerSection;
	char pad0012[8];
	int playerAbsRotation;
	int absRotationUpdated;
	char pad020[40];
	wall_t walls[499];
	char pad021[20];
	int wallCnt;
	char pad022[20];
	int wallSpeed;
	char pad023[8];
	int playerRotation;
	int playerRotation2;
	int velocity;
	int pad;
	int gameover2; // immediate one
	int stage; // permanent one also it is fuckin weird
	char pad03[28];
	int time;
	int rotationRadiusAdjust;
	char pad1[18];
	int prevRank;
	int rank;
};


struct gamestate_sub
{
	char pad02[5812];
};

struct gamestate_sub2
{
	char pad[3776];
	int gameoverSomething;
	char pad2[35];
	int timeRanks[6];
};


struct CSuperhex
{
	virtual int initializer(char) = 0;
	virtual int vj_nullsub_97(int, int, int) = 0;
	virtual int sub_4394F0(int a2, int a3, int a4, int a5, int a6, int a7) = 0;
	virtual int nullsub_97(int, int, int) = 0;
	virtual int OnGameStart() = 0;
	virtual int MainLoop() = 0;
	virtual void UpdateRotation() = 0;
	virtual int nullsub_1() = 0;
	virtual int sub_4400D0(int, int) = 0;
	virtual int onButtonDown(int a2) = 0;
	virtual int onButtonUp(int a2) = 0;
	virtual int nullsub_98(int, int) = 0;
	virtual int nullsub_99(int, int, int) = 0;
	virtual signed int setThangs(int a2, int a3, signed int a4) = 0;
	virtual int setThangsToZero(int, int, int) = 0;
	virtual int nullsub_1_repeat() = 0;
	virtual void deinit(int a1, int a2, int a3, char a4, int a5, int a6, int a7) = 0;
	virtual void deinit2(int a1, int a2, int a3, int a4, int a5, int a6, int a7) = 0;
	virtual int nullsub_96(int) = 0;

	char pad00[12];
	gamestate_t gamestate;
	char pad20[7276];
	gamestate_sub2 substate2;
	char pad30[48344];
	gamestate_sub substate;
	char pad99[196536];
	char buttonStates[512];
};

// glut keycodes: https://github.com/google/liquidfun/blob/master/freeglut/include/GL/freeglut_std.h
enum keyids
{
	// ascii table up to 255, including TAB,CRLF,ESC and DEL; these are the regular keycodes handled by the callback designated in glutKeyboardFunc/glutKeyboardUpFunc
	SPECIAL = 0x100, // special modifier bit
	// special keycodes are 257 and up, callback designated by glutSpecialFunc/glutSpecialUpFunc
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	LeftArrow = 0x0064|0x100,
	UpArrow,
	RightArrow,
	DownArrow,
	PageUp,
	PageDown,
	Home,
	End,
	Insert,
};
