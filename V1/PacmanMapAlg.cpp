//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <utility>
#include "PacmanResources.h"

using namespace std;

//DEFINES: Columns must be odd and rows must be even
#define COLUMNS 13
#define ROWS 12

//GLOBAL VARIABLES
static char  map[COLUMNS][ROWS];//test
Block blockMap[COLUMNS][ROWS];

//FUNCTIONS	
bool canPrint(int col, int row)
{
	Block 				 seeingBlock;
	std::pair <int, int> up			(col, row-1);
	std::pair <int, int> down		(col, row+1);
	std::pair <int, int> left		(col-1, row);
	std::pair <int, int> right		(col+1, row);
	std::pair <int, int> upLeft		(col-1, row-1);
	std::pair <int, int> upRight	(col+1, row-1);
	std::pair <int, int> downLeft	(col-1, row+1);
	std::pair <int, int> downRight	(col+1, row+1);
	
	
	//See up
	seeingBlock = blockMap[up.first][up.second];
	if (seeingBlock.getColumWeight() != 0
	||  seeingBlock.getRowWeight()	 != 0)
	{
		//See left
		seeingBlock = blockMap[left.first][left.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
			return false;
			
		//See right
		seeingBlock = blockMap[right.first][right.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
			return false;
			
		//See Down //Maybe must be pulished
		seeingBlock = blockMap[down.first][down.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
			return false;	
			
		//See down left
		seeingBlock = blockMap[downLeft.first][downLeft.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
			return false;
		
		//See down right	
		seeingBlock = blockMap[downRight.first][downRight.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
			return false;
			
		return true;
	}
	
	//See left
	seeingBlock = blockMap[left.first][left.second];
	if (seeingBlock.getColumWeight() != 0
	||  seeingBlock.getRowWeight()	 != 0)
	{	
		//See up right
		seeingBlock = blockMap[upRight.first][upRight.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
		return false;
		
		//See right
		seeingBlock = blockMap[right.first][right.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
			return false;
			
		//See down right	
		seeingBlock = blockMap[downRight.first][downRight.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
			return false;
			
		return true;
	}
	
	else
	{
		//See all diagonals
		
		//See up left
		seeingBlock = blockMap[upLeft.first][upLeft.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
			return false;
			
		//See up right
		seeingBlock = blockMap[upRight.first][upRight.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
		return false;
	
		//See down left
		seeingBlock = blockMap[downLeft.first][downLeft.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
			return false;
		
		//See down right	
		seeingBlock = blockMap[downRight.first][downRight.second];
		if (seeingBlock.getColumWeight() != 0
		||  seeingBlock.getRowWeight()	 != 0)
			return false;
	}
	
	return true;
}

//Applies simetry
void mirror(int col, int row)
{
	//Vertical
	blockMap[COLUMNS - col + 1][row] = Block(1, 1);
	
	//Horizontal
	blockMap[col][ROWS - row + 1] = Block(1, 1);
	blockMap[COLUMNS - col + 1][ROWS - row + 1] = Block(1, 1);
}

//Set ghosts home [can optimize] Must set the corret values to walls
void setGhostHome()
{
	int initPointCols = COLUMNS/2 + 1;
	int initPointRows = ROWS/2 + 2;
	
	//Base
	blockMap[initPointCols][initPointRows]   = Block(3,3);
	blockMap[initPointCols-1][initPointRows] = Block(3,3);
	blockMap[initPointCols-2][initPointRows] = Block(3,3);
	blockMap[initPointCols+1][initPointRows] = Block(3,3);
	blockMap[initPointCols+2][initPointRows] = Block(3,3);
	
	//Walls
	blockMap[initPointCols-2][initPointRows-1] = Block(3,3);
	blockMap[initPointCols-2][initPointRows-2] = Block(3,3);
	
	blockMap[initPointCols+2][initPointRows-1] = Block(3,3);
	blockMap[initPointCols+2][initPointRows-2] = Block(3,3);
	
	//Top
	blockMap[initPointCols-2][initPointRows-3] = Block(3,3);
	blockMap[initPointCols-1][initPointRows-3] = Block(3,3);
	blockMap[initPointCols+1][initPointRows-3] = Block(3,3);
	blockMap[initPointCols+2][initPointRows-3] = Block(3,3);
}

//Set basic layout, external corners and ghosts home [can improve performance in ghosts home]
void setUpMap()
{	
	for (int row = 1; row <= ROWS; row++)
	{
		for(int col = 1; col <= COLUMNS; col++)
		{
			if (col == 1 || col == COLUMNS || row == 1 || row == ROWS)
			{
				map[col][row]  		= 'O';//test
				blockMap[col][row]  = Block(col, row);//must set the correct values
			}
				
			
			else
			{
				map[col][row]  		= ' ';//test
				blockMap[col][row]  = Block(0, 0);
			}
		}
	}
	
	setGhostHome();
}

//Get random bool
bool randomBool() 
{
  return rand() % 2 == 1;
}

//Generates the map
void createMap()
{
	setUpMap();
	srand(time(NULL));
	
	for (int row = 1; row <= ROWS/2	; row++)
	{
		for (int col = 1; col <= COLUMNS/2; col++)
		{
			if (canPrint(col, row))
			{
				if (randomBool())
				{
					blockMap[col][row] = Block(1, 1);
					mirror(col, row);
				}
			}	
		}
	}
}

void printMap()
{
	Block printBlock;
	
	for (int row = 1; row <= ROWS; row++)
	{
		for (int col = 1; col <= COLUMNS; col++)
		{
			printBlock = blockMap[col][row];
			
			if (printBlock.getColumWeight() > 0 && printBlock.getRowWeight() > 0)
				printf("O ");
				
			else
				printf("  ");
			
		//	printf("%c ", map[row][col]);//test
		}
		printf("\n");
	}
}

int main(int argc,char *argv[])
{
	createMap();
	printMap();
	
	return 0;
}













