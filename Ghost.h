/* File Ghost.h */
#ifndef Ghost_H
#define Ghost_H

#include <utility>
#include <vector>
#include <string>
#include "gameResources.h"
#include "Block.h"

using namespace std;
using namespace gameResources;

class Ghost
{
	public:
		//Builder
		Ghost();
		Ghost(int _id, std::vector <std::vector<Block> > _currentMap, int _speed, IA _ia);

		//Objectd methods
		int getId();
		int getSpeed();
		void setSpeed(int _speed);
		std::pair<int, int> getPosition();
        void setPosition(std::pair<int, int> _position);
        void setPacmanPosition(float _pacX, float _pacY);
        void respawn();

        Status getStatus();
        void setStatus(Status);
        void setStatusNext(std::vector <Status> _nextStatus);
        std::vector <Status> getNextStatus();

        void init_movement(int duration, Status _status);
        void integrate(long _time);
        float getx();
        float gety();
        void setMap(std::vector <std::vector<Block> > _currentMap);
        void run();
        bool isPacmanDead();
        void setGhostIA(IA _ia);

	private:
        bool movementWallRoutines(Block _nextBlock);

        //A.I.
        pair<int, int> nextPosition(Status _status,pair<int, int> _position);
        std::vector<Status> getLegalMoves(int _col, int _row);
        vector< pair< pair<int, int>, Status > > getLegalMovesWithPositions(int _col, int _row);
        int goToPosition(pair<float, float> _finalPosition, pair<float, float> _currentPosition, std::vector<Status> _legalMoves,
                    vector< pair<float, float> > _visitedNodes, int _cost, int _depth, int _weight);
        void  exitHome();
        void  iaRandom();
        void  iaReflex();
        void  iaAlphaBetaPruning();
        float alphaBetaPruning(pair<float, float> _position, int _weight, float _alpha, float _beta,
                                    std::vector<Status> _legalMoves, vector< pair<float, float> > _visitedNodes, int _depth, bool _maximizer);
        float calcPacmanPosition(pair<float, float> _position);
        vector<Status> removeVisitedNodes(vector< pair<float, float> > _visitedNodes, pair<float, float> _actualPosition);
        Status invertStatus(Status _status);

        std::vector <std::vector<Block> > currentMap;
        std::vector <Status> statusNext;
        std::pair <float, float>  position;
        Status currentStatus = Status::STOP;
        Status prevStatus    = Status::STOP;
        IA     ia;

        int   id;
        float x = 0, y = 0;   //-- Current position
        float vx, vy;         //-- Velocity vector
        float pacX, pacY;
        float pacDistance;
        long  time_remaining;
        float plusInfinite  = numeric_limits<float>::max();
        float minusInfinite = -(plusInfinite);
        bool  isOutHome     = true;
        int   speed         = 0;
};
#endif /* File Ghost_H */
