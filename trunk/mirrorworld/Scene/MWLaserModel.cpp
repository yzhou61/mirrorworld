//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWLaserModel.h"

int MirrorWorld::LaserModel::m_nPlanes = 6;
Ogre::Real MirrorWorld::LaserModel::m_Radius = 0.4f;
const float laserRad = 0.01f;

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

void LaserBeam::update(const Ogre::Vector3& sp, const Ogre::Vector3& ep)
{
    Ogre::Vector3 dir = ep - sp;
    Ogre::Real length = dir.normalise();

    Ogre::Vector3 radVec(0, LaserModel::m_Radius/2, 0);
    Ogre::Quaternion rot;
    rot.FromAngleAxis(Ogre::Degree(180.0f/LaserModel::m_nPlanes), Ogre::Vector3::UNIT_Z);
    if (!m_bCreated)
    {
        m_pModel->begin(m_pMaterial->getName(), Ogre::RenderOperation::OT_LINE_LIST);
        m_bCreated = true;
    }
    else
        m_pModel->beginUpdate(0);
/*    for (int i = 0;i < LaserModel::m_nPlanes; i++)
    {
        m_pModel->position(-radVec.x, -radVec.y, 1.0);
        m_pModel->textureCoord(0.0, 0.2);
        m_pModel->position(radVec.x, radVec.y, 1.0);
        m_pModel->textureCoord(1.0, 0.2);
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
    m_pModel->position(-LaserModel::m_Radius/2, -LaserModel::m_Radius/2, 0);
    m_pModel->textureCoord(0.0, 0.0);
    m_pModel->position(LaserModel::m_Radius/2, -LaserModel::m_Radius/2, 0);
    m_pModel->textureCoord(0.2, 0.0);
    m_pModel->position(LaserModel::m_Radius/2, LaserModel::m_Radius/2, 0);
    m_pModel->textureCoord(0.2, 0.2);
    m_pModel->position(-LaserModel::m_Radius/2, LaserModel::m_Radius/2, 0);
    m_pModel->textureCoord(0.2, 0.0);
    m_pModel->triangle(24, 25, 26);
    m_pModel->triangle(26, 27, 24);
    m_pModel->position(-LaserModel::m_Radius/2, -LaserModel::m_Radius/2, 1);
    m_pModel->textureCoord(0.0, 0.0);
    m_pModel->position(LaserModel::m_Radius/2, -LaserModel::m_Radius/2, 1);
    m_pModel->textureCoord(0.2, 0.0);
    m_pModel->position(LaserModel::m_Radius/2, LaserModel::m_Radius/2, 1);
    m_pModel->textureCoord(0.2, 0.2);
    m_pModel->position(-LaserModel::m_Radius/2, LaserModel::m_Radius/2, 1);
    m_pModel->textureCoord(0.2, 0.0);
    m_pModel->triangle(28, 29, 30);
    m_pModel->triangle(30, 31, 28);*/
    for (int j = 0;j < 5; j++)
    {
        Ogre::Real rad = Ogre::Math::UnitRandom()*laserRad;
        for (int i = 0;i < 24; i++)
        {
            Ogre::Real val = Ogre::Math::UnitRandom()*360.0f;
            Ogre::Real x = Ogre::Math::Cos(val)*rad;
            Ogre::Real y = Ogre::Math::Sin(val)*rad;
            m_pModel->position(x, y, 0);
            m_pModel->position(x*10.0f, y*10.0f, 1);
        }
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

LaserModel::LaserModel(Ogre::SceneManager *sceneMgr):m_nUseLaser(0)
{
   m_pMaterial = Ogre::MaterialManager::getSingleton().getByName("Laserbeam");
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

void LaserModel::update(const std::vector<Ogre::Vector3> &contactPointList)
{
    deactive();
    m_nUseLaser = (contactPointList.size() - 1) < MAX_LASERBEAM ? contactPointList.size() - 1 : MAX_LASERBEAM;
    for (int i = 0;i < m_nUseLaser; i++)
        m_LaserBeamList[i].update(contactPointList[i], contactPointList[i+1]);
    active();
}

}