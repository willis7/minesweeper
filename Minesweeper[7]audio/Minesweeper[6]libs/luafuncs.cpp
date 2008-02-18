#include "luafuncs.h"

/************************************************************
Creates a Lua state
**************************************************************/
void createLuaState(lua_State *pLuaState, char* filename)
{
	luaopen_base(pLuaState);										//basic functions
	luaopen_io(pLuaState);											//io functions


	if (luaL_loadfile(pLuaState,filename)|| lua_pcall(pLuaState,	0,		0,		0))
	{
																	//if either of the script files pushed an error it will be on the top of the stack
																	//note that the stack is emptied after each function call
		printf("%s\n",lua_tostring(pLuaState,-1));	

	}
}
/************************************************************
Function for lua to handle Y axis movement
**************************************************************/
int moveY ( int move, int currenty, lua_State *pLuaControl )
{
	int newPosY;
	
	lua_getglobal(pLuaControl, "moveY");		// the function name	
	lua_pushnumber(pLuaControl, move);			// the first argument	
	lua_pushnumber(pLuaControl, currenty);		// the second argument	
	lua_call(pLuaControl, 2, 1);				// call the function with 2 arguments, return 1 result

	newPosY = (int)lua_tonumber(pLuaControl, -1);	// get the result
	lua_pop(pLuaControl, 1);

	return newPosY;
}
/************************************************************
Function for lua to handle X axis movements
**************************************************************/
int moveX ( int move, int currentx, lua_State *pLuaControl )
{
	int newPosX;
	
	lua_getglobal(pLuaControl, "moveX");			//the function name 	
	lua_pushnumber(pLuaControl, move);				// the first argument
	lua_pushnumber(pLuaControl, currentx);			// the second argument	
	lua_call(pLuaControl, 2, 1);					//call the function with 2 arguments, return 1 result
	
	newPosX = (int)lua_tonumber(pLuaControl, -1);		// get the result
	lua_pop(pLuaControl, 1);

	return newPosX;
}

int* configScreen(void)												//Calls on Lua scripts to configure glut window		
{
	
	lua_State *pLuaConfig = lua_open();								// Init the Lua environment
	
	int iStackTop;													// Pointer to the top of the stack
	int* screen = new int(2);
	
																	//Load the libraries required into the lua environment
	luaopen_base(pLuaConfig);										//basic functions
	luaopen_io(pLuaConfig);											//io functions
																	// load the script into the lua environment
	if (luaL_loadfile(pLuaConfig,"scripts/config.lua")|| lua_pcall(pLuaConfig,	0,		0,		0))
	{
																	//if either of the script files pushed an error it will be on the top of the stack
																	//note that the stack is emptied after each function call
		printf("%s\n",lua_tostring(pLuaConfig,-1));	

	}//if
	else 
	{			
		//load graphical configuration properties
		lua_getglobal(pLuaConfig, "ScreenWidth");
		lua_getglobal(pLuaConfig, "ScreenHeight");	    

		screen[0] = (int)lua_tonumber(pLuaConfig, -2);	
		screen[1] = (int)lua_tonumber(pLuaConfig, -1);			
		
		iStackTop = lua_gettop(pLuaConfig);	

	}	
		
	lua_close(pLuaConfig);											//dispose of the lua state

	return screen;
}
/************************************************************
This is where I load in my table from player.lua
This function calls on tableTraverse() recursively 
**************************************************************/
void configPlayer(player &p)
{
	lua_State *pLuaPlayer = lua_open();

	// Load file.
    if(luaL_loadfile(pLuaPlayer, "scripts/player.lua") || lua_pcall(pLuaPlayer, 0, 0, 0))
    {
        cout << lua_tostring(pLuaPlayer,-1) << endl;
    }	

		// Print table contents.
		lua_getglobal(pLuaPlayer, "level1");
		tableTraverse(pLuaPlayer, p);

		 // Print player details.
		lua_getglobal(pLuaPlayer, "level1");
		lua_pushstring(pLuaPlayer, "player");
		lua_gettable(pLuaPlayer, -2);

		if(lua_isstring(pLuaPlayer, -1))
        cout << "Level1.Player = " << lua_tostring(pLuaPlayer, -1) << endl;
		p.playerName = (char)lua_tostring(pLuaPlayer, -1);



    lua_close(pLuaPlayer);

}
/************************************************************
This function allows me to recursively traverse tables to any depth.
Here i'm also checking to see if the key im currently on is Y or X so that 
i can store them correctly.
**************************************************************/
void tableTraverse(lua_State *pLuaPlayer, player &p)
{
	const char *tempKey;

	

	lua_pushnil(pLuaPlayer);


		while(lua_next(pLuaPlayer, -2) != 0)
		{
			if(lua_isstring(pLuaPlayer, -1)){													//Does the checking to see if the next item on stack is string
			  tempKey = lua_tostring(pLuaPlayer, -2);
			  printf("%s = %s\n", lua_tostring(pLuaPlayer, -2), lua_tostring(pLuaPlayer, -1));	//prints to the console the current information
			  if(_stricmp(tempKey, "Y")==0)														//Here were checking to see if the key is equivalent to "Y",
			  {																					//this then enables me to store the correct X & Y positions
				  p.StartPosY=(float) lua_tonumber(pLuaPlayer,-1);
			  }
					else if(_stricmp(tempKey,"X")==0)
					{
						p.StartPosX =(float) lua_tonumber(pLuaPlayer,-1);
					}
			}else if(lua_isnumber(pLuaPlayer, -1))					//Does the checking to see if the next item on stack is number
			  printf("%s = %d\n", lua_tostring(pLuaPlayer, -2), lua_tonumber(pLuaPlayer, -1));
			else if(lua_istable(pLuaPlayer, -1))
			  tableTraverse(pLuaPlayer, p);

			lua_pop(pLuaPlayer, 1);
		}
}
int randNumbGen(int d)
{
	int random;
	srand((unsigned)time(0));
    int lowest=1, highest=d; 
    int range=(highest-lowest)+1;

	random = (lowest+int(range*rand()/(RAND_MAX + 1.0)));
	return random;
}

static int randNumGlue(lua_State *pLuaNumb)
{
	int d = (int)lua_tonumber(pLuaNumb,1);		// get argument from the stack (lua should have pushed this!)
	lua_pushnumber(pLuaNumb, randNumbGen(d));	// push the result onto the stack (lua will get this)
	return 1;									// return the number of items pushed onto the stack. 
}

void initRandNum(char *filename)
{	
	lua_State *pLuaNumb = lua_open();			// initialize Lua
	lua_baselibopen(pLuaNumb);					// load Lua libraries	
	lua_register(pLuaNumb, "randNum", randNumGlue);// register our function	
	lua_dofile(pLuaNumb, filename);				// execute the script */
	lua_close(pLuaNumb);						// cleanup Lua
}