//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_PLAYER_H_
#define _MW_PLAYER_H_
#include "MWObject.h"

using Ogre::Real;
using Ogre::Vector3;

namespace MirrorWorld{

class Player : public Object
{
public:
    Player(unsigned int id):Object(id){}
    ~Player() {}
    Ogre::String name() { return "Player"; }
private:

}; // End of Player
} // End of MirrorWorld
#endif