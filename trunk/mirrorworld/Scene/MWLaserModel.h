//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_LASER_MODEL_H_
#define _MW_LASER_MODEL_H_

#include <Ogre.h>
#include <vector>

namespace MirrorWorld{

class LaserModel;
class LaserBeam
{
public:
    LaserBeam() : m_bCreated(false), m_pSceneNode(NULL), m_pModel(NULL) { }
    ~LaserBeam() { }
    void init(Ogre::SceneManager* sceneMgr, Ogre::MaterialPtr, int i);
    void destroy(Ogre::SceneManager* sceneMgr);
    void update(Ogre::Vector3 sp, Ogre::Vector3 ep);
    void active();
    void deactive();
private:
    bool                m_bCreated;
    Ogre::SceneNode*    m_pSceneNode;
    Ogre::ManualObject* m_pModel;
    Ogre::MaterialPtr   m_pMaterial;
};

class LaserModel
{
public:
    LaserModel(Ogre::SceneManager* sceneMgr, Ogre::String materialName);
    ~LaserModel();
    void active();
    void deactive();
    void update(std::vector<Ogre::Vector3>& contactPointList);
    static const int            MAX_LASERBEAM = 15;
    static int                  m_nPlanes;
    static Ogre::Real           m_Radius;
private:
    Ogre::SceneManager*         m_pSceneMgr;
    Ogre::MaterialPtr           m_pMaterial;
    int                         m_nUseLaser;
    LaserBeam                   m_LaserBeamList[MAX_LASERBEAM];
};

}

#endif