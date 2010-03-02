//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWLogicManager.h"

using Ogre::Vector3;

namespace MirrorWorld
{
void LogicManager::init()
{
    // Will always has one laser
    m_pLaser = static_cast<Laser*>(ObjectFactory::getSingleton().createObj("Laser"));
    m_pLaser->reset();
    m_bLaserOn = false;
    
    // Clear up the mirror ball list
    int n = m_MirrorBallList.size();
    for (int i = 0;i < n; i++)
    {
        delete m_MirrorBallList.front();
        m_MirrorBallList.pop();
    }

    // Setup RayQuery
    m_TestRayQuery = m_pSceneMgr->createRayQuery(m_TestRay);
    m_TestRayQuery->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
    m_TestRayQuery->setSortByDistance(true);
    m_TestRayQuery->setWorldFragmentType(Ogre::SceneQuery::WFT_SINGLE_INTERSECTION);
}

void LogicManager::calcLaserPath()
{
    if (!m_bLaserOn)
        return;

    m_TestRay.setOrigin(Vector3(0, 0, 0));
    m_TestRay.setDirection(Vector3(1, 0, 0));
    for ( ; ; )
    {
        m_TestRayQuery->setRay(m_TestRay);

        Ogre::RaySceneQueryResult& queryResult = m_TestRayQuery->execute();
        if (queryResult.size() > 0)
        {
            Object* hitobj = Ogre::any_cast<Object*>(queryResult.begin()->movable->getUserAny());
            if (hitobj)
            {
                Ogre::LogManager::getSingleton().logMessage(hitobj->nameID());
                if (hitobj->isReflective())
                {

                }
                else
                    break;
            }
        }
    }
    
}

void LogicManager::calcMirrorBallsPath()
{

}

}