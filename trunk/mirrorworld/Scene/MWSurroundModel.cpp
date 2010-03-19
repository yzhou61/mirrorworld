//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWSurroundModel.h"

int MirrorWorld::SurroundModel::m_nUniqueID = 0;

namespace MirrorWorld
{
SurroundModel::SurroundModel(Ogre::SceneManager* sceneMgr)
{
    Ogre::String id = Ogre::StringConverter::toString(m_nUniqueID++);
    m_pSceneMgr = sceneMgr;
    m_pParticleSys = m_pSceneMgr->createParticleSystem("SurroundParticle"+id, "Surround");
    m_pSceneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("SurroundParticleNode"+id);
    m_bLive = false;
}

void SurroundModel::active(const Ogre::Vector3& pos, const Ogre::Vector3& normal, const Ogre::Vector3& up)
{
    if (m_bLive)
        return;
    Ogre::Quaternion q = Ogre::Vector3::UNIT_Z.getRotationTo(normal);
    Ogre::Vector3 newUp = q * Ogre::Vector3::UNIT_Y;
    m_pSceneNode->resetToInitialState();
    m_pSceneNode->rotate(newUp.getRotationTo(up));
    m_pSceneNode->rotate(q);
    m_pSceneNode->setPosition(pos);
    m_pParticleSys->clear();
    for (int i = 0;i < 4; i++)
        m_pParticleSys->getEmitter(i)->setEnabled(true);
    m_pSceneNode->attachObject(m_pParticleSys);
    m_bLive = true;
}

void SurroundModel::deactive()
{
    m_pSceneNode->detachAllObjects();
    m_bLive = false;
}

bool SurroundModel::update(double timeElasped)
{
    return m_bLive;
}

void SurroundModel::destroy()
{
    Ogre::LogManager::getSingleton().logMessage("Destroy SurroundModel");
    m_pSceneMgr->destroyParticleSystem(m_pParticleSys);
    m_pSceneMgr->destroySceneNode(m_pSceneNode);
}

Ogre::SceneNode *SurroundModel::getNode() {
    return m_pSceneNode;
}
}