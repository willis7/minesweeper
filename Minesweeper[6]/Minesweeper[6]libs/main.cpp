#include "luafuncs.h"
#include "gamefuncs.h"
#include <ctime>
#include <stdlib.h>
#include <glut.h>

#include <al.h>
#include <alc.h>
#include <alu.h>
#include <alut.h>

static int startX;
static int startY;

static int win;
static int menid;
static int primitive = 0;

const int ROWMAX = 16;
const int COLMAX = 16;

char mines[ROWMAX][COLMAX] = 
{
	{'B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','B'},
	{'B',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','F','B'},
	{'B','B','B','B','B','B','B','B','B','B','B','B','B','B','B','B'}
};
// Audio Varaiables
#define NUM_BUFFERS 3
#define NUM_SOURCES 3

#define LOOP 0
#define MINE 1
#define WARNING 2

ALuint Buffers[NUM_BUFFERS];
ALuint Sources[NUM_SOURCES];
ALfloat SourcePos[NUM_SOURCES][3];
ALfloat SourceVel[NUM_SOURCES][3];
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}
/************************************************************
Mine generator uses a random number generator that
**************************************************************/
void mineGenerator(int count)
{	
	 srand((unsigned)time(0)); 
    int randomX, randomY; 
    int lowest=1, highest=14; 
    int range=(highest-lowest)+1; 

    for(int index=0; index<count; index++)
	{ 
        randomX = lowest+int(range*rand()/(RAND_MAX + 1.0)); 
		randomY = lowest+int(range*rand()/(RAND_MAX + 1.0));
		
		if(mines[randomX][randomY] != 'B')						//Stops mines being placed where a wall is
			mines[randomX][randomY] = '*';
    }
}
/************************************************************
Using the array given at the start of the program, the boundary is drawn
**************************************************************/
void printMaze()												//Handles the drawing of the maze using polygons					
{
	for(int row = 0; row < ROWMAX;row++)
	{
		for(int col = 0; col < COLMAX;col++)
		{		
			if(mines[row][col] == 'B')
			{		
				glPushMatrix();
						glColor3f(0.0f, 1.0f, 0.0f);
						glTranslatef(col, row, 0.0);
						glutSolidCube(1.0f);
				glPopMatrix();

				
			}
			else if(mines[row][col] == 'F')
			{
				glBegin(GL_QUADS);		

				glColor3f(0.0f, 0.0f, 1.0f);

				glVertex3f(col+0.5,row+0.5,1.0f);				// Top Right Of The Quad (Bottom)
				glVertex3f(col-0.5,row+0.5,1.0f);					// Top Left Of The Quad (Bottom)
				glVertex3f(col-0.5,row-0.5,1.0f);						// Bottom Left Of The Quad (Bottom)
				glVertex3f(col+0.5,row-0.5,1.0f);					// Bottom Right Of The Quad (Bottom)

				glEnd();
			}
		}
	}
}
/************************************************************
This is where all of glutDisplayFunc() is handled
**************************************************************/
void drawScene()
{
									
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	ALint play;
	printMaze();
	
	
	glPushMatrix();	
		glTranslatef(startX,startY,0.0f);
		if(danger(mines,startX,startY) == true)
		{
			glColor3f(1.0f,0.0f,0.0f);
			alGetSourcei(Sources[2], AL_SOURCE_STATE, &play);
            alSourcefv(Sources[2], AL_POSITION, SourcePos[2] );
            alSourcePlay(Sources[2]);
				
		}else
		{
			glColor3f(1.0f,1.0f,0.0f);
		}
		
		glutSolidSphere(0.5,20,20);
	glPopMatrix();
	

		if(primitive == 1)
		{
			for(int row = 0; row < ROWMAX;row++)
			{
				for(int col = 0; col < COLMAX;col++)
				{		
					if(mines[col][row] == '*')
					{
						glPushMatrix();
							glBegin(GL_QUADS);
							glColor3f(1.0f, 0.0f, 1.0f);
							glVertex3f(col+0.5,row+0.5,1.0f);				// Top Right Of The Quad (Bottom)
							glVertex3f(col-0.5,row+0.5,1.0f);					// Top Left Of The Quad (Bottom)
							glVertex3f(col-0.5,row-0.5,1.0f);						// Bottom Left Of The Quad (Bottom)
							glVertex3f(col+0.5,row-0.5,1.0f);					// Bottom Right Of The Quad (Bottom)
							glEnd();
						glPopMatrix();

					}
				}
			}
		}

	glutSwapBuffers();

}
/************************************************************
ESC key press will cause program to exit
**************************************************************/
void handleKeypress(unsigned char key,int x,int y)					
{
		if(key == 27)
			exit(0);
}
/************************************************************
All arrow keypresses are handled in this function
**************************************************************/
void handleSpecialKeys(int key, int x, int y)							
{
	
	lua_State *pLuaControl = lua_open();

	createLuaState(pLuaControl, "scripts/control.lua");
	
		switch(key) 
		{
			case GLUT_KEY_LEFT :
					if(mines[startX-1][startY]!= 'B')
					startX = moveX(1,startX,pLuaControl);				// passing 1 tells Lua the function its a "-"
					break;
			case GLUT_KEY_RIGHT : 
					if(mines[startX+1][startY]!= 'B')
					startX = moveX(2,startX,pLuaControl);				// passing 2 tells Lua the function its a "+"
					break;
			case GLUT_KEY_DOWN : 
					if(mines[startX][startY-1]!= 'B')
					startY = moveY(1,startY,pLuaControl);
					break;
			case GLUT_KEY_UP : 
					if(mines[startX][startY+1]!= 'B')
					startY = moveY(2,startY,pLuaControl);
					break;
		}

		glutPostRedisplay();
}
ALboolean initOpenAL()
{
    ALCdevice* pDevice;
    ALCcontext* pContext;
    ALCubyte* deviceSpecifier;
    ALCubyte deviceName[] = "DirectSound3D";

    // Get handle to device.
    pDevice = alcOpenDevice(deviceName);

    // Get the device specifier.
    deviceSpecifier = alcGetString(pDevice, ALC_DEVICE_SPECIFIER);

    // Create audio context.
    pContext = alcCreateContext(pDevice, NULL);

    // Set active context.
    alcMakeContextCurrent(pContext);

    // Check for an error.
    if (alcGetError(pDevice) != ALC_NO_ERROR)
        return AL_FALSE;

    return AL_TRUE;
}

void exitOpenAL()
{
    ALCcontext* pCurContext;
    ALCdevice* pCurDevice;

    // Get the current context.
    pCurContext = alcGetCurrentContext();

    // Get the device used by that context.
    pCurDevice = alcGetContextsDevice(pCurContext);

    // Reset the current context to NULL.
    alcMakeContextCurrent(NULL);

    // Release the context and the device.
    alcDestroyContext(pCurContext);
    alcCloseDevice(pCurDevice);
}
ALboolean loadALData()
{
    // Variables to load into.

    ALenum format;
    ALsizei size;
    ALvoid* data;
    ALsizei freq;
    ALboolean loop;

    // Load wav data into buffers.

    alGenBuffers(NUM_BUFFERS, Buffers);

    if (alGetError() != AL_NO_ERROR)
        return AL_FALSE;

    alutLoadWAVFile("Data/loop.wav", &format, &data, &size, &freq, &loop);
    alBufferData(Buffers[LOOP], format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);

    alutLoadWAVFile("Data/minestep.wav", &format, &data, &size, &freq, &loop);
    alBufferData(Buffers[MINE], format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);

    alutLoadWAVFile("Data/alarm001.wav", &format, &data, &size, &freq, &loop);
    alBufferData(Buffers[WARNING], format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);

    // Bind buffers into audio sources.

    alGenSources(NUM_SOURCES, Sources);

    if (alGetError() != AL_NO_ERROR)
        return AL_FALSE;

    alSourcei (Sources[LOOP], AL_BUFFER,   Buffers[LOOP]  );
    alSourcef (Sources[LOOP], AL_PITCH,    1.0              );
    alSourcef (Sources[LOOP], AL_GAIN,     1.0              );
    alSourcefv(Sources[LOOP], AL_POSITION, SourcePos[LOOP]);
    alSourcefv(Sources[LOOP], AL_VELOCITY, SourceVel[LOOP]);
    alSourcei (Sources[LOOP], AL_LOOPING,  AL_TRUE          );

    alSourcei (Sources[MINE], AL_BUFFER,   Buffers[MINE]  );
    alSourcef (Sources[MINE], AL_PITCH,    1.0            );
    alSourcef (Sources[MINE], AL_GAIN,     1.0            );
    alSourcefv(Sources[MINE], AL_POSITION, SourcePos[MINE]);
    alSourcefv(Sources[MINE], AL_VELOCITY, SourceVel[MINE]);
    alSourcei (Sources[MINE], AL_LOOPING,  AL_FALSE       );

    alSourcei (Sources[WARNING], AL_BUFFER,   Buffers[WARNING]  );
    alSourcef (Sources[WARNING], AL_PITCH,    1.0            );
    alSourcef (Sources[WARNING], AL_GAIN,     1.0            );
    alSourcefv(Sources[WARNING], AL_POSITION, SourcePos[WARNING]);
    alSourcefv(Sources[WARNING], AL_VELOCITY, SourceVel[WARNING]);
    alSourcei (Sources[WARNING], AL_LOOPING,  AL_FALSE       );

    // Do another error check and return.

    if( alGetError() != AL_NO_ERROR)
        return AL_FALSE;

    return AL_TRUE;
}
void setListenerValues()
{
    alListenerfv(AL_POSITION,    ListenerPos);
    alListenerfv(AL_VELOCITY,    ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);
}
/************************************************************
Here all buffers and sources are de-allocated
**************************************************************/
void killALData()
{
    alDeleteBuffers(NUM_BUFFERS, &Buffers[0]);
    alDeleteSources(NUM_SOURCES, &Sources[0]);
    exitOpenAL();
}
void handleIdle(void)
{
	//ALint play;
	if(mines[startX][startY] == '*')
	{
		system("cls");
		cout << "\nYou Loose\n" << endl;
		exit(0);		
	}else if(mines[startX][startY] == 'F')
	{
		system("cls");
		cout << "\nYou Win\n" << endl;
		exit(0);		
	}
}
/************************************************************
This section handles the menu selection
**************************************************************/
void menu(int value)
{
  if(value == 0)
  {
    glutDestroyWindow(win);
    exit(0);
  }else{
    primitive=value;
  }   
  glutPostRedisplay();													// you would want to redraw now
}
/************************************************************
This section controls what will be in the menu
**************************************************************/
void createMenu(void)
{  
  menid = glutCreateMenu(menu);											// Create the menu, this menu becomes the current menu
  glutAddMenuEntry("Show Mines", 1);									// Create an entry
  glutAddMenuEntry("Quit", 0);											// Create an entry
  glutAttachMenu(GLUT_RIGHT_BUTTON);									// Let the menu respond on the right mouse button
}

int main(int argc,char **argv)
{
	
	initRandNum("scripts/mines.lua");										//call for Lua to do the random number generator
	int *pScreen;
	pScreen = configScreen();											//pScreen now becomes the array returned by configScreen

	player one;															//creates a player struct
	configPlayer(one);													//fills player struct
	startX = one.StartPosX;												//takes variables from player struct for OpenGL to use
	startY = one.StartPosY;
 
	int number;
	cout << "\nHow many mines do you want in the game  " << endl;
	cin >> number;

	mineGenerator(number);												//Calls function to randomly generate mines

	initOpenAL();
    alGetError();
    if (loadALData() == AL_FALSE)
        return 0;
    setListenerValues();
    atexit(killALData);
    alSourcePlay(Sources[LOOP]);

	glutInit(&argc,argv);												
	glutInitWindowPosition(100,100);
	glutInitWindowSize(pScreen[0],pScreen[1]);						
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	init();	
	win = glutCreateWindow("Mine Sweeper");
	createMenu();

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutIdleFunc(handleIdle);
	glutSpecialFunc(handleSpecialKeys);								

	glOrtho(-1,16,-1,16,0,-1);
	//gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	
	glutMainLoop();

	
}