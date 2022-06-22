/* File MapAlgorithm.h */
#ifndef MapAlgorithm_H
#define MapAlgorithm_H

#include <vector>
#include "Block.h"

using namespace std;

class MapAlgorithm
{
	public:
		//Builder
		MapAlgorithm();

		//Object methods
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
#endif /* MapAlgorithm_H */
