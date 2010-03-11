#include "MWMirror.h"
#include "../MWGameFramework.h"
#include <cmath>
#include <string>
#include <sstream>

using namespace Ogre;

namespace MirrorWorld {
Mirror::~Mirror() 
{
	delete m_Plane;
}

void Mirror::init(SceneManager *mgr, Camera *refCam, OgreNewt::World* world) {
	m_pSceneMgr = mgr;
    m_pWorld = world;
	ptr_RefCamera = refCam;

    m_Position = Vector3::ZERO;
    m_Normal = Vector3::UNIT_Z;
    m_Up = Vector3::UNIT_Y;

    m_Node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_Plane = new MovablePlane("");

    m_pSurroundModel = new SurroundModel(m_pSceneMgr);
    maxResource = 0;
    activated = false;
}

void Mirror::activate(Ogre::Vector3 normal, Ogre::Vector3 position, Ogre::Vector3 up) {
    if (activated)
        return;

    Ogre::String indexStr = Ogre::StringConverter::toString(m_Identity);

    m_Plane->normal = Vector3::UNIT_Z;
    m_Plane->d = 0;
    m_Node->attachObject(m_Plane);

    m_Mesh = MeshManager::getSingleton().createPlane("mirror" + indexStr,
        ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *m_Plane, 
        (Ogre::Real)MIRROR_WIDTH, (Ogre::Real)MIRROR_HEIGHT, 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);

    Quaternion q = Vector3::UNIT_Z.getRotationTo(normal);
    Vector3 newUp = q * Vector3::UNIT_Y;
    m_Node->rotate(newUp.getRotationTo(up));
    m_Node->rotate(q);
    m_Node->translate(position);

    m_pEntity = m_pSceneMgr->createEntity("mEnt" + indexStr, "mirror" + indexStr);
    Ogre::Vector3 size((Ogre::Real)MIRROR_WIDTH, (Ogre::Real)MIRROR_HEIGHT, 0.1);
    OgreNewt::ConvexCollisionPtr colPtr(
        new OgreNewt::CollisionPrimitives::Box(m_pWorld, size, 0));
    m_pPhyBody = new OgreNewt::Body(m_pWorld, colPtr);
#ifdef OGRENEWT_NO_COLLISION_SHAREDPTR
    delete colPtr;
#endif
    m_pPhyBody->setPositionOrientation(m_Node->getPosition(), m_Node->getOrientation());
    m_pPhyBody->attachNode(m_Node);
    m_pPhyBody->setMaterialGroupID(m_pWorld->getDefaultMaterialID());
    attachUserData();

    m_Node->attachObject(m_pEntity);

    m_Position = position;
    m_Normal = normal.normalisedCopy();
    m_Up = up.normalisedCopy();

    Vector3 hor = normal.crossProduct(m_Up);
    hor.normalise();

    m_Corners[0] = m_Position + hor * MIRROR_WIDTH / 2 + m_Up * MIRROR_HEIGHT / 2;
    m_Corners[1] = m_Position + hor * MIRROR_WIDTH / 2 - m_Up * MIRROR_HEIGHT / 2;
    m_Corners[2] = m_Position - hor * MIRROR_WIDTH / 2 - m_Up * MIRROR_HEIGHT / 2;
    m_Corners[3] = m_Position - hor * MIRROR_WIDTH / 2 + m_Up * MIRROR_HEIGHT / 2;

    preUpdate();

    unfolding = 1;
    activated = true;
    m_Node->setScale(1, 0, 1);
    m_pEntity->setVisible(true);
}

void Mirror::suspendResource() {

    m_pEntity->setVisible(false);

    m_Node->detachAllObjects();
    m_Node->resetToInitialState();

    MeshManager::getSingleton().remove(m_Mesh->getHandle());
    if (m_pEntity != NULL)
        m_pSceneMgr->destroyEntity(m_pEntity);

    m_pSurroundModel->deactive();
    delete m_pPhyBody;
    activated = false;
}

void Mirror::suspend() {
    if (!activated)
        return;

    unfolding = -1;
}

void Mirror::update() {

	size_t index = resourceStack.top();
	resourceStack.pop();

	std::stringstream ss;
	ss << m_Identity << "-" << index;
	String name = ss.str();
	
    eyes.at(index)->enableCustomNearClipPlane(m_Plane);
	textures.at(index)->update();
    eyes.at(index)->disableCustomNearClipPlane();
	m_pEntity->setMaterialName("RttMat" + name);

//    GameFramework::getSingleton().setDebugInfo(StringConverter::toString(m_Normal), 0);

//	GameFramework::getSingletonPtr()->m_pLog->stream() << "updated " << name;
}

bool Mirror::setEye(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Vector3 up, Ogre::Real fLeft,
	Ogre::Real fRight, Ogre::Real fTop, Ogre::Real fBottom) {
	size_t index = getNewResourceIndex();
/*
    GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(fLeft) + " "
        + Ogre::StringConverter::toString(fRight) + " "
        + Ogre::StringConverter::toString(fTop) + " "
        + Ogre::StringConverter::toString(fBottom), 0);
*/
	std::stringstream ss;
	ss << m_Identity << "-" << index;
	String name = ss.str();

	Real ofLeft, ofRight, ofTop, ofBottom;
	ptr_RefCamera->getFrustumExtents(ofLeft, ofRight, ofTop, ofBottom);
	eyes.at(index)->setFrustumExtents(ofLeft, ofRight, ofTop, ofBottom);
	
	direction.normalise();
	up.normalise();
	Quaternion q = eyes.at(index)->getRealDirection().getRotationTo(direction);
	Vector3 newUp = q * eyes.at(index)->getRealUp();

//	GameFramework::getSingletonPtr()->setDebugInfo("original dir: " + Ogre::StringConverter::toString(eyes.at(index)->getDirection()), 4);

	eyes.at(index)->rotate(q);
	eyes.at(index)->rotate(newUp.getRotationTo(up));
	eyes.at(index)->setPosition(position);

	eyePositions.at(index) = position - 2 * m_Normal.dotProduct(position - m_Position) / m_Normal.length() * m_Normal;
	eyeDirections.at(index) = direction - 2 * m_Normal.dotProduct(direction) / m_Normal.length() * m_Normal;

	eyeDirections.at(index).normalise();
	eyeUps.at(index) = up - 2 * m_Normal.dotProduct(up) / m_Normal.length() * m_Normal;
	eyeUps.at(index).normalise();

//	GameFramework::getSingletonPtr()->m_pLog->stream() << name << eyes.at(index)->getDirection();
//	GameFramework::getSingletonPtr()->m_pLog->stream() << name << direction;
//	GameFramework::getSingletonPtr()->m_pLog->stream() << name << eyes.at(index)->getUp();
//	GameFramework::getSingletonPtr()->m_pLog->stream() << name << up;
//	GameFramework::getSingletonPtr()->m_pLog->stream() << name << eyes.at(index)->getDirection().dotProduct(eyes.at(index)->getUp());
//	GameFramework::getSingletonPtr()->m_pLog->stream() << name << eyes.at(index)->getDirection().dotProduct(up);

//	GameFramework::getSingletonPtr()->setDebugInfo("1 up: " + Ogre::StringConverter::toString(up), 0);
//	GameFramework::getSingletonPtr()->setDebugInfo("1 dir: " + Ogre::StringConverter::toString(direction), 1);

//	GameFramework::getSingletonPtr()->setDebugInfo("2 up: " + Ogre::StringConverter::toString(eyes.at(index)->getUp()), 2);
//	GameFramework::getSingletonPtr()->setDebugInfo("2 dir: " + Ogre::StringConverter::toString(eyes.at(index)->getDirection()), 3);

//	GameFramework::getSingletonPtr()->setDebugInfo("real up: " + Ogre::StringConverter::toString(eyes.at(index)->getRealUp()), 2);
	
	Real omLeft, omRight, omTop, omBottom;
	omLeft = 1.0f;
	omRight = -1.0f;
	omTop = -1.0f;
	omBottom = 1.0f;
	
	for (int i = 0; i < 4; ++i) {
		Vector3 cornerPoint = eyes.at(index)->getProjectionMatrix() * eyes.at(index)->getViewMatrix() * m_Corners[i];

		if ((cornerPoint.z > 1)) {
			if (cornerPoint.x < 0)
				omRight = 1.0f;
			if (cornerPoint.x > 0)
				omLeft = -1.0f;
			if (cornerPoint.y > 0)
				omBottom = -1.0f;
			if (cornerPoint.y < 0)
				omTop = 1.0f;
			continue;
		} else {
			if (cornerPoint.x < omLeft)
				omLeft = cornerPoint.x;
			if (cornerPoint.x > omRight)
				omRight = cornerPoint.x;
			if (cornerPoint.y > omTop)
				omTop = cornerPoint.y;
			if (cornerPoint.y < omBottom)
				omBottom = cornerPoint.y;
		}
	}

	if (omLeft < -1.0f)
		omLeft = -1.0f;
	if (omRight > 1.0f)
		omRight = 1.0f;
	if (omTop > 1.0f)
		omTop = 1.0f;
	if (omBottom < -1.0f)
		omBottom = -1.0f;

//	GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(omLeft) + " " + Ogre::StringConverter::toString(omRight)
//		+ " " + Ogre::StringConverter::toString(omTop) + " " + Ogre::StringConverter::toString(omBottom), 4);

	fLeft /= -ofLeft;
	fRight /= ofRight;
	fTop /= ofTop;
	fBottom /= -ofBottom;

	if (fLeft > omLeft)
		omLeft = fLeft;
	if (fRight < omRight)
		omRight = fRight;
	if (fTop < omTop)
		omTop = fTop;
	if (fBottom > omBottom)
		omBottom = fBottom;

	if ((omBottom >= omTop) || (omLeft >= omRight)) {
		realReflectionStack.push(-2);
		return false;
	}

	if ((omBottom == -1.0f) && (omTop == 1.0f) && (omLeft == -1.0f) && (omRight == 1.0f)
		&& (direction.dotProduct(m_Normal.dotProduct(position - m_Position) * m_Normal) >= 0)) {
			realReflectionStack.push(-2);
			return false;
	}

	Real mWidth = (omRight - omLeft) / 2;
	Real mHeight = (omTop - omBottom) / 2;
	
/*	GameFramework::getSingletonPtr()->m_pLog->stream() << name << " " << mWidth << " " << mHeight;

	GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(omLeft) + " "
		+ Ogre::StringConverter::toString(omRight) + " "
		+ Ogre::StringConverter::toString(omTop) + " "
		+ Ogre::StringConverter::toString(omBottom), 4);
*/
	if ((mWidth < MINIMUN_TEXTURE_DIMENSION / 100.0) && (mHeight < MINIMUN_TEXTURE_DIMENSION / 100.0)) {
		realReflectionStack.push(-1);
		return false;
	}

	omLeft *= -ofLeft;
	omRight *= ofRight;
	omTop *= ofTop;
	omBottom *= -ofBottom;
	
	eyes.at(index)->setFrustumExtents(omLeft, omRight, omTop, omBottom);

	resourceStack.push(index);
	realReflectionStack.push(index);
	return true;
}

Ogre::Vector3 Mirror::getPosition() {
	size_t index = resourceStack.top();
	return eyePositions.at(index);
}

Ogre::Vector3 Mirror::getDirection() {
	size_t index = resourceStack.top();
	return eyeDirections.at(index);
}

Ogre::Vector3 Mirror::getUp() {
	size_t index = resourceStack.top();
	return eyeUps.at(index);
}

void Mirror::getEyeFrustum(Ogre::Real &left, Ogre::Real &right, Ogre::Real &top, Ogre::Real &bottom) {
	size_t index = resourceStack.top();
    eyes.at(index)->getFrustumExtents(left, right, top, bottom);
}

size_t Mirror::getNewResourceIndex() {
	if (curResource < maxResource) {
		++curResource;
		return curResource - 1;
	} else {
		std::stringstream ss;
		ss << m_Identity << "-" << maxResource;
		String name = ss.str();

		Ogre::Camera *newEye = m_pSceneMgr->createCamera("ReflectCam" + name);
		newEye->setAspectRatio(ptr_RefCamera->getAspectRatio());
		newEye->setNearClipDistance(ptr_RefCamera->getNearClipDistance());
		newEye->setFarClipDistance(ptr_RefCamera->getFarClipDistance());
		newEye->setFOVy(ptr_RefCamera->getFOVy());
        newEye->enableReflection(m_Plane);
        newEye->enableCustomNearClipPlane(m_Plane);
		eyes.push_back(newEye);

		Ogre::Vector3 temp;
		eyePositions.push_back(temp);
		eyeDirections.push_back(temp);
		eyeUps.push_back(temp);

        Ogre::TexturePtr texturePtr = Ogre::TextureManager::getSingleton().createManual("RttTex" + name,
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, TEXTURE_SIZE, TEXTURE_SIZE, 0, PF_R8G8B8, TU_RENDERTARGET);

        Ogre::RenderTexture *curTex = texturePtr->getBuffer()->getRenderTarget();
        curTex->setAutoUpdated(false);
        textures.push_back(curTex);

		Viewport *v = curTex->addViewport(newEye);
		v->setClearEveryFrame(true);
        v->setBackgroundColour(ColourValue::White);

        Ogre::MaterialPtr newMat = MaterialManager::getSingleton().create("RttMat" + name,
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		newMat->getTechnique(0)->getPass(0)->createTextureUnitState("RttTex" + name);
        newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
        newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setProjectiveTexturing(true, eyes.at(curResource));
        materials.push_back(newMat);
		
		++curResource;
		++maxResource;
		return curResource - 1;
	}
}

Ogre::Vector3 Mirror::getCorners(int index) {
	if ((index < 0) || (index > 3))
		return Vector3::ZERO;
	return m_Corners[index];
}

Ogre::Vector3 Mirror::getNormal() {
	return m_Normal;
}

Ogre::Vector3 Mirror::getCenterPosition() {
	return m_Position;
}

void Mirror::reflectReal() {
	int index = realReflectionStack.top();
	realReflectionStack.pop();
    last = index;
	if (index >= 0) {
		std::stringstream ss;
		ss << m_Identity << "-" << index;
		String name = ss.str();
		m_pEntity->setMaterialName("RttMat" + name);
//		GameFramework::getSingletonPtr()->m_pLog->stream() << "real" << name;
	} else if (index == -1) {
//		GameFramework::getSingletonPtr()->m_pLog->stream() << "real wall";
		m_pEntity->setMaterialName("Examples/Rockwall");
	}
}

void Mirror::preUpdate() {
    while (!resourceStack.empty())
        resourceStack.pop();
    while (!realReflectionStack.empty())
        realReflectionStack.pop();
    curResource = 0;
    //    m_pEntity->setVisible(true);
}

void Mirror::postUpdate(double timeElapsed)
{
    if (unfolding > 0) {
        m_pSurroundModel->active(m_Position, m_Normal, m_Up);
        unfolding += timeElapsed;
        if (unfolding > 850)
            return;
        if (unfolding < 320) {
            m_Node->setScale(1, (Ogre::Real)unfolding / 300, 1);
        } else if (unfolding < 650) {
            m_Node->setScale(1, 1, 1);
            if (!m_pEntity->getVisible())
                m_pEntity->setVisible(true);
        } else if (unfolding < 700) {
            if (m_pEntity->getVisible())
                m_pEntity->setVisible(false);
        } else if (unfolding < 750) {
            if (!m_pEntity->getVisible())
                m_pEntity->setVisible(true);
        } else if (unfolding < 800) {
            if (m_pEntity->getVisible())
                m_pEntity->setVisible(false);
        } else if (!m_pEntity->getVisible()) {
            m_pEntity->setVisible(true);
        }
    }
    if (unfolding < 0) {
        unfolding -= timeElapsed;
        if (unfolding < -400) {
            if (activated)
                suspendResource();
            return;
        }
        Ogre::Real scale = static_cast<Ogre::Real>(unfolding + 400) / 400.0f;
        m_Node->setScale(scale, scale, 1);
        m_Node->rotate(m_Normal, Degree(20), Ogre::Node::TS_WORLD);

//        std::stringstream ss;
//        ss << m_Identity << "-" << last;
//        String name = ss.str();
//        GameFramework::getSingleton().m_pLog->stream() << last;
//        Ogre::TextureUnitState *t = materials.at(last)->getTechnique(0)->getPass(0)->getTextureUnitState(0);
//        ColourValue c(1, 1, 1, 0);
//        materials.at(last)->getTechnique(0)->setDepthWriteEnabled(false);
//        t->setColourOperationEx(LBX_BLEND_MANUAL, LBS_TEXTURE, LBS_CURRENT, c, c, (unfolding + 400) / 400.0f);

    }
}

/////////////////////////////////////////////////////////////////////

Mirror *MirrorMgr::create(unsigned int id) const {
	return new Mirror(id);
}

}
