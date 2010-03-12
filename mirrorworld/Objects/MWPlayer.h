//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_PLAYER_H_
#define _MW_PLAYER_H_
#include "MWObject.h"

namespace MirrorWorld{

using Ogre::Real;
using Ogre::Vector3;
using Ogre::Quaternion;

class Player : public Object
{
public:
    Player(unsigned int id);
    ~Player();
    Ogre::String name() const { return "Player"; }
    void init(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, Ogre::Camera* camera);
    void up();
    void upRelease();
    void down();
    void downRelease();
    void left();
    void leftRelease();
    void right();
    void rightRelease();
    void jump();
    void update(double timeElasped);
    void moveCamera(int xRel, int yRel);
    void test();
    void test2() { testnum ++ ;}
    OgreNewt::MaterialID* getPhyMaterial() { return m_PhyMatID; }
    Ogre::Camera *getCamera() { return m_pCamera; }
private:
    void bodyForceCallback(OgreNewt::Body *body, float timeStep, int threadIndex);
    void walk();
    void stop();
    void updateAnimation(double timeElasped);
    int testnum;
    Ogre::SceneNode*        m_pSceneNode;
    Ogre::SceneNode*        m_pBodyNode;
    OgreNewt::MaterialID*   m_PhyMatID;
    OgreNewt::PlayerController* m_pPlayerControl;

    Ogre::Camera*           m_pCamera;
    Real                    m_xRotation;
    Real                    m_yRotation;
    Real                    m_xRotationTotal;
    Real                    m_yRotationTotal;
    Ogre::Radian            m_Heading;

    Vector3                 m_Size;
    Real                    m_Mass;
    Real                    m_Gravity;
    Real                    m_Height;
    Real                    m_JumpPower;
    Real                    m_xStep;
    Real                    m_yStep;
    Real                    m_yLimitA;
    Real                    m_yLimitB;

    Real                    m_Velocity;
    Real                    m_SideVel;

    Ogre::Skeleton*         m_pSkeleton;
    Ogre::AnimationState*   m_pWalkState;
    bool                    m_bWalking;
    Real                    m_AnimationSpeed;
    float                   m_WalkingTimeLength;
    Ogre::Bone*             m_pNeckBone;
    Ogre::Bone*             m_pHeadBone;
}; // End of Player
} // End of MirrorWorld
#endif