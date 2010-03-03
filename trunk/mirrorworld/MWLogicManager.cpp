//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWLogicManager.h"

using Ogre::Real;
using Ogre::Vector3;

namespace MirrorWorld
{
void LogicManager::init(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, int maxMirror, Ogre::Camera* camera)
{
    Ogre::LogManager::getSingleton().logMessage("Initializing LogicManager...");
    m_pSceneMgr = sceneMgr;
    m_pWorld = world;
    m_pCamera = camera;

    // Will always has one laser
    m_pLaser = static_cast<Laser*>(ObjectFactory::getSingleton().createObj("Laser"));
    m_pLaser->reset();
    m_bLaserOn = false;
    
    // Clear up the mirror ball list
    for (int i = m_MirrorBallListBegin; i < m_MirrorBallListEnd; i++)
        delete m_MirrorBallList[i];
    m_MirrorBallListBegin = m_MirrorBallListEnd = 0;

    m_MirrorBallVelocity = 20.0;
    m_MaxMirror = maxMirror;
    // Setup RayQuery
/*    m_TestRayQuery = m_pSceneMgr->createRayQuery(m_TestRay);
    m_TestRayQuery->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
    m_TestRayQuery->setSortByDistance(true);
    m_TestRayQuery->setWorldFragmentType(Ogre::SceneQuery::WFT_SINGLE_INTERSECTION);*/
}

void LogicManager::update(double timeElapsed)
{
    calcLaserPath();
    updateMirrorBalls(timeElapsed);
}

int LogicManager::mirrorSize()
{
    return m_MirrorBallListBegin < m_MirrorBallListEnd ? 
            m_MirrorBallListEnd-m_MirrorBallListBegin : 
            m_MirrorBallListEnd+MAX_MIRROR-m_MirrorBallListBegin;
}

void LogicManager::triggerLaser()
{
    m_bLaserOn = !m_bLaserOn;
}

void LogicManager::createMirrorBall()
{
    MirrorBall* newBall = m_MirrorBallList[(m_MirrorBallListEnd+1)%MAX_MIRROR];
    if (mirrorSize() > m_MaxMirror)
        destroyMirrorBall(m_MirrorBallListBegin);
    newBall = static_cast<MirrorBall*>(ObjectFactory::getSingleton().createObj("MirrorBall"));
    // Setup init postion and direction

}

void LogicManager::destroyMirrorBall(int idx)
{
    // TO DO
}

void LogicManager::calcLaserPath()
{
    if (!m_bLaserOn)
        return;

    Ogre::Ray firstRay = m_pCamera->getCameraToViewportRay(0.5, 0.5);
    Vector3 sp = firstRay.getOrigin();
    Vector3 dir = firstRay.getDirection();
    m_pLaser->reset();
    m_pLaser->contactPoints().push_back(sp);
    for ( ; ; )
    {
        dir.normalise();
        OgreNewt::BasicRaycast testRay(m_pWorld, sp, sp+dir*20000, true);
        if (testRay.getHitCount() > 0)
        {
            OgreNewt::BasicRaycast::BasicRaycastInfo result = testRay.getFirstHit();
            Object* hitobj = Ogre::any_cast<Object*>(result.mBody->getUserData());
            if (hitobj)
            {
                Ogre::LogManager::getSingleton().logMessage(hitobj->nameID());
                Ogre::LogManager::getSingleton().stream()<<"Laser"<<sp;
                Ogre::LogManager::getSingleton().stream()<<"Laser"<<dir;
                if (hitobj->isReflective())
                {
                    sp = sp + dir * result.mDistance;
                    dir = (-dir).reflect(result.mNormal);
                    m_pLaser->contactPoints().push_back(sp);
                }
                else
                {
                    m_pLaser->contactPoints().push_back(sp = sp + dir * result.mDistance);
                    break;
                }
            }
            else
            {
                Ogre::LogManager::getSingleton().logMessage("Laser hit nothing case1, wired");
                break;
            }
        }
        else
        {
            Ogre::LogManager::getSingleton().logMessage("Laser hit nothing case2, wired");
            break;
        }
    }
}

void LogicManager::updateMirrorBalls(double timeElasped)
{
    if (m_MirrorBallListBegin <= m_MirrorBallListEnd)
    {
        for (int i = m_MirrorBallListBegin; i < m_MirrorBallListEnd; i++)
            updateMirrorBall(m_MirrorBallList[i], timeElasped);
    }
    else
    {
        for (int i = m_MirrorBallListBegin; i < MAX_MIRROR; i++)
            updateMirrorBall(m_MirrorBallList[i], timeElasped);
        for (int i = 0;i < m_MirrorBallListEnd; i++)
            updateMirrorBall(m_MirrorBallList[i], timeElasped);
    }
}

void LogicManager::updateMirrorBall(MirrorBall* ball, double timeElasped)
{
    Vector3 sp = ball->position();
    Vector3 dir = ball->direction();
    Real travelDistance = static_cast<Real>(m_MirrorBallVelocity*timeElasped);
    Vector3 ep = sp + dir * travelDistance;
    
    OgreNewt::BasicRaycast testRay(m_pWorld, sp, ep, true);
    if (testRay.getHitCount() > 0)
    {
        OgreNewt::BasicRaycast::BasicRaycastInfo result = testRay.getFirstHit();
        Object* hitobj = Ogre::any_cast<Object*>(result.mBody->getUserData());
//            Ogre::LogManager::getSingleton().logMessage(hitobj->nameID());
//            Ogre::LogManager::getSingleton().stream()<<"MirrorBall"<<i<<sp;
//            Ogre::LogManager::getSingleton().stream()<<"MirrorBall"<<i<<ep;
        // Change position and dir
        if (hitobj->isReflective())
        {
            Vector3 hitPoint = sp + dir * result.mDistance;
            dir = (-dir).reflect(result.mNormal);
            dir.normalise();
            sp = hitPoint + dir * (travelDistance - result.mDistance);
        }
        // Attach 
        else if (hitobj->isAttachable())
        {

        }
        // Show particle system
        else 
        {

        }
    }
    // Otherwise, just move on
    else
        ball->position() = ep;
}

}