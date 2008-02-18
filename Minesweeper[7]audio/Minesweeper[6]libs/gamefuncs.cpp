#include "gamefuncs.h"

bool danger(char gameArray[16][16], int currX, int currY)
{
	if (gameArray[currX+1][currY] == '*'||gameArray[currX-1][currY] == '*'||gameArray[currX][currY+1] == '*'||gameArray[currX][currY-1] == '*')	//check north, east, south, west for mines
		return true;
	else
		return false;
}