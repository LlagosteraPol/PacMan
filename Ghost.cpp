//INCLUDES
#include <utility>
#include <vector>
#include <algorithm>
#include <string>
#include <limits>
#include "gameResources.h"
#include "Block.h"
#include <math.h>
#include <time.h>
#include <iostream>

#define PACMANDEADRANGE 0.45
#define DEPTH 5

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

Ghost::Ghost(){}

Ghost::Ghost(int _id, std::vector <std::vector<Block> > _currentMap, int _speed, IA _ia)
{
    srand(time(NULL));

    id         = _id;
    currentMap = _currentMap;
    ia         = _ia;
    speed      = _speed;
    respawn();
}

void Ghost::setGhostIA(IA _ia)
{
    ia = _ia;

   run();
}

int Ghost::getId()
{
    return id;
}

int Ghost::getSpeed()
{
    return speed;
}

void Ghost::setSpeed(int _speed)
{
    speed = _speed;
}

std::pair<int, int> Ghost::getPosition()
{
    return position;
}

void Ghost::setPosition(std::pair<int, int> _position)
{
    x = _position.first;
    y = _position.second;

    position   = _position;
}

void Ghost::respawn()
{
    int mapRows = currentMap.size();
    int mapCols = currentMap[0].size();
    pair<float, float> inHouse   = make_pair(mapCols/2 - 1, mapRows/2);

    x = inHouse.first;
    y = inHouse.second;

    position   = inHouse;
    isOutHome  = false;
}

Status Ghost::getStatus()
{
    return currentStatus;
}

void Ghost::setStatus(Status _sts)
{
    currentStatus = _sts;
}

void Ghost::setStatusNext(std::vector <Status> _nextStatus)
{
    statusNext = _nextStatus;
}

std::vector <Status> Ghost::getNextStatus()
{
    return statusNext;
}

bool Ghost::movementWallRoutines(Block _nextBlock)
{
    Block currentBlock = currentMap[position.first][position.second];

    if (_nextBlock.isWall())
    {
        currentStatus = Status::STOP;
        if (statusNext.size())
            statusNext.pop_back();
        return true;
    }
    return false;
}

void Ghost::init_movement(int _duration, Status _status)
{
    Block nextBlock;
    prevStatus = currentStatus;

    switch(_status)
    {
        case Status::RIGHT:
            nextBlock = currentMap[position.first + 1][position.second];

            if (!movementWallRoutines(nextBlock))
            {
                x  = roundf(x);
                vx = (x + 1 - x) / _duration;

                currentStatus  = _status;
                time_remaining = _duration;
            }
            break;

        case Status::LEFT:
            nextBlock = currentMap[position.first - 1][position.second];

            if (!movementWallRoutines(nextBlock))
            {
                x  = roundf(x);
                vx = (x - 1 - x) / _duration;

                currentStatus  = _status;
                time_remaining = _duration;
            }
            break;

        case Status::DOWN:
            nextBlock = currentMap[position.first][position.second - 1];

            if (!movementWallRoutines(nextBlock))
            {
                y  = roundf(y);
                vy = ((y - 1 - y) / _duration);

                currentStatus  = _status;
                time_remaining = _duration;
            }
            break;

        case Status::UP:
            nextBlock = currentMap[position.first][position.second + 1];

            if (!movementWallRoutines(nextBlock))
            {
                y  = roundf(y);
                vy = ((y + 1 - y) / _duration);

                currentStatus  = _status;
                time_remaining = _duration;
            }
            break;
    }
}

void Ghost::integrate(long _time)
{
    Block nextBlock;
    Block currentBlock = currentMap[position.first][position.second];

    if (currentStatus > Status::STOP &&
        currentStatus < Status::DEAD &&
        _time < time_remaining)
    {
        switch (currentStatus)
        {
            case Status::RIGHT:
            case Status::LEFT:
                x = x + vx * _time;
                break;

            case Status::DOWN:
            case Status::UP:
                y = y + vy * _time;
                break;
        }
        time_remaining -= _time;
    }

  else if (currentStatus > Status::STOP &&
           currentStatus < Status::DEAD &&
           _time >= time_remaining)
    {
        switch (currentStatus)
        {
            case Status::RIGHT:
            case Status::LEFT:
                x = x + vx * time_remaining;

                if(fmod(x,1) != 0)
                    x = roundf(x);

                setPosition({x, position.second});
                break;

            case Status::DOWN:
            case Status::UP:
                y = y + vy * time_remaining;

                if (fmod(y,1) != 0)
                    y = roundf(y);

                setPosition({position.first, y});
            break;
        }

        if(statusNext.size())
            init_movement(speed, statusNext.at(0));

        else
            init_movement(speed, currentStatus);
    }

   /* printf("%f, passa per aqui %f \n",x, y);
    printf("status %d \n",currentStatus);
    if(statusNext.size())
        printf("proxim status %d \n",statusNext.at(0));*/
}

float Ghost::getx()
{
	return x;
}

float Ghost::gety()
{
	return y;
}

void Ghost::setMap(std::vector <std::vector<Block> > _currentMap)
{
    currentMap = _currentMap;
}

void Ghost::setPacmanPosition(float _pacX, float _pacY)
{
    pacX        = _pacX;
    pacY        = _pacY;
    pacDistance = sqrt( pow( (pacX - x), 2.0) + pow( (pacY - y), 2.0) );
}

bool Ghost::isPacmanDead()
{
    return pacDistance <= PACMANDEADRANGE;
}

void Ghost::run()
{

    int mapRows = currentMap.size();
    int mapCols = currentMap[0].size();

    pair<float, float> outHouse;



    if (x - trunc(x) == 0
    &&  y - trunc(y) == 0)
    {
        outHouse = make_pair(mapCols/2, mapRows/2 - 3);

        if (x == outHouse.first && y == outHouse.second)
            isOutHome = true;

        else if (x == outHouse.first && y == outHouse.second + 1)
            isOutHome = false;

        if (isOutHome)
        {
            switch (ia)
            {
                case IA::RANDOM: iaRandom(); break;

                case IA::REFLEX: iaReflex(); break;

                case IA::ALPHABETA: iaAlphaBetaPruning(); break;

                default : init_movement(speed, Status::DOWN); break;
            }
        }

        else
            exitHome();
    }
}

pair<int, int> Ghost::nextPosition(Status _status,pair<int, int> _position)
{
    switch (_status)
    {
        case Status::UP : return make_pair(_position.first, _position.second + 1); break;

        case Status::DOWN : return make_pair(_position.first, _position.second - 1); break;

        case Status::RIGHT : return make_pair(_position.first + 1, _position.second); break;

        case Status::LEFT : return make_pair(_position.first - 1, _position.second); break;

        default : return make_pair(_position.first, _position.second); break;
    }
}

std::vector<Status> Ghost::getLegalMoves(int _col, int _row)
{
    std::vector<Status>  legalMoves;
    Block                viewBlock;

    //See up
    viewBlock = currentMap[_col][_row+1];
    if (!viewBlock.isWall()) legalMoves.push_back(Status::UP);

    //See down
    viewBlock = currentMap[_col][_row-1];
    if (!viewBlock.isWall()) legalMoves.push_back(Status::DOWN);

    //See right
    viewBlock = currentMap[_col+1][_row];
    if (!viewBlock.isWall()) legalMoves.push_back(Status::RIGHT);


    //See left
    viewBlock = currentMap[_col-1][_row];
    if (!viewBlock.isWall()) legalMoves.push_back(Status::LEFT);

    return legalMoves;
}

vector< pair<pair<int, int>, Status> > Ghost::getLegalMovesWithPositions(int _col, int _row)
{
    vector< pair<pair<int, int>, Status > > futurePositions;
    vector<Status> legalMoves = getLegalMoves(_col, _row);

    for (vector<Status> ::iterator lmIt = legalMoves.begin(); lmIt != legalMoves.end(); lmIt++)
    {
        switch( (*lmIt) )
        {
            case Status::UP:
                futurePositions.push_back( make_pair( make_pair(_col , _row + 1), (*lmIt) ) );
                break;
            case Status::DOWN:
                futurePositions.push_back( make_pair( make_pair(_col, _row - 1), (*lmIt) ) );
                break;
            case Status::RIGHT:
                futurePositions.push_back( make_pair( make_pair(_col + 1, _row), (*lmIt) ) );
                break;
            case Status::LEFT:
                futurePositions.push_back( make_pair( make_pair(_col - 1, _row), (*lmIt) ) );
                break;
        }
    }
    return futurePositions;
}

void Ghost::iaRandom()
{
    std::vector<Status> legalMoves = getLegalMoves(x, y);
    int random;


    if (rand() % 3 == 0 || currentStatus == Status::STOP)
    {
        random = rand() % legalMoves.size();
        init_movement(speed, legalMoves[random]);
    }
}

void Ghost::iaReflex()
{
    vector< pair< pair<int, int>, Status > > futurePositions = getLegalMovesWithPositions(x, y);
    Status bestAction = Status::STOP;
    float  prevDistance = pacDistance;
    float  nextDistance;


    for (vector< pair< pair<int, int>, Status> > ::iterator fpIt = futurePositions.begin(); fpIt != futurePositions.end(); fpIt++)
    {
        nextDistance = calcPacmanPosition( make_pair( (float) (*fpIt).first.first, (float) (*fpIt).first.second ) );

        if (nextDistance < pacDistance)
            bestAction = (*fpIt).second;

        prevDistance = nextDistance;
    }

    if (bestAction != Status::STOP)
        init_movement(speed, bestAction);

    else
        iaRandom();
}

void Ghost::iaAlphaBetaPruning()
{
    vector<Status> legalMoves = getLegalMoves(x, y);

    Status bestAction = Status::STOP;

    float alpha = minusInfinite;
    float beta  = plusInfinite;
    float prevBeta;

    pair<float, float> statusPosition;
    vector< pair<float, float> > visitedNodes;


    visitedNodes.push_back( make_pair(x, y) );

    if (( std::find(legalMoves.begin(), legalMoves.end(), invertStatus(prevStatus)) != legalMoves.end() ) )
            legalMoves.erase(std::find(legalMoves.begin(), legalMoves.end(), invertStatus(prevStatus)));

    for (vector<Status>::iterator it = legalMoves.begin(); it != legalMoves.end(); it++)
    {
        statusPosition = nextPosition( (*it), getPosition() );
        prevBeta      = beta;

        beta = min(beta, alphaBetaPruning(statusPosition, 1, alpha, beta,
                                            removeVisitedNodes(visitedNodes, statusPosition), visitedNodes, DEPTH, true) );

        if (beta < prevBeta)
            bestAction = (*it);

        if (beta <= alpha)
            break;
    }

    //MAXIMITZANT primer
    /*
    for (vector<Status>::iterator it = legalMoves.begin(); it != legalMoves.end(); it++)
    {
        statusPosition = nextPosition( (*it), getPosition() );
        prevAlpha      = alpha;

        alpha = max(alpha, alphaBetaPruning(statusPosition, calcPacmanPosition(statusPosition), 1, alpha, beta,
                                                removeVisitedNodes(visitedNodes, statusPosition), visitedNodes, DEPTH, false) );

        if (alpha > prevAlpha)
            bestAction = (*it);

        if (beta <= alpha )
            break;
    }*/

    if (bestAction != Status::STOP)
        init_movement(speed, bestAction);

    else
        iaRandom();
}


float Ghost::alphaBetaPruning(pair<float, float> _position, int _weight, float _alpha, float _beta,
                                std::vector<Status> _legalMoves, vector< pair<float, float> > _visitedNodes, int _depth, bool _maximizer)
{
    pair<float, float> statusPosition;
    float newDistance = calcPacmanPosition(_position);

    _visitedNodes.push_back(_position);

    if (newDistance <= PACMANDEADRANGE)
        return _weight;

    else if (_depth == _weight)
        _legalMoves.clear();

    _weight++;

    if (_maximizer)
    {
        for (vector<Status>::iterator it = _legalMoves.begin(); it != _legalMoves.end(); it++)
        {
            statusPosition = nextPosition((*it), _position);

            _alpha = max(_alpha, alphaBetaPruning(statusPosition, _weight, _alpha, _beta,
                            removeVisitedNodes(_visitedNodes, statusPosition), _visitedNodes, _depth, false) );

            if (_beta <= _alpha)
            {
                return _alpha;
            }
        }
        //Leaf
        return _alpha <= minusInfinite ? plusInfinite : _alpha;
    }
    else
    {
        for (vector<Status>::iterator it = _legalMoves.begin(); it != _legalMoves.end(); it++)
        {
            statusPosition = nextPosition((*it), _position);

            _beta = min(_beta, alphaBetaPruning(statusPosition, _weight, _alpha, _beta,
                            removeVisitedNodes(_visitedNodes, statusPosition), _visitedNodes, _depth, true));

            if (_beta <= _alpha)
            {
                return _beta;
            }
        }
        //Leaf
        return _beta >= plusInfinite ? minusInfinite : _beta;
    }
}

float Ghost::calcPacmanPosition(pair<float, float> _position)
{
   return sqrt( pow( (pacX - _position.first), 2.0) + pow( (pacY - _position.second), 2.0) );
}

vector<Status> Ghost::removeVisitedNodes(vector< pair<float, float> > _visitedNodes, pair<float, float> _actualPosition)
{
    vector< pair< pair<float, float>, Status> > possibleNodes;
    vector<Status> realLegalMoves;
    vector<Status> fakeLegalMoves = getLegalMoves(_actualPosition.first, _actualPosition.second);

    for (vector<Status> ::iterator flmIt = fakeLegalMoves.begin(); flmIt != fakeLegalMoves.end(); flmIt++)
    {
        switch( (*flmIt) )
        {
            case Status::UP:
                possibleNodes.push_back( make_pair( make_pair(_actualPosition.first , _actualPosition.second + 1), (*flmIt) ) );
                break;
            case Status::DOWN:
                possibleNodes.push_back( make_pair( make_pair(_actualPosition.first, _actualPosition.second - 1), (*flmIt) ) );
                break;
            case Status::RIGHT:
                possibleNodes.push_back( make_pair( make_pair(_actualPosition.first + 1, _actualPosition.second), (*flmIt) ) );
                break;
            case Status::LEFT:
                possibleNodes.push_back( make_pair( make_pair(_actualPosition.first - 1, _actualPosition.second), (*flmIt) ) );
                break;
        }
    }

    for (vector< pair< pair<float, float>, Status> > ::iterator pnIt = possibleNodes.begin(); pnIt != possibleNodes.end(); pnIt++)
    {
        if (!( std::find(_visitedNodes.begin(), _visitedNodes.end(), (*pnIt).first) != _visitedNodes.end() ) )
        {
            realLegalMoves.push_back((*pnIt).second);
        }
    }
    return realLegalMoves;
}

Status Ghost::invertStatus(Status _status)
{
    switch(_status)
    {
        case Status::UP:    return Status::DOWN;
        case Status::DOWN : return Status::UP;
        case Status::RIGHT: return Status::LEFT;
        case Status::LEFT : return Status::RIGHT;
        default           : return Status::STOP;
    }
}

void Ghost::exitHome()
{
    int cost  = numeric_limits<int>::max();
    float prevCost;
    int mapRows = currentMap.size();
    int mapCols = currentMap[0].size();

    pair<float, float> statusPosition;
    pair<float, float> outHouse   = make_pair(mapCols/2, mapRows/2 - 3);

    vector<Status> legalMoves  = getLegalMoves(x, y);
    vector< pair<float, float> > visitedNodes;

    Status bestAction = Status::STOP;

    visitedNodes.push_back( make_pair(x, y) );

    for (vector<Status>::iterator it = legalMoves.begin(); it != legalMoves.end(); it++)
    {
        statusPosition = nextPosition( (*it), getPosition() );
        prevCost       = cost;

        cost = min(cost, goToPosition(outHouse, statusPosition, removeVisitedNodes(visitedNodes, statusPosition),
                                            visitedNodes, cost, 6, 1) );

        if (cost < prevCost)
            bestAction = (*it);
    }
    init_movement(speed, bestAction);
}

//WARNING: Use this method only to go to nearby positions
int Ghost::goToPosition(pair<float, float> _finalPosition, pair<float, float> _currentPosition, std::vector<Status> _legalMoves,
                    vector< pair<float, float> > _visitedNodes, int _cost, int _depth, int _weight)
{
    pair<float, float> statusPosition;

    _visitedNodes.push_back(_currentPosition);

    if (_currentPosition == _finalPosition)
    {
        return _weight;
    }

    else if (_weight == _depth)
    {
        _legalMoves.clear();
    }

    _weight++;

    for (vector<Status>::iterator it = _legalMoves.begin(); it != _legalMoves.end(); it++)
    {

        statusPosition = nextPosition((*it), _currentPosition);

        _cost = min(_cost, goToPosition(_finalPosition, statusPosition, removeVisitedNodes(_visitedNodes, statusPosition),
                                            _visitedNodes, _cost, _depth, _weight) );
    }
    //Leaf
    return _cost;
}















