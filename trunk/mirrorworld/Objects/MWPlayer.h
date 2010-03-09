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
    Player(unsigned int id);
    ~Player();
    Ogre::String name() const { return "Player"; }
    void init(Ogre::SceneManager* sceneMgr, OgreNewt::World* world);
    void updateAnimation(double timeElasped);
private:
    Ogre::Real              m_AnimationSpeed;
    Ogre::Entity*           m_pEntity;
    Ogre::Skeleton*         m_pSeleton;
    Ogre::AnimationState*   m_pCurState;
    std::map<Ogre::String, int>        m_BoneNameConverter;
}; // End of Player
} // End of MirrorWorld
#endif