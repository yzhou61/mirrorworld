//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_MIRROR_BALL_H_
#define _MW_MIRROR_BALL_H_
#include "MWObject.h"

namespace MirrorWorld {
class MirrorBall : public Object
{
public:
    MirrorBall(unsigned int id):Object(id, false, false, true){}
    ~MirrorBall(){ }
    Ogre::String name() const { return "MirrorBall"; }
    Ogre::Entity* targetEntity() { return m_pTarget; }
    Ogre::Vector3& position() { return m_Position; }
    Ogre::Vector3& direction() { return m_Direction; }
private:
    Ogre::Vector3       m_Position;
    Ogre::Vector3       m_Direction;
    Ogre::Entity*       m_pTarget;
};

class MirrorBallMaker : public ObjectMaker
{
public:
    MirrorBallMaker(){}
    ~MirrorBallMaker(){}
    MirrorBall* create(unsigned int id) const 
    { return new MirrorBall(id); }
};
}
#endif