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
    m_pParticleSys->getEmitter(0)->setDirection(normal);
    if (m_pParticleSys->getNumAffectors() ==  3)
        m_pParticleSys->removeAffector(2);
    Ogre::ParticleAffector* groundAffector =  m_pParticleSys->addAffector("DeflectorPlane");
    Ogre::NameValuePairList pairList;
    pairList["plane_point"] = "	0 0 0";
    pairList["plane_normal"] = "0 1 0";
    pairList["bounce"] = "0.8989";
    groundAffector->setParameterList(pairList);
    m_pSceneNode->attachObject(m_pParticleSys);
    m_LifeTime = 0.0;
    m_bActive = true;
}

void UnattachModel::deactive()
{
    m_pSceneNode->detachAllObjects();
    m_bActive = false;
}

bool UnattachModel::update(double timeElasped)
{
    m_LifeTime += timeElasped;
    if (m_LifeTime > m_pParticleSys->getEmitter(0)->getRepeatDelay())
        deactive();
    return m_bActive;
}

void UnattachModel::destroy()
{
    
}
}