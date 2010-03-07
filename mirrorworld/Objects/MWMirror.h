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
		~Mirror();
		Ogre::String name() const { return "Mirror"; }

		virtual void init(Ogre::SceneManager *mgr, Ogre::Vector3 normal, Ogre::Vector3 position, Ogre::Vector3 up,
			size_t width, size_t height, Ogre::Camera *refCam);
		virtual void update();

		bool setEye(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Vector3 up, Ogre::Real fLeft, Ogre::Real fRight, 
			Ogre::Real fTop, Ogre::Real fBottom);

		Ogre::Vector3 getPosition();
		Ogre::Vector3 getDirection();
		Ogre::Vector3 getUp();
		Ogre::Camera *getEye();

		void resetResource();
		size_t getNewResourceIndex();

		Ogre::Vector3 getCorners(int index);
		Ogre::Vector3 getNormal();
		Ogre::Vector3 getCenterPosition();

		void reflectReal();

		void setEntity(Ogre::Entity* entity) { m_pEntity = entity; }
		void setSceneNode(Ogre::SceneNode *node) { m_Node = node; }
		void setPosition(Ogre::Vector3 p) { m_Node->setPosition(p); }

	private:
		Ogre::MovablePlane *plane;
		Ogre::Camera *ptr_RefCamera;
		Ogre::Camera *tempCam;
		Ogre::SceneNode* m_Node;
		Ogre::Vector3 m_Normal;
		Ogre::Vector3 m_Position;
		Ogre::Vector3 m_Up;
		Ogre::Vector3 m_Corners[4];
		std::vector<Ogre::Camera *> eyes;
		std::vector<Ogre::Vector3> eyePositions;
		std::vector<Ogre::Vector3> eyeDirections;
		std::vector<Ogre::Vector3> eyeUps;
		std::vector<Ogre::TexturePtr> textures;
		std::vector<Ogre::MaterialPtr> materials;
		std::stack<size_t> resourceStack;
		std::stack<int> realReflectionStack;
		size_t maxResource;
		size_t curResource;
		Ogre::SceneManager *m_SceneMgr;
	};

	class MirrorMgr : public ObjectMaker {
	public:
		MirrorMgr() {}
		~MirrorMgr() {}
		Mirror *create(unsigned int id) const;
	};

}
#endif