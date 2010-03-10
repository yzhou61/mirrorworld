//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWStaticObject.h"

namespace MirrorWorld {

void StaticObject::setEntity(Ogre::SceneManager* sceneMgr, OgreNewt::World* world, 
        Ogre::SceneNode* node, Ogre::Entity* entity)
{
    m_pSceneMgr = sceneMgr; 
    m_pWorld = world;
    m_pEntity = entity;

    OgreNewt::CollisionPtr colPtr(new OgreNewt::CollisionPrimitives::TreeCollision(world, entity, true, 0));
    m_pPhyBody = new OgreNewt::Body(world, colPtr);
#ifdef OGRENEWT_NO_COLLISION_SHAREDPTR
    delete colPtr;
#endif
    m_pPhyBody->setPositionOrientation(node->getPosition(), Ogre::Quaternion::IDENTITY);
    m_pPhyBody->attachNode(node);

    if (m_pPhyMatID)
        m_pPhyBody->setMaterialGroupID(m_pPhyMatID);
    else
        m_pPhyBody->setMaterialGroupID(m_pWorld->getDefaultMaterialID());

    attachUserData();
}

}