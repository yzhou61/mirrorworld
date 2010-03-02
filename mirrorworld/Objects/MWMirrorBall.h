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
    MirrorBall(unsigned int id):Object(id, false, false){}
    ~MirrorBall(){ }
    Ogre::String name() const { return "MirrorBall"; }
    Ogre::Entity* targetEntity() { return m_pTarget; }
    Ogre::Vector3& targetPoint() { return m_TargetPoint; }
    void update(double timeElasped);
private:
    Ogre::Vector3       m_TargetPoint;
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