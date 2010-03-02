#ifndef _MW_MIRROR_H_
#define _MW_MIRROR_H_

#include "MWObject.h"

#include <vector>
#include <stack>

namespace MirrorWorld {

class Mirror : public Object
{
public:
    Mirror(unsigned int id):Object(id, false, true){}
	~Mirror() { delete plane; }
	Ogre::String name() const { return "Mirror"; }

	virtual void init(Ogre::SceneManager *mgr, Ogre::Vector3 normal, Ogre::Real distance,
		size_t width, size_t height, Ogre::Camera *refCam);
	virtual void update();

	bool setEye(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Real fLeft, Ogre::Real fRight, 
		Ogre::Real fTop, Ogre::Real fBottom);

	Ogre::Vector3 getPosition();
	Ogre::Vector3 getDirection();
	Ogre::Camera *getEye();

	void resetResource();
	size_t getNewResourceIndex();

	Ogre::AxisAlignedBox getBound();
	Ogre::MovablePlane *getPlane();

	void reflectReal();

    void setEntity(Ogre::Entity* entity) { m_Entity = entity; }
    void setSceneNode(Ogre::SceneNode *node) { m_Node = node; }
    void setPosition(Ogre::Vector3 p) { m_Node->setPosition(p); }

private:
	Ogre::MovablePlane *plane;
	Ogre::Camera *ptr_RefCamera;
	std::vector<Ogre::Camera *> eyes;
	std::vector<Ogre::Vector3> eyePositions;
	std::vector<Ogre::Vector3> eyeDirections;
	std::vector<Ogre::TexturePtr> textures;
	std::vector<Ogre::MaterialPtr> materials;
	std::stack<size_t> resourceStack;
	std::stack<size_t> realReflectionStack;
	size_t maxResource;
	size_t curResource;
	Ogre::SceneManager *m_SceneMgr;
	size_t realReflectionNum;
	bool contRender;
    Ogre::SceneNode* m_Node;
};

class MirrorMgr : public ObjectMaker {
public:
	MirrorMgr(){}
	~MirrorMgr() {}
	void init(Ogre::SceneManager *mgr, Ogre::RenderWindow *window, Ogre::Camera *refCam);
	Mirror *create(unsigned int id) const;
	void initMirror(size_t id, Ogre::Vector3 normal, Ogre::Real distance, size_t width, size_t height);
	void updateMirrors(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Real fLeft, Ogre::Real fRight, 
		Ogre::Real fTop, Ogre::Real fBottom, size_t mirrorID);

	void resetMirrors();
	void setRealReflections(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Real fLeft, Ogre::Real fRight, 
		Ogre::Real fTop, Ogre::Real fBottom);

private:
	Ogre::SceneManager *m_SceneMgr;
	Ogre::RenderWindow *m_Window;
	Ogre::Camera *m_RefCamera;
	Ogre::Camera *m_CheckViewCamera;
	Mirror *m_Mirrors[10];
	size_t mirrorCount;
};

}
#endif