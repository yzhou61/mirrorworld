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
    while (realUnitStack.size() != 0) {
        delete realUnitStack.top();
        realUnitStack.pop();
    }
    for (int i = 0; i < TEXTURE_LEVELS; ++i)
        while (texturePools[i].size() != 0) {
            delete texturePools[i].top();
            texturePools[i].pop();
        }
    delete m_pSurroundModel;
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

    m_CamMoving = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();

    maxResource = 0;
    for (int i = 0; i < TEXTURE_LEVELS; ++i) {
        RenderUnit *unit = getNewRenderUnit(i);
        texturePools[i].push(unit);
    }

    m_pSurroundModel = new SurroundModel(m_pSceneMgr);
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
    m_pEntity->setCastShadows(false);
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
    m_pSurroundModel->getNode()->setScale(1, 1, 1);
    m_pEntity->setVisible(true);
    m_pSurroundModel->active(m_Position, m_Normal, m_Up);
}

void Mirror::suspendResource() {

    m_pEntity->setVisible(false);
    m_pSurroundModel->deactive();

    m_Node->detachAllObjects();
    m_Node->resetToInitialState();

    MeshManager::getSingleton().remove(m_Mesh->getHandle());
    if (m_pEntity != NULL)
        m_pSceneMgr->destroyEntity(m_pEntity);

    delete m_pPhyBody;
    activated = false;
}

void Mirror::suspend() {
    if (!activated)
        return;

    unfolding = -1;
}

void Mirror::update() {

    RenderUnit *unit = renderUnits.top();
    renderUnits.pop();
	
    unit->eyeCamera->enableCustomNearClipPlane(m_Plane);
	unit->texture->update();
    unit->eyeCamera->disableCustomNearClipPlane();
	m_pEntity->setMaterial(unit->material);

//	GameFramework::getSingletonPtr()->m_pLog->stream() << "updated " << unit->texture->getName();
}

bool Mirror::setEye(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Vector3 up, Ogre::Real fLeft,
                    Ogre::Real fRight, Ogre::Real fTop, Ogre::Real fBottom) {
/*
    GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(fLeft) + " "
        + Ogre::StringConverter::toString(fRight) + " "
        + Ogre::StringConverter::toString(fTop) + " "
        + Ogre::StringConverter::toString(fBottom), 0);
*/
//  GameFramework::getSingleton().setDebugInfo(StringConverter::toString(position.distance(m_Position)), 4);

    Ogre::Real dis = position.distance(m_Position);

    if (dis > MAXIMUM_MIRROR_DISTANCE) {
        realUnitStack.push(NULL);
        return false;
    }

    int level;
    if (dis < 200.0)
        level = 5;
    else {
        level = 5 - (int)(log(dis / 200.0) / log(2.0));
        if (level < 0)
            level = 0;
    }

    GameFramework::getSingleton().setDebugInfo(StringConverter::toString(level), 0);
    GameFramework::getSingleton().setDebugInfo(StringConverter::toString(dis), 1);

    RenderUnit *unit = getNewRenderUnit(level);

    m_CamMoving->detachAllObjects();
    m_CamMoving->resetToInitialState();
    unit->eyeCamera->detachFromParent();
    m_CamMoving->attachObject(unit->eyeCamera);

//    GameFramework::getSingleton().m_pLog->stream() << "set eye" << unit->texture->getName();

    Real ofLeft, ofRight, ofTop, ofBottom;
    ptr_RefCamera->getFrustumExtents(ofLeft, ofRight, ofTop, ofBottom);
    unit->eyeCamera->setFrustumExtents(ofLeft, ofRight, ofTop, ofBottom);

    direction.normalise();
    up.normalise();
//    GameFramework::getSingleton().m_pLog->stream() << "eye dir " << unit->eyeCamera->getRealDirection();
    Quaternion q = unit->eyeCamera->getRealDirection().getRotationTo(direction);
    Vector3 newUp = q * unit->eyeCamera->getRealUp();
/*
    unit->eyeCamera->rotate(newUp.getRotationTo(up));
    unit->eyeCamera->rotate(q);
    unit->eyeCamera->setPosition(position);
*/
    m_CamMoving->rotate(newUp.getRotationTo(up));
    m_CamMoving->rotate(q);
    m_CamMoving->translate(position - unit->eyeCamera->getRealPosition());

    unit->eyePosition = position - 2 * m_Normal.dotProduct(position - m_Position) / m_Normal.length() * m_Normal;
    unit->eyeDirection = direction - 2 * m_Normal.dotProduct(direction) / m_Normal.length() * m_Normal;

    unit->eyeDirection.normalise();
    unit->eyeUp = up - 2 * m_Normal.dotProduct(up) / m_Normal.length() * m_Normal;
    unit->eyeUp.normalise();
	
	Real omLeft, omRight, omTop, omBottom;
	omLeft = 1.0f;
	omRight = -1.0f;
	omTop = -1.0f;
	omBottom = 1.0f;
	
	for (int i = 0; i < 4; ++i) {
		Vector3 cornerPoint = unit->eyeCamera->getProjectionMatrix() * unit->eyeCamera->getViewMatrix() * m_Corners[i];

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
		realUnitStack.push(NULL);
        recyclePool.push(unit);
		return false;
	}

	if ((omBottom == -1.0f) && (omTop == 1.0f) && (omLeft == -1.0f) && (omRight == 1.0f)
		&& (direction.dotProduct(m_Normal.dotProduct(position - m_Position) * m_Normal) >= 0)) {
			realUnitStack.push(NULL);
            recyclePool.push(unit);
			return false;
	}

	Real mWidth = (omRight - omLeft) / 2;
	Real mHeight = (omTop - omBottom) / 2;

    GameFramework::getSingleton().setDebugInfo(StringConverter::toString(mWidth * 800) + " " + StringConverter::toString(mHeight * 600), 3);
/*
    GameFramework::getSingleton().m_pLog->stream() << mWidth << " " << mHeight;

	GameFramework::getSingletonPtr()->setDebugInfo(Ogre::StringConverter::toString(omLeft) + " "
		+ Ogre::StringConverter::toString(omRight) + " "
		+ Ogre::StringConverter::toString(omTop) + " "
		+ Ogre::StringConverter::toString(omBottom), 4);
*/

	omLeft *= -ofLeft;
	omRight *= ofRight;
	omTop *= ofTop;
	omBottom *= -ofBottom;

    unit->eyeCamera->setFrustumExtents(omLeft, omRight, omTop, omBottom);

	renderUnits.push(unit);
	realUnitStack.push(unit);
	return true;
}

Ogre::Vector3 Mirror::getPosition() {
	RenderUnit *unit = renderUnits.top();
	return unit->eyePosition;
}

Ogre::Vector3 Mirror::getDirection() {
    RenderUnit *unit = renderUnits.top();
    return unit->eyeDirection;
}

Ogre::Vector3 Mirror::getUp() {
    RenderUnit *unit = renderUnits.top();
    return unit->eyeUp;
}

void Mirror::getEyeFrustum(Ogre::Real &left, Ogre::Real &right, Ogre::Real &top, Ogre::Real &bottom) {
    RenderUnit *unit = renderUnits.top();
    unit->eyeCamera->getFrustumExtents(left, right, top, bottom);
}

RenderUnit *Mirror::getNewRenderUnit(int textureLevel) {

    if (texturePools[textureLevel].size() == 0) {
        std::stringstream ss;
        ss << m_Identity << "-" << maxResource++;
        String name = ss.str();

        RenderUnit *unit = new RenderUnit;

        unit->level = textureLevel;

        unit->eyeCamera = m_pSceneMgr->createCamera("ReflectCam" + name);
        unit->eyeCamera->setAspectRatio(ptr_RefCamera->getAspectRatio());
        unit->eyeCamera->setNearClipDistance(ptr_RefCamera->getNearClipDistance());
        unit->eyeCamera->setFarClipDistance(ptr_RefCamera->getFarClipDistance());
        unit->eyeCamera->setFOVy(ptr_RefCamera->getFOVy());
        unit->eyeCamera->enableReflection(m_Plane);
        unit->eyeCamera->enableCustomNearClipPlane(m_Plane);

        unsigned int textureSize = LEVEL_0_TEXTURE_SIZE << textureLevel;

        Ogre::TexturePtr texturePtr = Ogre::TextureManager::getSingleton().createManual("RttTex" + name,
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, textureSize, textureSize, 0, PF_R8G8B8, TU_RENDERTARGET);

        unit->texture = texturePtr->getBuffer()->getRenderTarget();
        unit->texture->setAutoUpdated(false);

        unit->material = MaterialManager::getSingleton().create("RttMat" + name,
            ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        unit->material->getTechnique(0)->getPass(0)->createTextureUnitState("RttTex" + name);
        unit->material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
        unit->material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setProjectiveTexturing(true, unit->eyeCamera);

        Viewport *v = unit->texture->addViewport(unit->eyeCamera);
        v->setClearEveryFrame(true);
        v->setBackgroundColour(ColourValue::White);

        return unit;

    } else {
		RenderUnit *unit = texturePools[textureLevel].top();
        texturePools[textureLevel].pop();

        return unit;
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

Ogre::AxisAlignedBox Mirror::getBound() {
    return m_pEntity->getWorldBoundingBox();
}

void Mirror::reflectReal() {

    RenderUnit *unit = realUnitStack.top();
    realUnitStack.pop();

	if (unit != NULL) {
		m_pEntity->setMaterial(unit->material);
        m_pEntity->setVisible(true);
        recyclePool.push(unit);
//		GameFramework::getSingletonPtr()->m_pLog->stream() << "real" << unit->texture->getName();
	} else {
//		GameFramework::getSingletonPtr()->m_pLog->stream() << "real wall";
        m_pEntity->setVisible(false);
//		m_pEntity->setMaterialName("Wall-white-length");
	}
}

void Mirror::preUpdate() {
    while (!renderUnits.empty())
        renderUnits.pop();
    while (!realUnitStack.empty())
        realUnitStack.pop();
    while (!recyclePool.empty()) {
        RenderUnit *unit = recyclePool.top();
        if (unit != NULL)
            texturePools[unit->level].push(unit);
        recyclePool.pop();
    }
}

void Mirror::postUpdate(double timeElapsed)
{
    if (unfolding > 0) {
        unfolding += timeElapsed;
        if (unfolding < 320) {
            m_Node->setScale(1, (Ogre::Real)unfolding / 300, 1);
//            m_pSurroundModel->getNode()->setScale(1, (Ogre::Real)unfolding / 300, 1);
        } else {
            m_Node->setScale(1, 1, 1);
//            m_pSurroundModel->getNode()->setScale(1, 1, 1);
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
        m_pSurroundModel->getNode()->setScale(scale, scale, 1);
        m_Node->rotate(m_Normal, Degree(20), Ogre::Node::TS_WORLD);
        m_pSurroundModel->getNode()->rotate(m_Normal, Degree(20), Ogre::Node::TS_WORLD);

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
