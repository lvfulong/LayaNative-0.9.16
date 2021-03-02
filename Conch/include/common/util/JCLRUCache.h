/**
@file			JCColor.h
@brief			
@author			James
@version		1.0
@date			2013_12_19
*/

#ifndef __LRUCache_H__
#define __LRUCache_H__

#include <stdio.h>
#include <vector>
#include <utility>
#include <list>
#include <cassert>
#ifndef ANDROID
#include <unordered_map>
#else
#include <map>
#endif
namespace laya
{
    // 最近最少缓存算法（key,value）
    template<typename KEY, typename VALUE, typename HASH>
    class LRUCache
    {
    private:
        typedef std::list<std::pair<KEY, VALUE> > LIST_TYPE;
        LIST_TYPE m_list;
//#ifndef ANDROID
        std::unordered_map<KEY, typename LIST_TYPE::iterator, HASH> m_map;
//#else
//        std::map<KEY, typename LIST_TYPE::iterator, HASH> m_map;
//#endif
        unsigned int m_capacity;
    public:
        LRUCache(unsigned int capacity) :m_capacity(capacity)
        {
        }
        ~LRUCache()
        {
            clear();
        }
        void clear()
        {
            m_list.clear();
            m_map.clear();
        }
        void set(const KEY& key, const VALUE& value)
        {
            // 1、查询是否在缓存中
            auto iteramap = m_map.find(key);
            if (iteramap != m_map.end()) {
                // 2、在缓存中，需要在链表中擦除。
                m_list.erase(iteramap->second);
                // 3、把数据放到链表头
                m_list.push_front(std::pair<KEY, VALUE>(key, value));
                m_map[key] = m_list.begin();
            }
            else {
                if (m_map.size() >= m_capacity) {
                    // 4、缓存已经满了
                    // 4.1 hash处要删除
                    m_map.erase(m_list.back().first);
                    // 4.2 链表也要删除尾巴部分
                    m_list.pop_back();
                }
                // 5、双向链表首结点插入
                m_list.push_front(std::pair<KEY, VALUE>(key, value));
                // 6、在hash中增加
                m_map[key] = m_list.begin();
            }
        }
        bool isCached(const KEY& key)
        {
            auto iteramap = m_map.find(key);
            if (iteramap == m_map.end())
            {
                return false;
            }
            return true;
        }
        // 获取:根据key，获取缓存的value
        VALUE get(const KEY& key)
        {
            // 1、先从hash中查找
            auto iteramap = m_map.find(key);
            assert(iteramap != m_map.end());
            // 2、如果在缓存中，需要把数据放到链表头部。
            
          
            m_list.push_front(std::pair<KEY, VALUE>(key, iteramap->second->second));
             m_list.erase(iteramap->second);

            // 3、hash原来存储的失效，需要重新设置
            m_map[key] = m_list.begin();

            // 4、返回value值
            //return iteramap->second->second;
            return m_list.begin()->second;
        }
    };
//------------------------------------------------------------------------------
}

#endif //__LRUCache_H__

//-----------------------------END FILE--------------------------------
