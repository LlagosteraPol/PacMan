//INCLUDES
#include <stdio.h>
#include <vector>
#include <string>
#include <utility>
#include "gameResources.h"
#include "Block.h"
#include <math.h>

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

PacmanGuy::PacmanGuy(){};

PacmanGuy::PacmanGuy(int _id, std::vector <std::vector<Block> > _currentMap)
{
    id         = _id;
    currentMap = _currentMap;
}


std::pair<int, int> PacmanGuy::getPosition()
{
    return position;
}

void PacmanGuy::setPosition(std::pair<int, int> _position)
{
    x = _position.first;
    y = _position.second;

    position   = _position;
}

Status PacmanGuy::getStatus()
{
    return currentStatus;
}

void PacmanGuy::setStatus(Status _sts)
{
    currentStatus = _sts;
}

int PacmanGuy::getPoints()
{
    return points;
}

int PacmanGuy::sumPoints(int _points)
{
    points += _points;
    return points;
}

int PacmanGuy::getLives()
{
    return lives;
}

void PacmanGuy::setLives(int _lives)
{
    lives = _lives;
}

void PacmanGuy::removeLives()
{
    lives--;
}

void PacmanGuy::setStatusNext(std::vector <Status> _nextStatus)
{
    statusNext = _nextStatus;
}

std::vector <Status> PacmanGuy::getNextStatus()
{
    return statusNext;
}

bool PacmanGuy::movementWallRoutines(Block _nextBlock)
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

void PacmanGuy::init_movement(int _duration, Status _status)
{
    Block nextBlock;

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

void PacmanGuy::integrate(long _time)
{
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
            init_movement(200,statusNext.at(0));

        else
            init_movement(200,currentStatus);
    }

    /*printf("%f, passa per aqui %f \n",x, y);
    printf("status %d \n",currentStatus);
    if(statusNext.size())
        printf("proxim status %d \n",statusNext.at(0));
    printf("Punts: %d\n",getPoints());*/
}

float PacmanGuy::getx()
{
	return x;
}

float PacmanGuy::gety()
{
	return y;
}

void PacmanGuy::setMap(std::vector <std::vector<Block> > _currentMap)
{
    currentMap = _currentMap;
}
