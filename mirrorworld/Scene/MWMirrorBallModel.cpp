//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWMirrorBallModel.h"

int MirrorWorld::MirrorBallModel::m_nUniqueID = 0;

namespace MirrorWorld {
MirrorBallModel::MirrorBallModel(Ogre::SceneManager* sceneMgr)
{
    Ogre::String id = Ogre::StringConverter::toString(m_nUniqueID++);
    m_pSceneMgr = sceneMgr;
    m_pBallNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("BallNode"+id);
    m_pTrailNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("TrailNode"+id);

    m_pRibbonTrail = m_pSceneMgr->createRibbonTrail("RibbonTrail"+id);
    m_pRibbonTrail->setMaterialName("Examples/LightRibbonTrail");
    m_pRibbonTrail->setTrailLength(400);
    m_pRibbonTrail->setInitialColour(0, 0.0, 0.1, 0.8);
    m_pRibbonTrail->setColourChange(0, 0.5, 0.5, 0.5, 0.5);
    m_pRibbonTrail->setInitialWidth(0, 5);
    m_pRibbonTrail->setMaxChainElements(80);
    m_pRibbonTrail->setNumberOfChains(2);
    m_pRibbonTrail->addNode(m_pBallNode);
    
    m_pLight = m_pSceneMgr->createLight("MirrorLight"+id);
    m_pLight->setDiffuseColour(m_pRibbonTrail->getInitialColour(0));
    m_pLight->setAttenuation(1000, 0.8, 0.1, 0.01);

    m_pBillboard = m_pSceneMgr->createBillboardSet("RibbonTrailBillboard"+id, 1);
    m_pBillboard->createBillboard(Ogre::Vector3::ZERO, m_pRibbonTrail->getInitialColour(0));
    m_pBillboard->setMaterialName("Examples/Flare");
}

void MirrorBallModel::active(const Ogre::Vector3& pos)
{
    m_pRibbonTrail->removeNode(m_pBallNode);
    m_pBallNode->setPosition(pos);
    m_pRibbonTrail->addNode(m_pBallNode);
    m_pBallNode->attachObject(m_pLight);
    m_pBallNode->attachObject(m_pBillboard);
    m_pTrailNode->attachObject(m_pRibbonTrail);
}

void MirrorBallModel::deactive()
{
    m_pBallNode->detachAllObjects();
    m_pTrailNode->detachAllObjects();
}

void MirrorBallModel::update(const Ogre::Vector3& pos)
{
    m_pBallNode->setPosition(pos);
}

void MirrorBallModel::destroy()
{

}
}