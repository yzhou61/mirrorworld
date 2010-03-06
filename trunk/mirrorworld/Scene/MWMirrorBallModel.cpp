//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWMirrorBallModel.h"

namespace MirrorWorld {
MirrorBallModel::MirrorBallModel(Ogre::SceneManager* sceneMgr, const Ogre::Vector3& initPos)
{
    m_pSceneMgr = sceneMgr;
    m_pSceneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("RibbonTrailNode");
    m_pSceneNode->setPosition(initPos);

    Ogre::NameValuePairList pairList;
    Ogre::RibbonTrail* trail = m_pSceneMgr->createRibbonTrail("1");
    trail->setMaterialName("Examples/LightRibbonTrail");
    trail->setTrailLength(400);
    trail->setInitialColour(0, 0.0, 0.1, 0.8);
    trail->setColourChange(0, 0.5, 0.5, 0.5, 0.5);
    trail->setInitialWidth(0, 5);
    trail->setMaxChainElements(80);
    trail->setNumberOfChains(2);
    m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(trail);
    trail->addNode(m_pSceneNode);
    
    m_pLight = m_pSceneMgr->createLight("l2");
    m_pLight->setDiffuseColour(trail->getInitialColour(0));
    m_pLight->setAttenuation(1000, 0.8, 0.1, 0.01);
    m_pSceneNode->attachObject(m_pLight);

    m_pBillboard = m_pSceneMgr->createBillboardSet("RibbonTrailBillboard", 1);
    m_pBillboard->createBillboard(Ogre::Vector3::ZERO, trail->getInitialColour(0));
    m_pBillboard->setMaterialName("Examples/Flare");
    m_pSceneNode->attachObject(m_pBillboard);
}

void MirrorBallModel::update(const Ogre::Vector3& pos)
{
    m_pSceneNode->setPosition(pos);
}

void MirrorBallModel::destroy()
{

}
}