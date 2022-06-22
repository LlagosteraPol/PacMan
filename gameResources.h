/* File gameResources.h */
#ifndef gameResources_H
#define gameResources_H

#define MAXPLAYERS 2
#define MAXGHOSTS 6

namespace gameResources
{
    enum Status { STOP, UP, DOWN, RIGHT, LEFT, DEAD, SCARED };
    enum IA { RANDOM, REFLEX,ALPHABETA };
}
#endif /* File gameResources_H */
