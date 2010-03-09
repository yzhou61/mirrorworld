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
		void init(Ogre::SceneManager *mgr, Ogre::Camera *refCam, OgreNewt::World* world);
        
        void update();
		bool setEye(Ogre::Vector3 position, Ogre::Vector3 direction, Ogre::Vector3 up, Ogre::Real fLeft, Ogre::Real fRight, 
			Ogre::Real fTop, Ogre::Real fBottom);

        void preUpdate();
        void postUpdate(double timeElapsed);

		Ogre::Vector3 getPosition();
		Ogre::Vector3 getDirection();
		Ogre::Vector3 getUp();
        void getEyeFrustum(Ogre::Real &left, Ogre::Real &right, Ogre::Real &top, Ogre::Real &bottom);

		Ogre::Vector3 getCorners(int index);
		Ogre::Vector3 getNormal();
		Ogre::Vector3 getCenterPosition();

		void reflectReal();

        size_t getNewResourceIndex();

        void activate(Ogre::Vector3 normal, Ogre::Vector3 position, Ogre::Vector3 up);
        void suspend();
        bool isActivated() { return activated; }
        bool shouldUpdate() { return activated && m_pEntity->getVisible(); }

	private:
        void suspendResource();

        static const int MIRROR_WIDTH = 150;
        static const int MIRROR_HEIGHT = 200;
        static const int TEXTURE_SIZE = 512;

		Ogre::MovablePlane *m_Plane;
		Ogre::Camera *ptr_RefCamera;
        Ogre::MeshPtr m_Mesh;
		Ogre::SceneNode* m_Node;
		Ogre::Vector3 m_Normal;
		Ogre::Vector3 m_Position;
		Ogre::Vector3 m_Up;
		Ogre::Vector3 m_Corners[4];
		std::vector<Ogre::Camera *> eyes;
		std::vector<Ogre::Vector3> eyePositions;
		std::vector<Ogre::Vector3> eyeDirections;
		std::vector<Ogre::Vector3> eyeUps;
        std::vector<Ogre::RenderTexture *> textures;
		std::vector<Ogre::MaterialPtr> materials;
		std::stack<size_t> resourceStack;
		std::stack<int> realReflectionStack;
		size_t maxResource;
		size_t curResource;
		Ogre::SceneManager *m_SceneMgr;

        bool activated;
        double unfolding;

        size_t last;
	};

	class MirrorMgr : public ObjectMaker {
	public:
		MirrorMgr() {}
		~MirrorMgr() {}
		Mirror *create(unsigned int id) const;
	};
}
#endif