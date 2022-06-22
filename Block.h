/* File Block.h */
#ifndef Block_H
#define Block_H

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
		int columWeight = 0;
		int rowWeight	= 0;
		bool food       = false;
		bool containsPacman = false;
		bool containsGhost  = false;
};
#endif /* File Block_H */
