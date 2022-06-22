/* File PacmanGuy.h */
#ifndef PacmanGuy_H
#define PacmanGuy_H

#include <stdio.h>
#include <vector>
#include <string>
#include <utility>
#include "gameResources.h"
#include "Block.h"

using namespace std;
using namespace gameResources;

class PacmanGuy
{
	public:
		//Builders
		PacmanGuy();
		PacmanGuy(int _id, std::vector <std::vector<Block> > _currentMap);

		//Object methods
        std::pair<int, int> getPosition();
        void setPosition(std::pair<int, int> _position);

        int getPoints();
        int sumPoints(int);
        int getLives();
        void setLives(int _lives);
        void removeLives();

        Status getStatus();
        void setStatus(Status);
        void setStatusNext(std::vector <Status> _nextStatus);
        std::vector <Status> getNextStatus();

        void init_movement(int duration, Status _status);
        void integrate(long _time);
        float getx();
        float gety();
        void setMap(std::vector <std::vector<Block> > _currentMap);

	private:
        bool movementWallRoutines(Block _nextBlock);

	    std::vector <Status> statusNext;
        std::pair <int, int>  position;
        std::vector <std::vector<Block> > currentMap;
        Status currentStatus = Status::STOP;

        int   points = 0;
        int   lives = 3;
        int   id;
        float x = 0, y = 0;   //-- Current position
        float vx, vy;         //-- Velocity vector
        long  time_remaining;
};
#endif /* File PacmanGuy_H */
