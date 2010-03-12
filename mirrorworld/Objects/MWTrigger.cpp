//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////
#include "MWTrigger.h"
#include "../MWGameFramework.h"

namespace MirrorWorld
{
void Trigger::initTrigger(const Vector3 &pos, const Vector3 &nor, const Vector3 &up)
{
    activated = false;

    Ogre::Plane plane;
    plane.normal = Vector3::UNIT_Z;
    plane.d = 0;

    Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createPlane(nameID()+"Mesh",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 
        (Ogre::Real)100, (Ogre::Real)100, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);

    Ogre::Quaternion q = Vector3::UNIT_Z.getRotationTo(nor);
    Ogre::Vector3 newUp = q * Vector3::UNIT_Y;
    m_pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(nameID()+"node");
    m_pNode->rotate(newUp.getRotationTo(up));
    m_pNode->rotate(q);
    m_pNode->translate(pos);

    m_pEntity = m_pSceneMgr->createEntity(nameID()+"Ent", nameID()+"Mesh");
    m_pEntity->setCastShadows(false);
    m_pEntity->setMaterialName("Trigger/Frowny");

    m_pNode->attachObject(m_pEntity);

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
    if (obj == NULL)
        return;
    m_pObject = obj;
/*    Ogre::Animation* animation = m_pSceneMgr->createAnimation(nameID()+"Animation", timeLength);
    animation->setInterpolationMode(Ogre::Animation::IM_LINEAR);

    Ogre::NodeAnimationTrack* track = animation->createNodeTrack(0, obj->getSceneNode());
    track->createNodeKeyFrame(0)->setTranslate(Ogre::Vector3(0, 0, 0));
    track->createNodeKeyFrame(timeLength)->setTranslate(translate);

    m_pState = m_pSceneMgr->createAnimationState(nameID()+"Animation");
    m_pState->setLoop(false);*/
    m_Translate = translate;
    m_TimeLength = timeLength;
}

void Trigger::trigger()
{
//    GameFramework::getSingleton().m_pLog->stream() << "3 " << nameID();
  //  if (m_pState->getEnabled())
    //    return;
//    GameFramework::getSingleton().m_pLog->stream() << "3 ";
  //  m_pState->setEnabled(true);

//    GameFramework::getSingleton().m_pLog->stream() << "3 ";
    activated = true;
    m_pEntity->setMaterialName("Trigger/Smiley");
    m_Acctime = 0;
}

void Trigger::update(double timeElasped) {
    Ogre::LogManager::getSingleton().stream()<<activated<<m_pObject;
    if (activated)
    {
        m_Acctime += timeElasped;
        if (m_Acctime > m_TimeLength)
        {
            activated = false;
            return;
        }
        Ogre::Vector3 pos;
        Ogre::Quaternion ori;
        m_pObject->getBody()->getPositionOrientation(pos, ori);
        m_pObject->getBody()->setPositionOrientation(pos+m_Translate*timeElasped/m_TimeLength, ori);
    }
}

}