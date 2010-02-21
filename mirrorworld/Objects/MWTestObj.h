#ifndef _MW_TEST_OBJ_H_
#define _MW_TEST_OBJ_H_

#include "MWObject.h"

namespace MirrorWorld {
class TestObj : public Object
{
public:
    TestObj():Object() {}
    ~TestObj(){}
    Ogre::String name() { return "TestObj"; }
};

class TestObjMaker : public ObjectMaker
{
public:
    TestObjMaker() : ObjectMaker() {}
    ~TestObjMaker(){}
    TestObj* create() { return new TestObj(); }
};

}
#endif