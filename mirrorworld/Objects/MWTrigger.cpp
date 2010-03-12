//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////
#include "MWTrigger.h"

namespace MirrorWorld
{
void Trigger::initTrigger(const Vector3 &pos, const Vector3 &nor, const Vector3 &up)
{
    Ogre::Plane plane;
    plane.normal = Vector3::UNIT_Z;
    plane.d = 0;

    Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createPlane(nameID()+"Mesh",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
        (Ogre::Real)100, (Ogre::Real)100, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);

    Ogre::Quaternion q = Vector3::UNIT_Z.getRotationTo(nor);
    Ogre::Vector3 newUp = q * Vector3::UNIT_Y;
    m_pNode->rotate(newUp.getRotationTo(up));
    m_pNode->rotate(q);
    m_pNode->translate(pos);

    m_pEntity = m_pSceneMgr->createEntity(nameID()+"Ent", nameID()+"Mesh");
    m_pEntity->setCastShadows(false);
    Ogre::Vector3 size((Ogre::Real)100, (Ogre::Real)100, 0.1);
    OgreNewt::ConvexCollisionPtr colPtr(
        new OgreNewt::CollisionPrimitives::Box(m_pWorld, size, 0));
    m_pPhyBody = new OgreNewt::Body(m_pWorld, colPtr);
#ifdef OGRENEWT_NO_COLLISION_SHAREDPTR
    delete colPtr;
#endif
    m_pPhyBody->setPositionOrientation(m_pNode->getPosition(), m_pNode->getOrientation());
    m_pPhyBody->attachNode(m_pNode);
    m_pPhyBody->setMaterialGroupID(m_pWorld->getDefaultMaterialID());
    attachUserData();
}

void Trigger::setTarget(Object* obj, const Ogre::Vector3& translate, Ogre::Real timeLength)
{
    m_pObject = obj;
    Ogre::Animation* animation = m_pSceneMgr->createAnimation(nameID()+"Animation", 5);
    animation->setInterpolationMode(Ogre::Animation::IM_LINEAR);

    Ogre::NodeAnimationTrack* track = animation->createNodeTrack(0, obj->getSceneNode());
    track->createNodeKeyFrame(0)->setTranslate(Ogre::Vector3(0, 0, 0));
    track->createNodeKeyFrame(timeLength)->setTranslate(translate);

    m_pState = m_pSceneMgr->createAnimationState(nameID()+"AnimationState");
}

void Trigger::trigger()
{
    m_pState->setEnabled(true);
}
}