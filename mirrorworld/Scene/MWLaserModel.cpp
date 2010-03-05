//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWLaserModel.h"

int MirrorWorld::LaserModel::m_nPlanes = 6;
Ogre::Real MirrorWorld::LaserModel::m_Radius = 1.0f;

namespace MirrorWorld {
void LaserBeam::init(Ogre::SceneManager* sceneMgr, Ogre::MaterialPtr material, int i)
{
    m_pMaterial = material;
    m_pModel = sceneMgr->createManualObject("LaserBeam"+Ogre::StringConverter::toString(i));
    m_pModel->setDynamic(true);
    material->getTechnique(0)->getPass(0)->setSelfIllumination(0, 0, 1);

    m_pSceneNode = sceneMgr->getRootSceneNode()->createChildSceneNode("LaserBeamNode"+Ogre::StringConverter::toString(i));
    m_pSceneNode->attachObject(m_pModel);
}

void LaserBeam::destroy(Ogre::SceneManager* sceneMgr)
{
    if (m_pModel)
        m_pSceneNode->detachObject(m_pModel);
    sceneMgr->destroyManualObject(m_pModel);
}

void LaserBeam::update(Ogre::Vector3 sp, Ogre::Vector3 ep)
{
    Ogre::Vector3 dir = ep - sp;
    Ogre::Real length = dir.normalise();

    Ogre::Vector3 radVec(0, LaserModel::m_Radius/2, 0);
    Ogre::Quaternion rot;
    rot.FromAngleAxis(Ogre::Degree(180.0f/LaserModel::m_nPlanes), Ogre::Vector3::UNIT_Z);
    if (!m_bCreated)
    {
        m_pModel->begin(m_pMaterial->getName(), Ogre::RenderOperation::OT_TRIANGLE_LIST);
        m_bCreated = true;
    }
    else
        m_pModel->beginUpdate(0);
    for (int i = 0;i < LaserModel::m_nPlanes; i++)
    {
        m_pModel->position(-radVec.x, -radVec.y, 1.0);
        m_pModel->textureCoord(0.0, 0.0);
        m_pModel->position(radVec.x, radVec.y, 1.0);
        m_pModel->textureCoord(1.0, 0.0);
        m_pModel->position(-radVec.x, -radVec.y, 0.0);
        m_pModel->textureCoord(0.0, 1.0);
        m_pModel->position(radVec.x, radVec.y, 0.0);
        m_pModel->textureCoord(1.0, 1.0);
        m_pModel->triangle(4*i, 4*i+3, 4*i+1);
        m_pModel->triangle(4*i, 4*i+2, 4*i+3);
        m_pModel->triangle(4*i+1, 4*i+3, 4*i+2);
        m_pModel->triangle(4*i, 4*i+1, 4*i+2);
        radVec = rot*radVec;
    }
    m_pModel->end();

    Ogre::Quaternion rotDir = Ogre::Vector3::UNIT_Z.getRotationTo(dir);
    m_pSceneNode->resetToInitialState();
    m_pSceneNode->scale(1.0, 1.0, length);
    m_pSceneNode->rotate(rotDir);
    m_pSceneNode->translate(sp);
}

void LaserBeam::active()
{
    m_pSceneNode->setVisible(true);
}

void LaserBeam::deactive()
{
    m_pSceneNode->setVisible(false);
}

LaserModel::LaserModel(Ogre::SceneManager *sceneMgr, Ogre::String materialName):m_nUseLaser(0)
{
   m_pMaterial = Ogre::MaterialManager::getSingleton().getByName(materialName);
   Ogre::LogManager::getSingleton().logMessage("initializing lasermodel");
   for (int i = 0;i < MAX_LASERBEAM; i++)
       m_LaserBeamList[i].init(sceneMgr, m_pMaterial, i);
}

void LaserModel::active()
{
    for (int i = 0;i < m_nUseLaser; i++)
        m_LaserBeamList[i].active();
}

void LaserModel::deactive()
{
    for (int i = 0;i < MAX_LASERBEAM; i++)
        m_LaserBeamList[i].deactive();
}

void LaserModel::update(std::vector<Ogre::Vector3> &contactPointList)
{
    m_nUseLaser = (contactPointList.size() - 1) < MAX_LASERBEAM ? contactPointList.size() - 1 : MAX_LASERBEAM;
    for (int i = 0;i < m_nUseLaser; i++)
        m_LaserBeamList[i].update(contactPointList[i], contactPointList[i+1]);
}

}