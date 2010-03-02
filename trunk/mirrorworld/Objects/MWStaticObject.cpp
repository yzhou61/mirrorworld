//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWStaticObject.h"

namespace MirrorWorld {

void StaticObject::setEntity(Ogre::SceneManager *sceneMgr, OgreNewt::World *world, Ogre::SceneNode *node, Ogre::Entity *entity)
{
    m_Entity = entity; 
    m_Entity->setUserAny(Ogre::Any(this));

    OgreNewt::CollisionPtr colPtr(new OgreNewt::CollisionPrimitives::TreeCollision(world, entity, true, 0));
    m_pPhyBody = new OgreNewt::Body(world, colPtr);

#ifdef OGRENEWT_NO_COLLISION_SHAREDPTR
    delete camCollision;
#endif

    m_pPhyBody->setPositionOrientation(node->getPosition(), Ogre::Quaternion::IDENTITY);
    m_pPhyBody->attachNode(node);

    if (m_pPhyMatID)
        m_pPhyBody->setMaterialGroupID(m_pPhyMatID);
}

}