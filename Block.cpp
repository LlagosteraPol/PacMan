#include <utility>

using namespace std;

class Block
{
	public:
		//Builder
		Block();
		Block(int columWeight, int rowWeight);
		Block(int columWeight, int rowWeight, bool food);

		//Object methods
		void setWeights(int columWeight, int rowWeight);
		void setFood(bool food);
		bool isFood();
		bool setPacman(bool _containsPacman);
		bool isPacman();
        bool setGhost(bool _containsGhost);
        bool isGhost();
		int getColumWeight();
		int getRowWeight();
		std::pair<int, int> getWeights();
		bool isWall();

	private:
		int columWeight     = 0;
		int rowWeight	    = 0;
		bool food           = false;
		bool containsPacman = false;
		bool containsGhost  = false;
};

Block::Block(){}

Block::Block(int _columW, int _rowW)
{
	columWeight = _columW;
	rowWeight   = _rowW;
}

Block::Block(int _columW, int _rowW, bool _foodB)
{
	columWeight = _columW;
	rowWeight   = _rowW;
	food        = _foodB;
}

std::pair<int, int> Block::getWeights()
{
	return std::pair<int, int> (columWeight, rowWeight);
}

void Block::setWeights(int _columW, int _rowW)
{
	columWeight = _columW;
	rowWeight   = _rowW;
}

void Block::setFood(bool _foodB)
{
	food = _foodB;
}

bool Block::isFood()
{
	return food;
}

bool Block::setPacman(bool _containsPacman)
{
    containsPacman = _containsPacman;
}

bool Block::isPacman()
{
    return containsPacman;
}

bool Block::setGhost(bool _containsGhost)
{
    containsGhost = _containsGhost;
}

bool Block::isGhost()
{
    return containsGhost;
}

int Block::getColumWeight()
{
	return columWeight;
}

int Block::getRowWeight()
{
	return rowWeight;
}

bool Block::isWall()
{
    return columWeight > 0 && rowWeight > 0;
}








