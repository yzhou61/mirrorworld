#include "MWMirror.h"
#include "../MWGameFramework.h"
#include <cmath>
#include <string>
#include <sstream>

using namespace Ogre;

namespace MirrorWorld {
Mirror::~Mirror() 
{
	delete plane;
}

void Mirror::init(SceneManager *mgr, Vector3 normal, Vector3 position, Vector3 up, size_t width, size_t height,
				  Camera *refCam) {
	m_SceneMgr = mgr;
	ptr_RefCamera = refCam;

	m_Position = position;
	m_Normal = normal.normalisedCopy();
	m_Up = up.normalisedCopy();

//	tempCam = m_SceneMgr->createCamera("tempCam");

	plane = new MovablePlane("plane" + m_Identity);
	plane->normal = Vector3::UNIT_Z;
	plane->d = 0;
	MeshManager::getSingleton().createPlane("mirror" + m_Identity,
	  ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *plane, 
	  static_cast<Real>(width), static_cast<Real>(height), 1, 1, true, 1, 1, 1, Vector3::UNIT_Y);
	
	m_pEntity = m_SceneMgr->createEntity("mEnt" + StringConverter::toString(m_Identity), "mirror" + m_Identity);
	m_Node = m_SceneMgr->getRootSceneNode()->createChildSceneNode();
	m_Node->attachObject(m_pEntity);
	m_Node->attachObject(plane);

	Quaternion q = Vector3::UNIT_Z.getRotationTo(normal);
	Vector3 newUp = q * Vector3::UNIT_Y;

	m_Node->translate(position);
	m_Node->rotate(newUp.getRotationTo(up));
	m_Node->rotate(q);

	Vector3 hor = normal.crossProduct(m_Up);
	hor.normalise();

	m_Corners[0] = m_Position + hor * (Real)width / 2 + m_Up * (Real)height / 2;
	m_Corners[1] = m_Position + hor * (Real)width / 2 - m_Up * (Real)height / 2;
	m_Corners[2] = m_Position - hor * (Real)width / 2 - m_Up * (Real)height / 2;
	m_Corners[3] = m_Position - hor * (Real)width / 2 + m_Up * (Real)height / 2;

	maxResource = 0;
	curResource = 0;
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

//	GameFramework::getSingletonPtr()->m_pLog->stream() << "updated " << name;
}

bool Mirror::setEye(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Vector3 up, Ogre::Real fLeft,
	Ogre::Real fRight, Ogre::Real fTop, Ogre::Real fBottom) {
	size_t index = getNewResourceIndex();

	std::stringstream ss;
	ss << m_Identity << "-" << index;
	String name = ss.str();

	Real ofLeft, ofRight, ofTop, ofBottom;
	ptr_RefCamera->getFrustumExtents(ofLeft, ofRight, ofTop, ofBottom);
	eyes.at(index)->setFrustumExtents(ofLeft, ofRight, ofTop, ofBottom);

	materials.at(index)->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
	materials.at(index)->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setProjectiveTexturing(true, eyes.at(index));

	direction.normalise();
	up.normalise();
//	eyes.at(index)->setDirection(direction);
	Quaternion q = eyes.at(index)->getRealDirection().getRotationTo(direction);
	Vector3 newUp = q * eyes.at(index)->getRealUp();

//	GameFramework::getSingletonPtr()->setDebugInfo("original dir: " + Ogre::StringConverter::toString(eyes.at(index)->getDirection()), 4);

	eyes.at(index)->rotate(q);
	eyes.at(index)->rotate(newUp.getRotationTo(up));
//	eyes.at(index)->setDirection(direction);
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

//		GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(m_Corners[i]), i);

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
	
//	GameFramework::getSingletonPtr()->m_pLog->stream() << name << " " << mWidth << " " << mHeight;
/*
	GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(omLeft) + " "
		+ Ogre::StringConverter::toString(omRight) + " "
		+ Ogre::StringConverter::toString(omTop) + " "
		+ Ogre::StringConverter::toString(omBottom), 4);
*/
	if ((mWidth < 0.1) && (mHeight < 0.1)) {
		realReflectionStack.push(-1);
		return false;
	}

	omLeft *= -ofLeft;
	omRight *= ofRight;
	omTop *= ofTop;
	omBottom *= -ofBottom;
	
	eyes.at(index)->setFrustumExtents(omLeft, omRight, omTop, omBottom);
//	if (index == 0)
//	GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(eyes.at(index)->getRealUp()), 3);
//	if (index == 3)
//	GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(eyes.at(index)->getRealUp()), 1);
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
		eyeUps.push_back(temp);

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
	if (index >= 0) {
		std::stringstream ss;
		ss << m_Identity << "-" << index;
		String name = ss.str();
		m_pEntity->setMaterialName("RttMat" + name);
//		GameFramework::getSingletonPtr()->m_pLog->stream() << "real" << name;
	} else if (index == -1) {
		GameFramework::getSingletonPtr()->m_pLog->stream() << "real wall";
//		m_pEntity->setMaterialName("Examples/Rockwall");
	}
}

/////////////////////////////////////////////////////////////////////

Mirror *MirrorMgr::create(unsigned int id) const {
	return new Mirror(id);
}

}