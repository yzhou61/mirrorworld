#ifndef _MW_MIRROR_H_
#define _MW_MIRROR_H_

#include "MWObject.h"
#include "../Scene/MWSurroundModel.h"

#include <vector>
#include <stack>

namespace MirrorWorld {

    struct RenderUnit {
        int level;

        Ogre::Camera *eyeCamera;
        Ogre::Vector3 eyePosition;
        Ogre::Vector3 eyeDirection;
        Ogre::Vector3 eyeUp;
        Ogre::MaterialPtr material;
        Ogre::RenderTexture *texture;
    };

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
        Ogre::AxisAlignedBox getBound();

		void reflectReal();

        RenderUnit *getNewRenderUnit(int textureLevel);

        void activate(Ogre::Vector3 normal, Ogre::Vector3 position, Ogre::Vector3 up);
        void suspend();
        bool isActivated() { return activated; }

	private:
        void suspendResource();

        static const int MIRROR_WIDTH = 80;
        static const int MIRROR_HEIGHT = 120;
        static const unsigned int LEVEL_0_TEXTURE_SIZE = 16;
        static const int MAXIMUM_MIRROR_DISTANCE = 3200;
        static const int TEXTURE_LEVELS = 6;

		Ogre::MovablePlane *m_Plane;
		Ogre::Camera *ptr_RefCamera;
        Ogre::SceneNode *m_CamMoving;
        Ogre::MeshPtr m_Mesh;
		Ogre::SceneNode* m_Node;
		Ogre::Vector3 m_Normal;
		Ogre::Vector3 m_Position;
		Ogre::Vector3 m_Up;
		Ogre::Vector3 m_Corners[4];

        std::stack<RenderUnit *> renderUnits;
		std::stack<RenderUnit *> realUnitStack;
		size_t maxResource;
        std::stack<RenderUnit *> texturePools[TEXTURE_LEVELS];
        std::stack<RenderUnit *> recyclePool;

        bool activated;
        double unfolding;

        size_t last;
        SurroundModel*      m_pSurroundModel;
	};

	class MirrorMgr : public ObjectMaker {
	public:
		MirrorMgr() {}
		~MirrorMgr() {}
		Mirror *create(unsigned int id) const;
	};
}
#endif