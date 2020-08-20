/*
 * Carlos Ramos CS 241.
 * December 11 2019.
 * AI tron project.
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "tron.h"

static int myPlayerIndex;
static int myFirstCycleIdx;
static int myLastCycleIdx;

static int DEBUG = 0;
static int grid[MAX_GRID_WIDTH][MAX_GRID_HEIGHT];

static struct Cycle lastCycles[MAX_CYCLES];
static struct Cycle curCycles[MAX_CYCLES];

static int lastDirBit[MAX_CYCLES];
static int goalDirBit[MAX_CYCLES];
static int openingMove[MAX_CYCLES];
static int originalX[MAX_CYCLES], originalY[MAX_CYCLES];

static int gridWidth;
static int gridHeight;
static int cyclesInPlay;
static int northPosition;
static char myName[] = "Carlos";
static char allCycle[] = "ABCDEFGHIJKLMNOPQRST";
static char cycleTracked[] = "00000000000000000000";
static char northCycles[] = "ABCDEFGHIJ";
static char southCycles[] = "KLMNOPQRST";

static int halfWidth;
static int beginMoveCount2[MAX_CYCLES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
static int beginMoveCount2Sides[MAX_CYCLES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static int beginMoveCount;
static int start2 = TRUE;
static int start = TRUE;
static int myCyclesInd = 0;
static int zero = 0;
static const char EMPTY = '.';
static const char WALL = '#';
#define TOTAL_DIRECTIONS 4


static void printGrid(void)
{ 
    int x, y;
    for (y=0; y<gridHeight; y++)
    {
        for (x=0; x<gridWidth; x++)
        {
            printf("%3d ", grid[x][y]);
        }
        printf("\n");
    }
}
/*
 *Clars the grid of dead cycles 
 Uses cycleTracked as a list flags for each cycle that was not called
 from the linked list. If this is the case then it will look 
 to see if they are not called and then Erase them from the board.
 *
*/
static void clearGrid(void)
{
for (int i = 0; i < MAX_CYCLES; i++)
    {
        if(cycleTracked[i] == 0)
        {    
            for(int rows =1; rows < gridWidth - 1; rows++)
	    {
                for(int columns = 1; columns < gridHeight-1; columns++)
                {
                    if(grid[rows][columns] == allCycle[i])
                    {
                        grid[rows][columns] = 0;
                    }
                }
            }
        }
    }
}
/*
 * Returns the index of each ot he cycles.
*/
static int getCycleIndex(char cycleName)
{ 
    int cycleIdx = cycleName - 'A';
    if ((cycleIdx < 0) || (cycleIdx >= MAX_CYCLES))
    {
        printf("JoelRobot.c: ***ERROR*** getCycleIndex(%d)\n",
        cycleIdx);
    exit(0);
}
return cycleIdx;
}
/*
 * Checks a new position. If there is more opennings than it returns TRUE;
 * if there are no open position returns FALSE;
*/
static int possibleDirection(short x, short y)
{
    int indPos[TOTAL_DIRECTIONS];
    int count = 0;
    for(int i = 0; i < TOTAL_DIRECTIONS; i++)
    {
       // printf("x + dir = %d\n", x + DIR_DX[i]);   
       // printf("grid[x][y] = %3d\n" ,grid[x + DIR_DX[i]][y + DIR_DY[i]]);
         if( grid[x + DIR_DX[i]][y+DIR_DY[i]] == 0) 
         {
            indPos[count] = i;
            count++;
//          printf("\n");
//          printf("x,y, value = %d (%d,%d)     ",grid[x+DIR_DX[i]][y+DIR_DY[i]],
       //  x+DIR_DX[i], y+DIR_DY[i]);
         }
    }
    // returns 0 if count of directions is 0;
    if( count == 0)
    {
        return 0;
    }else return 1;

}
static int cyclePathClear(short x, short y, int comingFromIdx)
{
}
/*
 * SEts Height priority fro the cycle. if the cycle is in the northern
 * side of the grid return a index of 2 to go south
 * if the cycle is in the south Direction return 0 to go noth.
 * */
static int cycleDirPriorityHeight(short x, short y)
{
    if( y< gridHeight/2) return 2;
    else if(y >= gridHeight/2) return 0;
}
/*
 *Checks to see if the cycle is in the leftmost side of the grid
 or the rightmost side of the grid
 returns index 3 to turn left and index 1 to turn right;
 */
static int cycleDirPriority(short x, short y)
{
	
    if( x < halfWidth) return 3;
    else if( x >= halfWidth) return 1;
}
/*
 *Finds a direction for the cycles to go into.
 * checks the options for the cycle in 4 different directions
 * if that direction is open then it adds them to indPos which is an array with 
 * indexes to take. count == 0 means no open positions. 
 * comingFromIdx is the index from 0 - 19 of the cycles previous direction
*/

static int dirFinder(short  x, short y, int comingFromIdx)
{
    int indPos[TOTAL_DIRECTIONS];
    int count = 0;
    //chckes to see open positions of north south east and west
    //It places them in an array of possible indexes to go into.
    printf("possible indexes:");
    for(int i = 0; i < TOTAL_DIRECTIONS; i++)
    {
       // printf("x + dir = %d\n", x + DIR_DX[i]);   
       // printf("grid[x][y] = %3d\n" ,grid[x + DIR_DX[i]][y + DIR_DY[i]]);
        if( grid[x + DIR_DX[i]][y+DIR_DY[i]] == 0 &&
            possibleDirection(x+DIR_DX[i],y+DIR_DY[i]) == 1) 
        {
            indPos[count] = i;
            count++;
//          printf("x,y, value = %d (%d,%d)     ",grid[x+DIR_DX[i]][y+DIR_DY[i]],
           //  x+DIR_DX[i], y+DIR_DY[i]);
        }
    }
    // returns 4 if no Available position in the array.
    if( count == 0)
    {
        return 4;
    }
    if ((comingFromIdx == 0 || comingFromIdx == 10) && beginMoveCount > 2) start = FALSE; 
  //  printf("comingFromIdx = %d\n",comingFromIdx);
  //  printf("start = %d, start2 = %d, beginMoveCount= %d\n", start,start2,beginMoveCount); 
    // Phase 2 of the maze will begin with having 2 list of counts. one side moves and the 
    // other for vertical moves.
    if(start != TRUE && start2 == TRUE)
    {
        if(beginMoveCount2[comingFromIdx] >= 1)
        { 
             for(int i = 0; i < count; i++)
             {
                  if(indPos[i] == cycleDirPriority(x,y)) 
                  {
                      beginMoveCount2Sides[comingFromIdx] += 1;
                      beginMoveCount2[comingFromIdx] = 0;
                      return indPos[i];
                  }
             }
        }
        else
        {
            for(int i = 0; i < count; i ++)
            {
                if( indPos[i] == cycleDirPriorityHeight(x,y))
                {
                    beginMoveCount2[comingFromIdx] += 1;
                    beginMoveCount2Sides[comingFromIdx] = 0;
                    return indPos[i];
                }
            }
	}	
    }
    // if its true it has set 
    else if( start == TRUE)
    { 
        if( comingFromIdx == 0 || comingFromIdx == 10) beginMoveCount++;
        for(int i = 0; i < count; i++)
        {
            if(northPosition == TRUE && indPos[i] == 2)
            { 
                return indPos[i];
            }
            else if(northPosition == FALSE && indPos[i] == 0)
            {
                return indPos[i];
            }
        }
    }
   // if no other direction go in a straight line from where 
   // it was ccoming from. 
        for(int i = 0; i < count; i++)
	{
            if(DIR_LIST[indPos[i]] == lastDirBit[comingFromIdx])
	    {
	        return indPos[i];
	    }
	} 
    //last resort go in a random direction.
    int randomValue = rand()%count;
    printf("START = %d\n", start);
    printf("RANDOM\n");
    //  printf("randomValue = %d\n", randomValue);
    count = 0;
 //   printf("indpos = %d\n", indPos[randomValue]);
    return indPos[randomValue];
}

char *Carlos_RamosGuereca_getName() { return "Carlos"; }
/*
 * Initializes all of my variables.
 * the grid is from 1 to height -1 because it contains
 * boarders that are not inside of the moving grid.
 * creates a struct for walls
*/
void Carlos_RamosGuereca_init(struct InitData *data) 
{
    gridWidth = data->gridWidth;
    gridHeight = data->gridHeight;
    halfWidth = (data->gridWidth)/2; 
    int location = strcmp(myName, data->name[0]);
    for( int i = 0; i < MAX_PLAYERS; i++)
    {
         
    }
    for( int x = 0; x < gridWidth; x++)
    {
        for(int y = 0; y < gridHeight; y++)
	{
            grid[x][y] = 0;
	    if(x == 0 || y == 0 || x == gridWidth - 1 || y == gridHeight -1)
            {
                grid[x][y] = 256;
	    }
	}
    }
    struct Wall *wall = data->wallList;
    while(wall)
    {
        grid[wall->x][wall ->y] = WALL;
	wall = wall->next;
    }
    cyclesInPlay = MAX_CYCLES;
    //if location and name[0] are the same Location is 0
    //if it is 0 then i start north.
    if (location == FALSE)northPosition = TRUE;
    else 
    {
        myCyclesInd = 9;
        northPosition = FALSE;
    }
}

/*
 * method find counts the amount of cycles cycles and clears the ones 
 * that are not in the cyckeTracked list.
 * */
static void find(struct MoveData *data)
{
    struct Cycle *cycle = data->cycleList;
    int i = 0;
    while(cycle)
    {
    int index = getCycleIndex(cycle->cycleName);
    cycleTracked[index] = 1;
     cycle = cycle->nextCycle;
    }
    clearGrid();
}
/*
 * Move data consist of storing enemy cycles
 * moving mycycles and storing previous direction 
 * */
void Carlos_RamosGuereca_move(struct MoveData *data) 
{   printf("%d\n",halfWidth); 
    find(data);
    clearGrid();
    //
    struct Cycle *cycle = data->cycleList;
    int i = 0;
  while(cycle)//  for(int i = 0; i < cyclesInPlay; i++ )
    {
        int player = cycle->playerIdx;
	int xdirection = cycle->x;
	int ydirection = cycle->y;
	char cycleName = cycle->cycleName;
	int cycleIndex = getCycleIndex(cycle->cycleName);
	
	if(start == TRUE)
	{
            originalX[cycleIndex] = cycle->x;
	    originalY[cycleIndex] = cycle->y;
	}
	//moves my cycles only
 	if(((player != northPosition  && northPosition == TRUE))|| (player != northPosition
			&& northPosition == FALSE))
	{
	if(grid[(cycle->x)][(cycle->y)] == 0)
	{
	    grid[(cycle->x)][(cycle->y)] += cycle->cycleName;
	}
	int moveDirection = dirFinder(cycle->x, cycle->y, cycleIndex);

        cycleTracked[cycleIndex] = 1;     
	if(moveDirection != 4 )
            {
	//        printf("moveDirection = %d : %c\n", moveDirection,cycleName); 
		if(allCycle[cycleIndex] == cycle->cycleName) cycleTracked[cycleIndex] = 1;
                
       	        grid[(cycle->x) + DIR_DX[moveDirection]]
	        [(cycle->y) + DIR_DY[moveDirection]] += cycle->cycleName;
		
		lastDirBit[i] = DIR_LIST[moveDirection];
	        cycle->x += DIR_DX[moveDirection];	
	        cycle->y += DIR_DY[moveDirection];
		
            }
       }
       // only stores the cycles if they are not mine.
       else 
       {
            // storing Opponents Cycles
	    if(allCycle[cycleIndex] == cycle->cycleName) cycleTracked[cycleIndex] = 1;
	    if(grid[(cycle->x)][(cycle->y)] == 0)
	    {
	    grid[(cycle->x)][(cycle->y)] += cycleName;
	    }
	}
        // Goes to next cycles if they are Null.
      //  if(cycle->nextCycle != NULL)
//	{  
        i++;
	cycle = cycle->nextCycle;
//        }
    }
    
    clearGrid();
    //resets the flags fro uncalled cycles.
    for( int i = 0; i < MAX_CYCLES;i ++)
    {
    cycleTracked[i] = 0;
    }

   // printGrid();
}   
