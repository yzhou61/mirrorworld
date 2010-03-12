//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "../MWGameFramework.h"
#include "MWPlayer.h"

#define M_PI_DEGREE 0.017453f

namespace MirrorWorld {
Player::Player(unsigned int id):Object(id, false, false), m_AnimationSpeed(0.01), testnum(0)
{
}
Player::~Player()
{
    
}

void Player::init(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::Camera* camera)
{
    m_Size = Vector3(30, 50, 20);
    m_Gravity = -9.8;
    m_Mass = 80;
    m_JumpPower = 20;
    m_xStep = 0.15;
    m_yStep = 0.1;
    m_xRotationTotal = 0;
    m_yRotationTotal = 0;
    m_yLimitA = 90;
    m_yLimitB = -60;
    m_Velocity = 0;
    m_SideVel = 0;

    m_pSceneMgr = sceneMgr;
    m_pWorld = world;
    m_pCamera = camera;
    m_pEntity = m_pSceneMgr->createEntity("Player", "robot.mesh");
    m_pSceneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode");
    m_pSceneNode->setPosition(-400, 0.5, 425);
    m_pBodyNode = m_pSceneNode->createChildSceneNode("BodyNode", Vector3::ZERO, Vector3::UNIT_X.getRotationTo(Vector3::NEGATIVE_UNIT_Z));
    m_pBodyNode->attachObject(m_pEntity);

    Ogre::Technique* t = m_pEntity->getSubEntity(0)->getMaterial()->getBestTechnique();
    Ogre::Pass* p = t->getPass(0);
    if (p->hasVertexProgram() && p->getVertexProgram()->isSkeletalAnimationIncluded())
        Ogre::LogManager::getSingleton().logMessage("Hardware skinning is enabled");
    else
        Ogre::LogManager::getSingleton().logMessage("Software skinning is enabled");

    OgreNewt::ConvexCollisionPtr colPtr(
        new OgreNewt::CollisionPrimitives::Ellipsoid(m_pWorld, m_Size, 0, Quaternion::IDENTITY, Vector3(0, m_Size[1], 0)));
    m_pPhyBody = new OgreNewt::Body(m_pWorld, colPtr);
    Vector3 inertia, offset;
    colPtr->calculateInertialMatrix(inertia, offset);
#ifdef OGRENEWT_NO_COLLISION_SHAREDPTR
    delete colPtr;
#endif
    m_pPhyBody->setMassMatrix(m_Mass, m_Mass*inertia);
    m_pPhyBody->setCenterOfMass(offset);
    m_pPhyBody->setAutoSleep(0);
    m_pPhyBody->setStandardForceCallback();
    m_pPhyBody->setPositionOrientation(m_pSceneNode->getPosition(), m_pSceneNode->getOrientation());
    m_pPhyBody->attachNode(m_pSceneNode);

    m_pPlayerControl = new OgreNewt::PlayerController(m_pPhyBody, 0.1);
    m_PhyMatID = new OgreNewt::MaterialID(m_pWorld);
    m_pPhyBody->setMaterialGroupID(m_PhyMatID);
    attachUserData();
    m_pSkeleton = m_pEntity->getSkeleton();
    m_pHeadBone = m_pSkeleton->getBone(11);
    m_pNeckBone = m_pSkeleton->getBone(10);
    m_pNeckBone->setManuallyControlled(true);
    m_pHeadBone->setManuallyControlled(true);

//    m_pSeleton->setBlendMode(Ogre::SkeletonAnimationBlendMode::ANIMBLEND_CUMULATIVE);
    m_pWalkState = m_pEntity->getAnimationState("Walk");
    m_pWalkState->setEnabled(false);
    m_bWalking = false;
    m_WalkingTimeLength = m_pWalkState->getLength();
    m_pSkeleton->getAnimation("Walk")->destroyNodeTrack(m_pNeckBone->getHandle());
    m_pSkeleton->getAnimation("Walk")->destroyNodeTrack(m_pHeadBone->getHandle());
}

void Player::up()
{
    m_Velocity = 160;
    walk();
}

void Player::upRelease()
{
    m_Velocity = 0;
    stop();
}

void Player::down()
{
    m_Velocity = -100;
    walk();
}

void Player::downRelease()
{
    m_Velocity = 0;
    stop();
}

void Player::left()
{
    m_SideVel = -100;
    walk();
}

void Player::leftRelease()
{
    m_SideVel = 0;
    stop();
}

void Player::right()
{
    m_SideVel = 100;
    walk();
}

void Player::rightRelease()
{
    m_SideVel = 0;
    stop();
}

void Player::jump()
{
}

void Player::moveCamera(int xRel, int yRel)
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
        m_pNeckBone->roll(Ogre::Degree(m_yRotation));
//        m_pHeadBone->roll(Ogre::Degree(m_yRotation));
    }
//    m_pNeckBone->yaw(Ogre::Degree(m_xRotation), Ogre::Node ::TS_WORLD);

    m_Heading = Ogre::Radian(m_xRotationTotal*M_PI_DEGREE);
    m_pPlayerControl->setVelocity(m_Velocity, m_SideVel, m_Heading);
}

void Player::walk()
{
    m_bWalking = true;
    m_pWalkState->setEnabled(true);
    m_pWalkState->setLoop(true);
    m_AnimationSpeed = 0.001;
}

void Player::stop()
{
    if (abs(m_Velocity) < 1.0f && abs(m_SideVel) < 1.0f)
    {
        m_bWalking = false;
        m_AnimationSpeed *= 2;
    }
}

void Player::updateAnimation(double timeElasped)
{
    if (!m_bWalking && m_pWalkState->getEnabled())
    {
        float ratio = m_pWalkState->getTimePosition()/m_WalkingTimeLength;
        if ((ratio >= 0.23 && ratio <= 0.27) || (ratio >= 0.73 && ratio <= 0.77))
        {
            m_pWalkState->setEnabled(false);
            m_pWalkState->setTimePosition(0);
        }
    }
    m_pWalkState->addTime(static_cast<Real>(timeElasped)*m_AnimationSpeed);
}

void Player::update(double timeElasped)
{
/*    Quaternion orient;
    Vector3 pos;
    m_pPhyBody->getPositionOrientation(pos, orient);
    OgreNewt::BasicRaycast testRay(m_pWorld, pos-Vector3::UNIT_Y*2*m_Size.y, Vector3::NEGATIVE_UNIT_Y * 2000, false);
    if (testRay.getHitCount() > 0)
        m_Height = testRay.getFirstHit().mDistance*10000;*/
    Vector3 pos;
    Quaternion orient;
    Quaternion pitch(Ogre::Radian(m_yRotationTotal*M_PI_DEGREE), Vector3::UNIT_X);
    m_pPhyBody->getVisualPositionOrientation(pos, orient);
    orient = orient * pitch;
    pos += Vector3(0, m_pPlayerControl->getPlayerHeight() * 0.75f, 0);
    Vector3 bonePos = orient * Vector3::UNIT_Y * 2 + orient * Vector3::NEGATIVE_UNIT_Z * 5 + pos;
    
    m_pCamera->setPosition(bonePos);
    m_pCamera->setOrientation(orient);
    m_pPlayerControl->setVelocity(m_Velocity, m_SideVel, m_Heading);
    updateAnimation(timeElasped);
}

void Player::test()
{
    m_pSkeleton->getBone(testnum)->setManuallyControlled(true);
    m_pSkeleton->getBone(testnum)->translate(0, 0, 10);
}
}