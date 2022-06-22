#include <utility>

using namespace std;

class Block
{
	public:
		//Builder
		Block();
		Block(int columWeight, int rowWeight);
		
		//Object methods
		void setWeights(int columWeight, int rowWeight);
		int getColumWeight();
		int getRowWeight();
		std::pair<int, int> getWeights();
			
	private: 
		int columWeight = 0;
		int rowWeight	= 0;
};

Block::Block(){}

Block::Block(int columW, int rowW)
{
	columWeight = columW;
	rowWeight   = rowW;
}

void Block::setWeights(int columW, int rowW)
{
	columWeight = columW;
	rowWeight   = rowWeight;
}

int Block::getColumWeight()
{
	return columWeight;
}

int Block::getRowWeight()
{
	return rowWeight;
}

std::pair<int, int> Block::getWeights()
{
	return std::pair<int, int> (columWeight,rowWeight);
}








