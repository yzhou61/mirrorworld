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
    Laser(unsigned int id) :Object(id, false, false) {}
    ~Laser() {}
    Ogre::String name() const { return "Laser"; }
    void reset() { m_ContactPointList.clear(); }
    std::vector<Ogre::Vector3>& contactPoints() { return m_ContactPointList; }
    void setEntity(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::SceneNode* node, Ogre::Entity* entity) 
    { }
    void update(double timeElasped);
private:
    std::vector<Ogre::Vector3> m_ContactPointList;
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