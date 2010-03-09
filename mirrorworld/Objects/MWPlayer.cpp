//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWPlayer.h"

namespace MirrorWorld {
Player::Player(unsigned int id):Object(id, false, false)
{
    m_AnimationSpeed = 0.001;
    m_BoneNameConverter["Neck"] = 1;
    m_BoneNameConverter["Head"] = 2;
}
Player::~Player()
{

}

void Player::init(Ogre::SceneManager* sceneMgr, OgreNewt::World* world)
{
    m_pSceneMgr = sceneMgr;
    m_pWorld = world;
    m_pEntity = m_pSceneMgr->createEntity("Player", "robot.mesh");
    m_pSeleton = m_pEntity->getSkeleton();
    Ogre::SceneNode* sceneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode");
    sceneNode->setPosition(200, 0.25, 0);
    sceneNode->attachObject(m_pEntity);

    for (int i = 0;i < m_pSeleton->getNumBones(); i++)
        Ogre::LogManager::getSingleton().stream()<<m_pSeleton->getBone(i)->getName();
    
    m_pSeleton->setBlendMode(Ogre::SkeletonAnimationBlendMode::ANIMBLEND_CUMULATIVE);
    m_pCurState = m_pEntity->getAnimationState("Walk");
    m_pCurState->setEnabled(true);
    m_pCurState->setLoop(true);
}

void Player::updateAnimation(double timeElasped)
{
    m_pCurState->addTime(static_cast<Real>(timeElasped)*m_AnimationSpeed);
}
}