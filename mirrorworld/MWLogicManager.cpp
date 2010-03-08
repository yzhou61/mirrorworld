//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWLogicManager.h"

using Ogre::Real;
using Ogre::Vector3;

float MirrorWorld::LogicManager::m_RaycastDistance = 5000;

namespace MirrorWorld
{
MirrorBallNode::MirrorBallNode(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, LogicManager* logicEng):
    m_pSceneMgr(sceneMgr), m_pWorld(world), m_pLogicEngine(logicEng)
{
    ball = static_cast<MirrorBall*>(ObjectFactory::getSingleton().createObj("MirrorBall"));
    model = new MirrorBallModel(m_pSceneMgr);
    m_MirrorBallVelocity = 0.5;
}

bool MirrorBallNode::update(double timeElasped)
{
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
        Vector3 hitPoint = sp + dir * result.mDistance * travelDistance;
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

            return false;
        }
        // Show particle system
        else 
        {
            m_pLogicEngine->hitObstacle(hitPoint, result.mNormal);
            return false;
        }
    }
    // Otherwise, just move on
    else
        ball->position() = ep;
    model->update(ball->position());
    return true;
}

void MirrorBallNode::active(const Ogre::Vector3& pos, const Ogre::Vector3& dir)
{
    ball->position() = pos;
    ball->direction() = dir;
    model->active(pos);
}

void MirrorBallNode::deactive()
{
    model->deactive();
}

UnattachNode::UnattachNode(Ogre::SceneManager* sceneMgr, OgreNewt::World* world):
        m_pSceneMgr(sceneMgr), m_pWorld(world)
{
    model = new UnattachModel(m_pSceneMgr, m_pWorld);
}

bool UnattachNode::update(double timeElasped)
{
    return model->update(timeElasped);
}

void UnattachNode::active(const Ogre::Vector3& pos, const Ogre::Vector3& nor)
{
    model->active(pos, nor);
}

void UnattachNode::deactive()
{
    model->deactive();
}

LogicManager::LogicManager():m_bLaserOn(false), m_pLaser(NULL),m_pLaserModel(NULL),
                    m_pMirrorBallPool(NULL),m_pUnattachPool(NULL)
{
}

void LogicManager::init(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::RenderWindow* window,
                    int maxMirror, Ogre::Camera* camera)
{
    Ogre::LogManager::getSingleton().logMessage("Initializing LogicManager...");
    m_pSceneMgr = sceneMgr;
    m_pWorld = world;
    m_pCamera = camera;
    m_Window = window;
    mirrorCount = 0;

    // Will always has one laser
    m_pLaser = static_cast<Laser*>(ObjectFactory::getSingleton().createObj("Laser"));
    m_pLaser->reset();
    m_bLaserOn = false;
    m_pLaserModel = new LaserModel(m_pSceneMgr);
    
    // Clear up the mirror ball list
    m_MaxMirror = maxMirror;
    m_pMirrorBallPool = new ResourcePool(m_MaxMirror);
    for (int i = 0;i < m_MaxMirror; i++)
        m_pMirrorBallPool->getNodeList()[i] = new MirrorBallNode(m_pSceneMgr, m_pWorld, this);
    m_pUnattachPool = new ResourcePool(m_MaxMirror);
    for (int i = 0;i < m_MaxMirror; i++)
        m_pUnattachPool->getNodeList()[i] = new UnattachNode(m_pSceneMgr, m_pWorld);
    MirrorBallModel::resetCounter();
    UnattachModel::resetCounter();

    m_ShootMode = LASER;
}

void LogicManager::update(double timeElapsed)
{
    calcLaserPath();
    m_pMirrorBallPool->update(timeElapsed);
    m_pUnattachPool->update(timeElapsed);
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
    MirrorBallNode* mirrorBall = static_cast<MirrorBallNode*>(m_pMirrorBallPool->getOneNode());
    mirrorBall->active(m_pCamera->getRealPosition(), m_pCamera->getRealDirection());
}

void LogicManager::hitObstacle(const Ogre::Vector3& pos, const Ogre::Vector3& nor)
{
    UnattachNode* unattachNode = static_cast<UnattachNode*>(m_pUnattachPool->getOneNode());
    unattachNode->active(pos, nor);
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

size_t LogicManager::createMirror(Ogre::Vector3 normal, Ogre::Vector3 position, Ogre::Vector3 up,
		size_t width, size_t height) {
	Ogre::LogManager::getSingleton().logMessage("second mirror?");
	Mirror *curMirror = static_cast<Mirror *>(ObjectFactory::getSingleton().createObj("Mirror"));
	curMirror->init(m_pSceneMgr, normal, position, up, width, height, m_pCamera);

	m_MirrorList[mirrorCount] = curMirror;
	return mirrorCount++;
}

void LogicManager::resetMirrors() {
	for (size_t i = 0; i < mirrorCount; ++i) {
		m_MirrorList[i]->resetResource();
	}
}

void LogicManager::updateMirrors(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Vector3 up,
        Ogre::Real fLeft, Ogre::Real fRight, Ogre::Real fTop, Ogre::Real fBottom, size_t mirrorID) 
{
    for (size_t i = 0; i < mirrorCount; ++i) 
    {
        Mirror *m = m_MirrorList[i];
        if (m->getID() == mirrorID)
            continue;
        if (m->getNormal().dotProduct(position - m->getCenterPosition()) <= 0)
            continue;
        bool cont = m->setEye(position, direction, up, fLeft, fRight, fTop, fBottom);
        if (cont) 
        {
			 //		  if (mirrorID == 0) {
			 //			  GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(m->getPosition()), 0);
			 //			  GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(m->getDirection()), 1);
			 //			  GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(position) + " "
			 //				  + Ogre::StringConverter::toString(direction), 2);
			 //		  }
            Ogre::Real ffLeft, ffRight, ffTop, ffBottom;
            m->getEye()->getFrustumExtents(ffLeft, ffRight, ffTop, ffBottom);
            updateMirrors(m->getPosition(), m->getDirection(), m->getUp(), -ffRight, -ffLeft, ffTop, ffBottom, m->getID());
            m->update();
        }
    }
    setRealReflections(position, direction, fLeft, fRight, fTop, fBottom);
}

void LogicManager::setRealReflections(Ogre::Vector3 position, Ogre::Vector3 direction,
		Ogre::Real fLeft, Ogre::Real fRight, Ogre::Real fTop, Ogre::Real fBottom) 
{
    for (size_t i = 0; i < mirrorCount; ++i) 
    {
        Mirror *m = m_MirrorList[i];
        if (m->getNormal().dotProduct(position - m->getCenterPosition()) <= 0)
            continue;
        m->reflectReal();
    }
}
}