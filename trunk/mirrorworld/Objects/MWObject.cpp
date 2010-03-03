//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////
#include "MWObject.h"

namespace MirrorWorld {
void Object::attachUserData()
{
    if (m_pEntity) 
        m_pEntity->setUserAny(Ogre::Any(this)); 

    if (m_pPhyBody)
        m_pPhyBody->setUserData(Ogre::Any(this));
}

}