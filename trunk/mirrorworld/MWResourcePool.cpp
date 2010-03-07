//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
#include "MWResourcePool.h"

namespace MirrorWorld {
ResourcePool::ResourcePool(int maxNum):m_nMaxNodes(maxNum)
{
    m_ResourceNodes = new ResourceNode*[m_nMaxNodes];
    m_ActiveList.clear();
    for (int i = 0;i < m_nMaxNodes; i++)
        m_FreeList.insert(i);
}

ResourcePool::~ResourcePool()
{
    m_ActiveList.clear();
    m_FreeList.clear();
    delete[] m_ResourceNodes;
}

void ResourcePool::update(double timeElasped)
{
    std::list<int> deadList;
    for (std::list<int>::iterator it = m_ActiveList.begin(); it != m_ActiveList.end(); it++)
    {
        if (!m_ResourceNodes[*it]->update(timeElasped))
            deadList.push_back(*it);
    }
    for (std::list<int>::iterator it = deadList.begin(); it != deadList.end(); it++)
    {
        m_ResourceNodes[*it]->deactive();
        m_ActiveList.remove(*it);
        m_FreeList.insert(*it);
    }
}

ResourceNode* ResourcePool::getOneNode()
{
    int idx = 0;
    if (m_FreeList.empty())
    {
        idx = (*m_ActiveList.begin());
        m_ActiveList.remove(idx);
        m_ResourceNodes[idx]->deactive();
    }
    else
    {
        idx = (*m_FreeList.begin());
        m_FreeList.erase(idx);
    }

    m_ActiveList.push_back(idx);
    return m_ResourceNodes[idx];
}
}