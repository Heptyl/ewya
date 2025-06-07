/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#ifndef __HI_MW_ARRAY_H__
#define __HI_MW_ARRAY_H__

#include "hi_mw_log.h"
#include <list>

/* 检查list 是否为空 */
#define CHECK_LIST_EMPTY(listobject)          \
    do {                                      \
        if (listobject.empty()) {             \
            LOGW("%s is empty", #listobject); \
        }                                     \
    } while (0)

/* *************************************************************************** */
/*
 * Base array template class.  The implementation is system-dependent.
 * You must instantiate it by specify the type 'T' before using.
 *
 */
template<typename T>
class Array {
    typedef typename std::list<T>::iterator Iterator;

public:
    unsigned int insert(unsigned int pos, const T &dat);
    void addHead(const T &dat);
    void addTail(const T &dat);
    unsigned int remove(unsigned int pos);
    void removeHead();
    void removeTail();
    void clear();
    const T &head() const;
    const T &tail() const;
    const T &getAt(unsigned int pos) const;
    unsigned int length() const;
    bool empty() const;
    const Array<T> &operator=(const Array<T> &arry);
    const Array<T> &operator=(const Array<T> &&arry);
    const T &operator[](unsigned int pos) const;

    Array();
    Array(const Array<T> &arry);
    Array(const Array<T> &&arry);
    virtual ~Array();

private:
    std::list<T> m_list;
};

template<typename T>
inline Array<T>::Array()
{
}

template<typename T>
inline Array<T>::~Array()
{
}

template<typename T>
inline Array<T>::Array(const Array<T> &arry) :
    m_list(arry.m_list)
{
}

template<typename T>
inline Array<T>::Array(const Array<T> &&arry) :
    m_list(std::move(arry.m_list))
{
}

template<typename T>
unsigned int Array<T>::insert(unsigned int pos, const T &dat)
{
    if (0 == m_list.size()) {
        pos = 0;
        LOGW("hi_mw_array.h: arry max size is 0, insert list first, index:%d", pos);
    } else if (pos >= m_list.size()) {
        LOGW("hi_mw_array.h: index:%d, arry max size:%d", pos, m_list.size());
        pos = m_list.size() - 1;
    }
    Iterator iter = m_list.begin();
    while (pos) {
        iter++;
        pos--;
    }
    iter = m_list.insert(iter, dat);
    while (iter != m_list.begin()) {
        pos++;
        iter--;
    }
    return pos;
}

template<typename T>
unsigned int Array<T>::remove(unsigned int pos)
{
    if (0 == m_list.size()) {
        LOGW("hi_mw_array.h: remove pos:%d, m_list size is 0, remove fail", pos);
        return 0;
    }
    if (pos >= m_list.size()) {
        LOGW("hi_mw_array.h: index:%d, arry max size:%d", pos, m_list.size());
        pos = m_list.size() - 1;
    }
    Iterator iter = m_list.begin();
    while (pos) {
        iter++;
        pos--;
    }
    iter = m_list.erase(iter);
    while (iter != m_list.begin()) {
        pos++;
        iter--;
    }
    return pos;
}

template<typename T>
const T &Array<T>::operator[](unsigned int pos) const
{
    Iterator iter = const_cast<Array<T> *>(this)->m_list.begin();
    if (0 == m_list.size()) {
        LOGW("hi_mw_array.h: remove pos:%d, m_list size is 0, operator fail, return null value", pos);
        return *iter;
    }
    if (pos >= m_list.size()) {
        LOGW("hi_mw_array.h: index:%d, arry max size:%d", pos, m_list.size());
        pos = m_list.size() - 1;
    }
    while (pos) {
        iter++;
        pos--;
    }
    return *iter;
}

template<typename T>
void Array<T>::addHead(const T &dat)
{
    return m_list.push_front(dat);
}

template<typename T>
void Array<T>::addTail(const T &dat)
{
    return m_list.push_back(dat);
}

template<typename T>
void Array<T>::removeHead()
{
    return m_list.pop_front();
}

template<typename T>
void Array<T>::removeTail()
{
    return m_list.pop_back();
}

template<typename T>
void Array<T>::clear()
{
    return m_list.clear();
}

template<typename T>
bool Array<T>::empty() const
{
    return m_list.empty() ? true : false;
}

template<typename T>
const T &Array<T>::head() const
{
    CHECK_LIST_EMPTY(m_list);
    return m_list.front();
}

template<typename T>
const T &Array<T>::tail() const
{
    return m_list.back();
}

template<typename T>
const T &Array<T>::getAt(unsigned int pos) const
{
    return (*this)[pos];
}

template<typename T>
unsigned int Array<T>::length() const
{
    return m_list.size();
}

template<typename T>
const Array<T> &Array<T>::operator=(const Array<T> &arry)
{
    this->m_list = arry.m_list;
    return *this;
}

template<typename T>
const Array<T> &Array<T>::operator=(const Array<T> &&arry)
{
    this->m_list = std::move(arry.m_list);
    return *this;
}

#endif /* __HI_MW_ARRAY_H___ */
