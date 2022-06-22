//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <time.h>
#include "Block.h"

using namespace std;

class MapAlgorithm
{
	public:
		//Builder
		MapAlgorithm();

		//Objectd methods
        std::vector<std::vector<Block> > setUpMap(int argc,char *argv[], int _seed, int _level);
        void displayPrintedMap(int argc,char *argv[]);
        std::vector<std::vector<Block> > getMap();
        int getTotalFood();

	private:
        void setColumnsRows(std::string _colums, std::string _rows);
        bool canPrint(int _col, int _row);
        void mirror(int _col, int _row);
        void setGhostHome();
        void setUpMap();
        bool randomBool();
        void createMap(int _seed);
        void show_usage(std::string _name);
        bool checkInputs(int argc,char *argv[]);

        std::vector <std::vector<Block> > blockMap;
        int  argc;
        char *argv[];
        int  COLUMNS;
        int  ROWS;
        int  level;
};

MapAlgorithm::MapAlgorithm(){}

//FUNCTIONS
void MapAlgorithm::setColumnsRows(std::string _colums, std::string _rows)
{
    int extendedSize = (level-1)*2;

    COLUMNS = atoi(_colums.c_str()) + extendedSize;
    ROWS = atoi(_rows.c_str()) + extendedSize;

	// now for memory allocation
    blockMap.resize(COLUMNS, std::vector<Block>(ROWS));
}

bool MapAlgorithm::canPrint(int _col, int _row)
{
	Block seeingBlock;

	std::pair <int, int> up			(_col, _row-1);
	std::pair <int, int> down		(_col, _row+1);
	std::pair <int, int> left		(_col-1, _row);
	std::pair <int, int> right		(_col+1, _row);
	std::pair <int, int> upLeft		(_col-1, _row-1);
	std::pair <int, int> upRight	(_col+1, _row-1);
	std::pair <int, int> downLeft	(_col-1, _row+1);
	std::pair <int, int> downRight	(_col+1, _row+1);


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
void MapAlgorithm::mirror(int _col, int _row)
{
	//Vertical
	blockMap[COLUMNS - _col - 1][_row] = Block(1, 1);

	//Horizontal
	blockMap[_col][ROWS - _row-1] = Block(1, 1);
	blockMap[COLUMNS - _col-1][ROWS - _row-1] = Block(1, 1);
}

//Set ghosts home
void MapAlgorithm::setGhostHome()
{
	int initPointCols = COLUMNS/2;
	int initPointRows = ROWS/2 + 1;

	//Base first wall
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

	//Inside
	blockMap[initPointCols][initPointRows-1]   = Block(0,0,false);
	blockMap[initPointCols+1][initPointRows-1] = Block(0,0,false);
	blockMap[initPointCols-1][initPointRows-1] = Block(0,0,false);
	blockMap[initPointCols][initPointRows-2]   = Block(0,0,false);
	blockMap[initPointCols+1][initPointRows-2] = Block(0,0,false);
	blockMap[initPointCols-1][initPointRows-2] = Block(0,0,false);
	blockMap[initPointCols][initPointRows-3]   = Block(0,0,false);
}

//Set basic layout, external corners and ghosts home [can improve performance in ghosts home]
void MapAlgorithm::setUpMap()
{
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLUMNS; col++)
		{
			if (col == 0 || col == COLUMNS-1 || row == 0 || row == ROWS-1)
			{
				blockMap[col][row]  = Block(1, 1);
			}

			else
			{
				blockMap[col][row]  = Block(0,0,true);
			}
		}
	}

	setGhostHome();
}

//Get random bool
bool MapAlgorithm::randomBool()
{
  return rand() % 2 == 1;
}

//Generates the map
void MapAlgorithm::createMap(int _seed)
{
    srand(_seed);
	setUpMap();

	for (int row = 1; row < ROWS/2; row++)
	{
		for (int col = 1; col < COLUMNS/2; col++)
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


void MapAlgorithm::show_usage(std::string _name)
{
    std::cerr << "Please enter the value of columns and rows.\n\n"
	      << "IMPORTANT: The value of colums and rows must be odd.\n\n"
              << "Example:\n\n"
              << _name << "25 25"
              << std::endl;
}

bool MapAlgorithm::checkInputs(int argc,char *argv[])
{
    if (argc < 3 || atoi(argv[1]) % 2 == 0 || atoi(argv[2]) % 2 == 0)
	{
		show_usage(argv[0]);
		return false;
    }
    else
    {
    	setColumnsRows(argv[1],argv[2]);
    	return true;
	}
}

//TODO: This method can be optimized
int MapAlgorithm::getTotalFood()
{
	Block block;
    int   totalFood;
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLUMNS; col++)
		{
            block = blockMap[col][row];

			if (block.getColumWeight() == 0 && block.getRowWeight() == 0)
			{
                totalFood++;
			}
		}
    }
    // -7 == Ghost house
    return totalFood - 7;
}

std::vector<std::vector<Block> > MapAlgorithm::setUpMap(int argc,char *argv[], int _seed, int _level)
{
    level = _level;

	if (checkInputs(argc, argv))
        createMap(_seed);

	return blockMap;
}

std::vector<std::vector<Block> > MapAlgorithm::getMap()
{
    return blockMap;
}













