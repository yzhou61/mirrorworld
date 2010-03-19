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
    m_pObject = NULL;
    if (obj == NULL)
        return;
    m_pObject = obj;
    m_Translate = translate;
    m_TimeLength = timeLength;
}

bool Trigger::trigger()
{
    if (activated)
        return false;

    m_pEntity->setMaterialName("Trigger/Smiley");
    m_Acctime = 0;
    activated = true;
    return true;
}

void Trigger::update(double timeElasped) {
    if (m_pObject == NULL || !activated || m_Acctime > m_TimeLength)
        return;
    m_Acctime += timeElasped;

    Ogre::Vector3 pos;
    Ogre::Quaternion ori;
    m_pObject->getBody()->getPositionOrientation(pos, ori);
    m_pObject->getBody()->setPositionOrientation(pos+m_Translate*static_cast<Ogre::Real>(timeElasped/m_TimeLength), ori);
}

}