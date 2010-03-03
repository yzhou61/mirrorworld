#include "MWMirror.h"
#include "../MWGameFramework.h"
#include <cmath>
#include <string>
#include <sstream>

using namespace Ogre;

namespace MirrorWorld {
void Mirror::init(SceneManager *mgr, Vector3 normal, Real distance, size_t width, size_t height,
				  Camera *refCam) {
	m_SceneMgr = mgr;
	ptr_RefCamera = refCam;

	plane = new MovablePlane(normal, distance);
	MeshManager::getSingleton().createPlane("mirror" + m_Identity,
	  ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *plane, 
      static_cast<Real>(width), static_cast<Real>(height), 20, 20, true, 1, 5, 5);
	m_pEntity = m_SceneMgr->createEntity("mirrorEnt" + m_Identity, "mirror" + m_Identity);
	m_Node = m_SceneMgr->getRootSceneNode()->createChildSceneNode();
	m_Node->attachObject(m_pEntity);
	m_Node->attachObject(plane);
	m_Node->translate(0, static_cast<Real>(height/2.0), 0);

	maxResource = 0;
	curResource = 0;
	contRender = true;
}

void Mirror::update() {
	size_t index = resourceStack.top();
	resourceStack.pop();

	std::stringstream ss;
	ss << m_Identity << "-" << index;
	String name = ss.str();
	
	Ogre::RenderTexture *renderTexture = textures.at(index)->getBuffer()->getRenderTarget();
	renderTexture->setAutoUpdated(false);

	m_pEntity->setVisible(false);
	eyes.at(index)->enableReflection(plane);
	eyes.at(index)->enableCustomNearClipPlane(plane);
	renderTexture->update();
	m_pEntity->setMaterialName("RttMat" + name);
	eyes.at(index)->disableReflection();
	eyes.at(index)->disableCustomNearClipPlane();
	m_pEntity->setVisible(true);

	GameFramework::getSingletonPtr()->m_pLog->stream() << "updated " << name;
}

bool Mirror::setEye(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Real fLeft,
	Ogre::Real fRight, Ogre::Real fTop, Ogre::Real fBottom) {
	size_t index = getNewResourceIndex();

	std::stringstream ss;
	ss << m_Identity << "-" << index;
	String name = ss.str();

	materials.at(index)->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
	materials.at(index)->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setProjectiveTexturing(true, eyes.at(index));

	eyes.at(index)->setPosition(position);
	eyes.at(index)->setDirection(direction);

	Real ofLeft, ofRight, ofTop, ofBottom;
	ptr_RefCamera->getFrustumExtents(ofLeft, ofRight, ofTop, ofBottom);
	eyes.at(index)->setFrustumExtents(ofLeft, ofRight, ofTop, ofBottom);

	eyePositions.at(index) = eyes.at(index)->getPosition() -
		plane->normal * 2 * plane->getDistance(eyes.at(index)->getPosition());
	eyeDirections.at(index) = 2 * plane->projectVector(eyes.at(index)->getDirection()) - eyes.at(index)->getDirection();

	AxisAlignedBox aab = m_pEntity->getWorldBoundingBox();
	
	Real omLeft, omRight, omTop, omBottom;
	omLeft = 100;
	omRight = -100;
	omTop = -100;
	omBottom = 100;
	for (int i = 0; i < 8; ++i) {
		Vector3 cornerPoint = eyes.at(index)->getProjectionMatrix()
			* eyes.at(index)->getViewMatrix() * aab.getAllCorners()[i];

		if ((cornerPoint.z > 1) || (cornerPoint.z < 0)) {
			if (cornerPoint.x < 0)
				omRight = 1;
			if (cornerPoint.x > 0)
				omLeft = -1;
			if (cornerPoint.y > 0)
				omBottom = -1;
			if (cornerPoint.y < 0)
				omTop = 1;
			continue;
		}

		if (cornerPoint.x < -1)
			omLeft = -1;
		else if (cornerPoint.x < omLeft)
			omLeft = cornerPoint.x;
		if (cornerPoint.x > 1)
			omRight = 1;
		else if (cornerPoint.x > omRight)
			omRight = cornerPoint.x;
		if (cornerPoint.y > 1)
			omTop = 1;
		else if (cornerPoint.y > omTop)
			omTop = cornerPoint.y;
		if (cornerPoint.x < -1)
			omBottom = -1;
		else if (cornerPoint.y < omBottom)
			omBottom = cornerPoint.y;
	}

	if (omLeft < -1)
		omLeft = -1;
	if (omRight > 1)
		omRight = 1;
	if (omTop > 1)
		omTop = 1;
	if (omBottom < -1)
		omBottom = -1;

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

	Real mWidth = abs(omRight - omLeft);
	Real mHeight = abs(omTop - omBottom);
	GameFramework::getSingletonPtr()->m_pLog->stream() << name << " " << mWidth << " " << mHeight;

	if ((mWidth < 0.3) && (mHeight < 0.3)) {
		contRender = false;
		GameFramework::getSingletonPtr()->m_pLog->stream() << "wall " << name;
		realReflectionStack.push(-1);
		return false;
	}

	omLeft *= -ofLeft;
	omRight *= ofRight;
	omTop *= ofTop;
	omBottom *= -ofBottom;
	
	eyes.at(index)->setFrustumExtents(omLeft, omRight, omTop, omBottom);
	contRender = true;
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

Ogre::Camera *Mirror::getEye() {
	size_t index = resourceStack.top();
	return eyes.at(index);
}

void Mirror::resetResource() {
	while (!resourceStack.empty())
		resourceStack.pop();
	while (!realReflectionStack.empty())
		realReflectionStack.pop();
	curResource = 0;
	contRender = true;
}

size_t Mirror::getNewResourceIndex() {
	if (curResource < maxResource) {
		++curResource;
		return curResource - 1;
	} else {
		std::stringstream ss;
		ss << m_Identity << "-" << maxResource;
		String name = ss.str();

		Ogre::Camera *newEye = m_SceneMgr->createCamera("ReflectCam" + name);
		newEye->setAspectRatio(ptr_RefCamera->getAspectRatio());
		newEye->setNearClipDistance(ptr_RefCamera->getNearClipDistance());
		newEye->setFarClipDistance(ptr_RefCamera->getFarClipDistance());
		newEye->setFOVy(ptr_RefCamera->getFOVy());
		eyes.push_back(newEye);

		Ogre::Vector3 temp;
		eyePositions.push_back(temp);
		eyeDirections.push_back(temp);

		textures.push_back(Ogre::TextureManager::getSingleton().createManual("RttTex" + name,
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, 512, 512, 0, PF_R8G8B8, TU_RENDERTARGET));

		Viewport *v = textures.at(curResource)->getBuffer()->getRenderTarget()->addViewport(eyes.at(curResource));
		v->setClearEveryFrame(true);
		v->setBackgroundColour(ColourValue::Black);

		materials.push_back(MaterialManager::getSingleton().create("RttMat" + name,
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
		materials.at(curResource)->getTechnique(0)->getPass(0)->createTextureUnitState("RttTex" + name);
		
		++curResource;
		++maxResource;
		return curResource - 1;
	}
}

Ogre::AxisAlignedBox Mirror::getBound() {
	return m_pEntity->getWorldBoundingBox();
}

Ogre::MovablePlane *Mirror::getPlane() {
	return plane;
}

void Mirror::reflectReal() {
	size_t index = realReflectionStack.top();
	realReflectionStack.pop();
	if (index != -1) {
		std::stringstream ss;
		ss << m_Identity << "-" << index;
		String name = ss.str();
		m_pEntity->setMaterialName("RttMat" + name);
		GameFramework::getSingletonPtr()->m_pLog->stream() << "real " << name;
	} else {
		m_pEntity->setMaterialName("Examples/Rockwall");
		GameFramework::getSingletonPtr()->m_pLog->stream() << "real mirror";
	}
}

/////////////////////////////////////////////////////////////////////

void MirrorMgr::init(Ogre::SceneManager *mgr, Ogre::RenderWindow *window, Ogre::Camera *refCam) {
	m_SceneMgr = mgr;
	m_Window = window;
	m_RefCamera = refCam;
	mirrorCount = 0;
	m_CheckViewCamera = m_SceneMgr->createCamera("CheckViewCamera");
	m_CheckViewCamera->setAspectRatio(m_RefCamera->getAspectRatio());
	m_CheckViewCamera->setNearClipDistance(m_RefCamera->getNearClipDistance());
	m_CheckViewCamera->setFarClipDistance(m_RefCamera->getFarClipDistance());
	m_CheckViewCamera->setFOVy(m_RefCamera->getFOVy());
}

Mirror *MirrorMgr::create(unsigned int id) const {
	Mirror *newOne = new Mirror(id);
//	m_Mirrors[mirrorCount++] = newOne;
	return newOne;
}

void MirrorMgr::initMirror(size_t id, Ogre::Vector3 normal, Ogre::Real distance,
						  size_t width, size_t height) {
	Mirror *curMirror;
	for (size_t i = 0; i < mirrorCount; ++i) {
		if (m_Mirrors[i]->getID() == id) {
			curMirror = m_Mirrors[i];
			break;
		}
	}
	curMirror->init(m_SceneMgr, normal, distance, width, height, m_RefCamera);
}

void MirrorMgr::resetMirrors() {
	for (size_t i = 0; i < mirrorCount; ++i) {
		m_Mirrors[i]->resetResource();
	}
}

void MirrorMgr::updateMirrors(Ogre::Vector3 position, Ogre::Vector3 direction,
	Ogre::Real fLeft, Ogre::Real fRight, Ogre::Real fTop, Ogre::Real fBottom, size_t mirrorID) {

	for (size_t i = 0; i < mirrorCount; ++i) {
		m_CheckViewCamera->setPosition(position);
		m_CheckViewCamera->setDirection(direction);
		m_CheckViewCamera->setFrustumExtents(fLeft, fRight, fTop, fBottom);

		Mirror *m = m_Mirrors[i];
		if (m->getID() == mirrorID) {
			continue;
		}
		if (m->getPlane()->getDistance(position) <= 0)
			continue;
		if (!m_CheckViewCamera->isVisible(m->getBound()))
			continue;
		bool cont = m->setEye(position, direction, fLeft, fRight, fTop, fBottom);
		if (cont) {
			Ogre::Real fLeft, fRight, fTop, fBottom;
			m->getEye()->getFrustumExtents(fLeft, fRight, fTop, fBottom);
			updateMirrors(m->getPosition(), m->getDirection(), -fRight, -fLeft, fTop, fBottom, m->getID());
			m->update();
		}
	}
	setRealReflections(position, direction, fLeft, fRight, fTop, fBottom);
}

void MirrorMgr::setRealReflections(Ogre::Vector3 position, Ogre::Vector3 direction,
	Ogre::Real fLeft, Ogre::Real fRight, Ogre::Real fTop, Ogre::Real fBottom) {

	m_CheckViewCamera->setPosition(position);
	m_CheckViewCamera->setDirection(direction);
	m_CheckViewCamera->setFrustumExtents(fLeft, fRight, fTop, fBottom);

	for (size_t i = 0; i < mirrorCount; ++i) {
		Mirror *m = m_Mirrors[i];
		if (m->getPlane()->getDistance(position) <= 0)
			continue;
		if (!m_CheckViewCamera->isVisible(m->getBound()))
			continue;
		m->reflectReal();
	}
}

}