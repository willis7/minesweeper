#ifndef LUAFUNCS_H
#define LUAFUNCS_H

#include <string.h>
#include <iostream>
#include <ctime>
using namespace std;

extern "C" 
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

typedef struct player
{
	char playerName;
	float StartPosY;
	float StartPosX;
}player;

void createLuaState(lua_State *pLuaState, char* filename); 

int moveX(int dir, int currentx, lua_State *pLuaControl );
int moveY(int dir, int currenty, lua_State *pLuaControl );

int* configScreen (void);
void configPlayer(player &p);
void tableTraverse(lua_State *pLuaState, player &p);

int randNumbGen(int d);
static int randNumGlue(lua_State *pLuaNumb);
void initRandNum(char *filename);

#endif