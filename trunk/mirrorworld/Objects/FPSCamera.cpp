//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "FPSCamera.h"

FPSCamera::FPSCamera(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::Camera* camera)
{
    if((m_SceneMgr = sceneMgr) == NULL)
        Ogre::LogManager::getSingleton().logMessage("SceneMgr is null in Player");

    if((m_World = world) == NULL)
        Ogre::LogManager::getSingleton().logMessage("World is null in Player");

    if((m_Camera = camera) == NULL)
        Ogre::LogManager::getSingleton().logMessage("Camera is null in Player");

    m_Size = Vector3(30, 30, 30);
    m_Gravity = -9.8;
    m_Mass = 40;
    m_JumpPower = 20;
    m_FPSSpeed = 20;
    m_xStep = 1.0;
    m_yStep = 1.0;
    m_xRotationTotal = 0;
    m_yRotationTotal = 0;
    m_yLimitA = 60;
    m_yLimitB = -60;
    m_Velocity = 0;
    m_SideVel = 0;
    
    m_CameraNode = m_SceneMgr->getRootSceneNode()->createChildSceneNode();
    m_CameraNode->setScale(m_Size);
    m_CameraNode->setPosition(Vector3(40,30,40));

    OgreNewt::ConvexCollisionPtr camCollision(
        new OgreNewt::CollisionPrimitives::Ellipsoid(m_World, m_Size, 0));
    m_CamBody = new OgreNewt::Body(m_World, camCollision);
    
    Vector3 inertia, offset;
    camCollision->calculateInertialMatrix(inertia, offset);
#ifdef OGRENEWT_NO_COLLISION_SHAREDPTR
    delete camCollision;
#endif
    m_CamBody->setMassMatrix(m_Mass, m_Mass*inertia);
    m_CamBody->setCenterOfMass(offset);
    m_CamBody->setAutoSleep(0);
//    m_CamBody->setStandardForceCallback();
    m_CamBody->setCustomForceAndTorqueCallback<FPSCamera>(&FPSCamera::cameraForceCallback, this);
    m_CamBody->setPositionOrientation(m_CameraNode->getPosition(), Ogre::Quaternion::IDENTITY);
    m_CamBody->attachNode(m_CameraNode);
    
    m_CameraViewNode = m_CameraNode->createChildSceneNode(Vector3(0, 5, 0));
    m_CameraViewNode->attachObject(m_Camera);

    m_PhyMatID = new OgreNewt::MaterialID(m_World);
    m_CamBody->setMaterialGroupID(m_PhyMatID);
}

FPSCamera::~FPSCamera()
{

}

void FPSCamera::cameraForceCallback(OgreNewt::Body *body, float timeStep, int threadIndex)
{
    Real mass;
    Vector3 inertia;

    body->getMassMatrix(mass, inertia);
    // Gravity
    Vector3 gravityForce(0, m_Gravity, 0);

    Quaternion orient(Ogre::Degree(m_xRotationTotal), Vector3::UNIT_Y);
//    Vector3 pos;
//    m_CamBody->getPositionOrientation(pos, orient);

    Vector3 direction = orient * Vector3::NEGATIVE_UNIT_Z;
    Vector3 sideDir = orient * Vector3::NEGATIVE_UNIT_X;
//    Ogre::LogManager::getSingleton().stream()<<direction<<sideDir;

    Vector3 v0 = body->getVelocity();
    Vector3 v1 = direction*m_Velocity+sideDir*m_SideVel;
    Vector3 acc = (v1 - v0)/timeStep;

    Vector3 force(static_cast<Real>(acc.x), gravityForce.y, static_cast<Real>(acc.z));
    force *= m_Mass;

    body->addForce(force);
    body->setOmega(Vector3(m_xRotation, 0, 0));
}

void FPSCamera::moveCamera(int xRel, int yRel)
{
    m_xRotation = -xRel * m_xStep;
    m_yRotation = -yRel * m_yStep;

    Real newxVal = m_xRotationTotal + m_xRotation;
    Real newyVal = m_yRotationTotal + m_yRotation;
    if (newxVal > 360)
        newxVal -= 360;
    m_xRotationTotal = newxVal;
    
    if (newyVal <= m_yLimitA && newyVal >= m_yLimitB)
    {
        m_yRotationTotal = newyVal;
        m_CameraViewNode->pitch(Ogre::Degree(m_yRotation), Ogre::Node ::TS_LOCAL);
    }
    m_CameraViewNode->setFixedYawAxis(true);
    m_CameraViewNode->yaw(Ogre::Degree(m_xRotation), Ogre::Node ::TS_WORLD);
}

void FPSCamera::up()
{
    m_Velocity = m_FPSSpeed*10;
}

void FPSCamera::upRelease()
{
    m_Velocity = 0;
}

void FPSCamera::down()
{
    m_Velocity = -m_FPSSpeed*10;
}

void FPSCamera::downRelease()
{
    m_Velocity = 0;
}

void FPSCamera::left()
{
    m_SideVel = m_FPSSpeed*5;
}

void FPSCamera::leftRelease()
{
    m_SideVel = 0;
}

void FPSCamera::right()
{
    m_SideVel = -m_FPSSpeed*5;
}

void FPSCamera::rightRelease()
{
    m_SideVel = 0;
}

void FPSCamera::jump()
{
    if (m_Height < 100)
        m_CamBody->setVelocity(m_CamBody->getVelocity()*Vector3(1, 0, 1)+Vector3(0, 1, 0)*m_JumpPower);
}

void FPSCamera::update()
{
    Quaternion orient;
    Vector3 pos;
    m_CamBody->getPositionOrientation(pos, orient);
    OgreNewt::BasicRaycast camRay(m_World, pos-Vector3::UNIT_Y*2*m_Size.y, Vector3::NEGATIVE_UNIT_Y * 2000, false);
    if (camRay.getHitCount() > 0)
        m_Height = camRay.getFirstHit().mDistance*10000;

    m_xRotation = 0;
    m_yRotation = 0;
}