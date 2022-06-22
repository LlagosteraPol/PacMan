#ifndef PacmanResources_H
#define PacmanResources_H

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
		int columWeight;
		int rowWeight;
};

// End of the header guard
#endif
