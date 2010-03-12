//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_TRIGGER_H_
#define _MW_TRIGGER_H_

#include "MWObject.h"
#include <vector>

namespace MirrorWorld
{
using Ogre::Vector3;
class Trigger : public Object
{
public:
    Trigger(unsigned int id) : Object(id, false, false, false, true) { }
    ~Trigger(){}
    void initTrigger(const Vector3& pos, const Vector3& nor, const Vector3& up);
    void setTarget(Object* obj, const Ogre::Vector3& translate, Ogre::Real timeLength);
    Ogre::String name() const { return "Trigger"; }
    void trigger();

    void update(double timeElasped);

    std::vector<Vector3> path;
private:
    Object* m_pObject;
    Ogre::AnimationState* m_pState;
    double  m_TimeLength;
    double  m_Acctime;
    Ogre::Vector3   m_Translate;
    bool activated;
};

class TriggerMaker : public ObjectMaker
{
public:
    TriggerMaker(){}
    ~TriggerMaker(){}
    Trigger* create(unsigned int id) const 
    { Trigger* trigger = new Trigger(id); return trigger; }
}; // End of WallMaker

}

#endif