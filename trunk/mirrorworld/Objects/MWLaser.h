//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_LASER_H_
#define _MW_LASER_H_

#include <vector>
#include "MWObject.h"

namespace MirrorWorld{
class Laser : public Object
{
public:
    Laser(unsigned int id) :Object(id, false, false, true) {}
    ~Laser() {}
    Ogre::String name() const { return "Laser"; }
    void reset() { m_ContactPointList.clear(); m_DistanceTravelled = 0; }
    std::vector<Ogre::Vector3>& contactPoints() { return m_ContactPointList; }
    void setEntity(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::SceneNode* node, Ogre::Entity* entity) 
    { }
private:
    std::vector<Ogre::Vector3> m_ContactPointList;
    float                       m_DistanceTravelled;
}; // End of Laser

class LaserMaker : public ObjectMaker
{
public:
    LaserMaker(){}
    ~LaserMaker(){}
    Laser* create(unsigned int id) const 
    { return new Laser(id); }
}; // End of LaserMaker
} // End of MirrorWorld
#endif