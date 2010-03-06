//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWLogicManager.h"

using Ogre::Real;
using Ogre::Vector3;

float MirrorWorld::LogicManager::m_RaycastDistance = 5000;

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

    m_MirrorBallVelocity = 0.1;
    m_MaxMirror = maxMirror;

    m_ShootMode = LASER;

    m_pLaserModel = new LaserModel(m_pSceneMgr);
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
    if (m_bLaserOn)
        calcLaserPath();
    else
        m_pLaserModel->deactive();
}

void LogicManager::shootMirrorBall()
{
    MirrorBall* newBall = static_cast<MirrorBall*>(ObjectFactory::getSingleton().createObj("MirrorBall"));
    if (mirrorSize() > m_MaxMirror)
        destroyMirrorBall(m_MirrorBallListBegin);
    m_MirrorBallList[m_MirrorBallListEnd] = newBall;
    // Setup init postion and direction
    newBall->position() = m_pCamera->getRealPosition();
    newBall->direction() = m_pCamera->getRealDirection();

    MirrorBallModel* newBallModel =  new MirrorBallModel(m_pSceneMgr, newBall->position());
    m_MirrorBallModelList[m_MirrorBallListEnd] = newBallModel;
    m_MirrorBallListEnd = (m_MirrorBallListEnd+1)%MAX_MIRROR;
}

void LogicManager::destroyMirrorBall(int idx)
{
    // TO DO
}

void LogicManager::calcLaserPath()
{
    if (!m_bLaserOn)
        return;

//    Ogre::Ray firstRay = m_pCamera->getCameraToViewportRay(0.5, 0.5);
    Vector3 sp = m_pCamera->getRealPosition();
    Vector3 dir = m_pCamera->getRealDirection();
    m_pLaser->reset();
    m_pLaser->contactPoints().push_back(sp);
    for (int i = 0;i < LaserModel::MAX_LASERBEAM; i++)
    {
        dir.normalise();
/*        Ogre::LogManager::getSingleton().stream()<<"Laser"<<i<<" sp"<<sp;
        Ogre::LogManager::getSingleton().stream()<<"Laser"<<i<<" dir"<<dir;*/
        OgreNewt::BasicRaycast testRay(m_pWorld, sp, sp+dir*m_RaycastDistance, true);
        if (testRay.getHitCount() > 0)
        {
            OgreNewt::BasicRaycast::BasicRaycastInfo result = testRay.getFirstHit();
            try
            {
                Object* hitobj = Ogre::any_cast<Object*>(result.mBody->getUserData());
                Vector3 ep = sp + dir * (result.mDistance * m_RaycastDistance - 0.01f);
                /*                Ogre::LogManager::getSingleton().logMessage(hitobj->nameID());
                Ogre::LogManager::getSingleton().stream()<<"Laser ep normal"<<ep<<result.mNormal;*/
                if (hitobj->isReflective())
                {
                    sp = ep;
                    dir = dir.reflect(result.mNormal);
                    m_pLaser->contactPoints().push_back(ep);
                }
                else
                {
                    m_pLaser->contactPoints().push_back(ep);
                    break;
                }
            }
            catch(Ogre::Exception)
            {
                m_pLaser->contactPoints().push_back(sp = sp + dir * m_RaycastDistance);
                //                Ogre::LogManager::getSingleton().logMessage("Laser hit nothing case1, wired");
                break;
            }
        }
        else
        {
            m_pLaser->contactPoints().push_back(sp = sp + dir * m_RaycastDistance);
//            Ogre::LogManager::getSingleton().logMessage("Laser hit nothing case2, wired");
            break;
        }
    }
    if (m_pLaser->contactPoints().size() > 1)
    {
        m_pLaser->contactPoints()[0] -= Vector3(0, 10, 0);
        m_pLaserModel->update(m_pLaser->contactPoints());
    }
}

void LogicManager::updateMirrorBalls(double timeElasped)
{
    if (m_MirrorBallListBegin <= m_MirrorBallListEnd)
    {
        for (int i = m_MirrorBallListBegin; i < m_MirrorBallListEnd; i++)
            updateMirrorBall(i, timeElasped);
    }
    else
    {
        for (int i = m_MirrorBallListBegin; i < MAX_MIRROR; i++)
            updateMirrorBall(i, timeElasped);
        for (int i = 0;i < m_MirrorBallListEnd; i++)
            updateMirrorBall(i, timeElasped);
    }
}

void LogicManager::updateMirrorBall(int idx, double timeElasped)
{
    MirrorBall* ball = m_MirrorBallList[idx];
    Vector3& sp = ball->position();
    // Notice: dir should be normalized before
    Vector3& dir = ball->direction();
    Real travelDistance = static_cast<Real>(m_MirrorBallVelocity*timeElasped);
    Vector3 ep = sp + dir * travelDistance;
    
    OgreNewt::BasicRaycast testRay(m_pWorld, sp, ep, true);
    if (testRay.getHitCount() > 0)
    {
        OgreNewt::BasicRaycast::BasicRaycastInfo result = testRay.getFirstHit();
        Object* hitobj = Ogre::any_cast<Object*>(result.mBody->getUserData());
//            Ogre::LogManager::getSingleton().logMessage(hitobj->nameID());
//            Ogre::LogManager::getSingleton().stream()<<"MirrorBall"<<idx<<sp;
//            Ogre::LogManager::getSingleton().stream()<<"MirrorBall"<<idx<<ep;
        // Change position and dir
        if (hitobj->isReflective())
        {
            Vector3 hitPoint = sp + dir * result.mDistance * travelDistance;
            dir = dir.reflect(result.mNormal);
            dir.normalise();
            sp = hitPoint + dir * (1.0f - result.mDistance) * travelDistance;
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
    m_MirrorBallModelList[idx]->update(ball->position());
}

}