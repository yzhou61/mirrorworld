//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _FPS_CAMERA_H_
#define _FPS_CAMERA_H_

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreNewt.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <dVector.h>
#include <CustomUpVector.h>

using Ogre::Real;
using Ogre::Vector3;
using Ogre::Quaternion;

class UpVector : public OgreNewt::Joint
{
public:
    UpVector(const OgreNewt::Body* body, const Vector3& pin );
    ~UpVector();
    void setPin( const Vector3& pin );
    const Vector3& getPin() const;
private:
    Vector3 m_pin;
};

class FPSCamera
{
public:
    FPSCamera(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::Camera* camera);
    ~FPSCamera();
    void moveCamera(int xRel, int yRel);
    void up();
    void upRelease();
    void down();
    void downRelease();
    void left();
    void leftRelease();
    void right();
    void rightRelease();
    void update();
    void jump();
    OgreNewt::MaterialID* getPhyMat() { return m_PhyMat; }
private:
    void cameraForceCallback(OgreNewt::Body *body, float timeStep, int threadIndex);
    Ogre::SceneManager*     m_SceneMgr;
    OgreNewt::World*        m_World;
    OgreNewt::MaterialID*   m_PhyMat;
    Ogre::Camera*       m_Camera;
    Ogre::SceneNode*    m_CameraNode;
    Ogre::SceneNode*    m_CameraViewNode;

    OgreNewt::Body*     m_CamBody;
    Real                m_xRotation;
    Real                m_yRotation;
    Real                m_xRotationTotal;
    Real                m_yRotationTotal;

    Vector3             m_Size;
    Real                m_Mass;
    Real                m_Gravity;
    Real                m_Height;
    Real                m_JumpPower;
    Real                m_FPSSpeed;
    Real                m_xStep;
    Real                m_yStep;
    Real                m_yLimitA;
    Real                m_yLimitB;

    Real                m_Velocity;
    Real                m_SideVel;
};

#endif