//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWLogicManager.h"
#include "MWGameFramework.h"

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
        Ogre::LogManager::getSingleton().stream()<<result.mBody->getOgreNode()->getName();
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
            
            m_pLogicEngine->showMirror(hitPoint, result.mNormal, dir);
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

LogicManager::~LogicManager() {
    delete[] m_MirrorList;
}

void LogicManager::init(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::RenderWindow* window,
	int maxMirror, Ogre::Camera* camera)
{
    Ogre::LogManager::getSingleton().logMessage("Initializing LogicManager...");
    m_pSceneMgr = sceneMgr;
    m_pWorld = world;
    m_pCamera = camera;
	m_Window = window;

    // Will always has one laser
    m_pLaser = static_cast<Laser*>(ObjectFactory::getSingleton().createObj("Laser"));
    m_pLaser->reset();
    m_bLaserOn = false;
    m_pLaserModel = new LaserModel(m_pSceneMgr);
    
    m_MaxMirrors = maxMirror + 1;
    m_CurMirrorIndex = 0;
    m_MirrorList = new Mirror*[m_MaxMirrors];

    for (int i = 0; i < m_MaxMirrors; ++i) {
        Mirror *curMirror = static_cast<Mirror *>(ObjectFactory::getSingleton().createObj("Mirror"));
        curMirror->init(m_pSceneMgr, m_pCamera, m_pWorld);

        m_MirrorList[i] = curMirror;
    }

    // Clear up the mirror ball list
    m_pMirrorBallPool = new ResourcePool(maxMirror);
    for (int i = 0;i < maxMirror; i++)
        m_pMirrorBallPool->getNodeList()[i] = new MirrorBallNode(m_pSceneMgr, m_pWorld, this);
    m_pUnattachPool = new ResourcePool(maxMirror);
    for (int i = 0;i < maxMirror; i++)
        m_pUnattachPool->getNodeList()[i] = new UnattachNode(m_pSceneMgr, m_pWorld);
    MirrorBallModel::resetCounter();
    UnattachModel::resetCounter();
}

void LogicManager::update(double timeElapsed)
{
    calcLaserPath();
    m_pMirrorBallPool->update(timeElapsed);
    m_pUnattachPool->update(timeElapsed);

    Ogre::Real fLeft, fRight, fTop, fBottom;
    m_pCamera->getFrustumExtents(fLeft, fRight, fTop, fBottom);
    preUpdateMirrors();
    updateMirrors(m_pCamera->getRealPosition(), m_pCamera->getRealDirection(), m_pCamera->getRealUp(),
        fLeft, fRight, fTop, fBottom, -1);
    postUpdateMirrors(timeElapsed);
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

void LogicManager::preUpdateMirrors() {
	for (int i = 0; i < m_MaxMirrors; ++i) {
        if (m_MirrorList[i]->isActivated())
		    m_MirrorList[i]->preUpdate();
	}
}

void LogicManager::postUpdateMirrors(double timeElapsed) {
    for (int i = 0; i < m_MaxMirrors; ++i) {
        if (m_MirrorList[i]->isActivated())
            m_MirrorList[i]->postUpdate(timeElapsed);
    }
}

void LogicManager::updateMirrors(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Vector3 up,
	Ogre::Real fLeft, Ogre::Real fRight, Ogre::Real fTop, Ogre::Real fBottom, size_t mirrorID) {

	for (int i = 0; i < m_MaxMirrors; ++i) {

		Mirror *m = m_MirrorList[i];
        if (!m->shouldUpdate())
            continue;

		if (m->getID() == mirrorID) {
			continue;
		}
		if (m->getNormal().dotProduct(position - m->getCenterPosition()) <= 0) {
			continue;
		}
		bool cont = m->setEye(position, direction, up, fLeft, fRight, fTop, fBottom);
		if (cont) {
			Ogre::Real ffLeft, ffRight, ffTop, ffBottom;
			m->getEyeFrustum(ffLeft, ffRight, ffTop, ffBottom);
			updateMirrors(m->getPosition(), m->getDirection(), m->getUp(), -ffRight, -ffLeft, ffTop, ffBottom, m->getID());
			m->update();
		}
	}
	setRealReflections(position, direction, fLeft, fRight, fTop, fBottom);
}

void LogicManager::setRealReflections(Ogre::Vector3 position, Ogre::Vector3 direction,
		Ogre::Real fLeft, Ogre::Real fRight, Ogre::Real fTop, Ogre::Real fBottom) {

	for (int i = 0; i < m_MaxMirrors; ++i) {
        
		Mirror *m = m_MirrorList[i];
        if (!m->shouldUpdate())
            continue;
		if (m->getNormal().dotProduct(position - m->getCenterPosition()) <= 0)
			continue;
		m->reflectReal();
	}
}

void LogicManager::showMirror(Ogre::Vector3 position, Ogre::Vector3 normal, Ogre::Vector3 hitDirection) {
    int toSuspend = (m_CurMirrorIndex + 1) % m_MaxMirrors;
    m_MirrorList[toSuspend]->suspend();

    Vector3 up;
    if ((abs(normal.angleBetween(Vector3::UNIT_Y).valueDegrees()) < 1) || (abs(normal.angleBetween(Vector3::NEGATIVE_UNIT_Y).valueDegrees()) < 1)) {
        up = normal.crossProduct(hitDirection.crossProduct(normal));
    } else {
        up = normal.crossProduct(Vector3::UNIT_Y.crossProduct(normal));
    }
    m_MirrorList[m_CurMirrorIndex]->activate(normal, position + normal, up);
    m_CurMirrorIndex = (m_CurMirrorIndex + 1) % m_MaxMirrors;
}

}