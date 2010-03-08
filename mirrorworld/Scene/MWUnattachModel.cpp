//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWUnattachModel.h"

int MirrorWorld::UnattachModel::m_nUniqueID = 0;

namespace MirrorWorld
{
UnattachModel::UnattachModel(Ogre::SceneManager* sceneMgr, OgreNewt::World* world)
{
    Ogre::String id = Ogre::StringConverter::toString(m_nUniqueID++);
    m_pSceneMgr = sceneMgr;
    m_pWorld = world;
    m_pParticleSys = m_pSceneMgr->createParticleSystem("UnattachParticle"+id, "Unattach");
    m_pSceneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("UnattachParticleNode"+id);
}

void UnattachModel::active(const Ogre::Vector3& pos, const Ogre::Vector3& normal)
{
    m_pSceneNode->setPosition(pos);
    m_pParticleSys->clear();
    m_pParticleSys->getEmitter(0)->setEnabled(true);
    m_pParticleSys->getEmitter(0)->setPosition(Ogre::Vector3::ZERO);
    m_pParticleSys->getEmitter(0)->setDirection(normal);
    if (m_pParticleSys->getNumAffectors() ==  3)
        m_pParticleSys->removeAffector(2);

    OgreNewt::BasicRaycast testRay(m_pWorld, pos, Ogre::Vector3::NEGATIVE_UNIT_Y * 2000, true);
    if (testRay.getHitCount() > 0)
    {
        Ogre::Vector3 plane = Ogre::Vector3::ZERO;
        plane.y = pos.y - testRay.getFirstHit().mDistance*2000;
        Ogre::ParticleAffector* groundAffector =  m_pParticleSys->addAffector("DeflectorPlane");
        Ogre::NameValuePairList pairList;
        pairList["plane_point"] = Ogre::StringConverter::toString(plane);
        pairList["plane_normal"] = "0 1 0";
        pairList["bounce"] = "1.00";
        groundAffector->setParameterList(pairList);
    }
    m_pSceneNode->attachObject(m_pParticleSys);
    m_LifeTime = 0.0;
}

void UnattachModel::deactive()
{
    m_pSceneNode->detachAllObjects();
}

bool UnattachModel::update(double timeElasped)
{
    m_LifeTime += timeElasped;
    if (m_LifeTime > m_pParticleSys->getEmitter(0)->getRepeatDelay()*1000.0)
        return false;
    return true;
}

void UnattachModel::destroy()
{
    
}
}