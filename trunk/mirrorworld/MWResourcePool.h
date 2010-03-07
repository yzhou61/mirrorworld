//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#ifndef _MW_RESOURCE_POOL_H_
#define _MW_RESOURCE_POOL_H_
#include <list>
#include <set>

namespace MirrorWorld  {
class ResourceNode
{
public:
    virtual ~ResourceNode() {}
    virtual bool update(double timeElasped) = 0;
    virtual void deactive() = 0;
    int idx;
};

class ResourcePool
{
public:
    ResourcePool(int maxNum);
    ~ResourcePool();
    void update(double timeElasped);
    ResourceNode* getOneNode();
    ResourceNode**   getNodeList() { return m_ResourceNodes; }
private:
    int                 m_nMaxNodes;
    ResourceNode**      m_ResourceNodes;
    std::list<int>      m_ActiveList;
    std::set<int>       m_FreeList;
};

}
#endif