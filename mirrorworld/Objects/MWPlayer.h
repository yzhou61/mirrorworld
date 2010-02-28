//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_PLAYER_H_
#define _MW_PLAYER_H_

#include <OgreNewt.h>
#include "MWObject.h"

using Ogre::Real;
using Ogre::Vector3;

namespace MirrorWorld{

class Player : public Object
{
public:
    Player():Object(){}
    ~Player() {}
private:
}; // End of Player
} // End of MirrorWorld
#endif