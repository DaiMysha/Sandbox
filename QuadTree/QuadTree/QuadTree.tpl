#include <DMUtils/maths.hpp>
#include <DMUtils/maths.hpp>

#include "QuadTree.hpp"

template <size_t CAPACITY, typename T, typename P, typename N>
QuadTree<CAPACITY, T, P, N>::QuadTree(float width, float height, int maximumDepth) : QuadTree(0, 0, width, height, maximumDepth)
{
}

template <size_t CAPACITY, typename T, typename P, typename N>
QuadTree<CAPACITY, T, P, N>::QuadTree(float left, float top, float width, float height, int maximumDepth) :
    _coveredZone(left, top, width, height),
    _children(nullptr),
    _size(0),
    _maximumDepth(maximumDepth)
{
}

template <size_t CAPACITY, typename T, typename P, typename N>
QuadTree<CAPACITY, T, P, N>::~QuadTree()
{
    _deleteChildren();
}

template <size_t CAPACITY, typename T, typename P, typename N>
void QuadTree<CAPACITY, T, P, N>::insert(const P& position, const T& item)
{
    if(N::belongsTo(_coveredZone.left, _coveredZone.top, _coveredZone.width, _coveredZone.height, position)) _insert(position, item);
}

template <size_t CAPACITY, typename T, typename P, typename N>
bool QuadTree<CAPACITY, T, P, N>::remove(const T& item)
{
    auto it = std::find_if(_data.begin(),_data.end(),[&item](const N& n) {
        return n.data == item;
    });

    if(it != _data.end())
    {
        _data.erase(it);
        --_size;
        return true;
    }

    if(_children)
    {
        bool res = false;
        res = _children->nw.remove(item);
        if(!res && _children->ne.remove(item)) res =  true;
        if(!res && _children->sw.remove(item)) res =  true;
        if(!res && _children->se.remove(item)) res =  true;

        if(res)
        {
            --_size;
        }
        return res;
    }
    return false;
}

template <size_t CAPACITY, typename T, typename P, typename N>
size_t QuadTree<CAPACITY, T, P, N>::remove(const sf::Rect<float>& zone)
{
    size_t res = 0;
    if(!_coveredZone.intersects(zone)) return res;

    _data.remove_if([&res,&zone](const N& n){
        if(N::belongsTo(zone.left, zone.top, zone.width, zone.height, n.position))
        {
            ++res;
            return true;
        }
        return false;
    });

    if(_children)
    {
        res += _children->nw.remove(zone);
        res += _children->ne.remove(zone);
        res += _children->sw.remove(zone);
        res += _children->se.remove(zone);
    }

    _size -= res;
    shrinkToFit();
    return res;
}

template <size_t CAPACITY, typename T, typename P, typename N>
void QuadTree<CAPACITY, T, P, N>::clear()
{
    if(_children)
    {
        _deleteChildren();
    }
    _size = 0;
    _data.clear();
}

template <size_t CAPACITY, typename T, typename P, typename N>
size_t QuadTree<CAPACITY, T, P, N>::size() const
{
    return _size;
}

template <size_t CAPACITY, typename T, typename P, typename N>
size_t QuadTree<CAPACITY, T, P, N>::getNodeCount() const
{
    size_t ans = 1;
    if(_children)
    {
        ans += _children->nw.getNodeCount();
        ans += _children->ne.getNodeCount();
        ans += _children->sw.getNodeCount();
        ans += _children->se.getNodeCount();
    }
    return ans;
}

template <size_t CAPACITY, typename T, typename P, typename N>
size_t QuadTree<CAPACITY, T, P, N>::getCapacity() const
{
    return CAPACITY;
}

//void QuadTree<CAPACITY, T, P, N>::setArea(const physics::AABB<float>& area)

template <size_t CAPACITY, typename T, typename P, typename N>
std::list<T> QuadTree<CAPACITY, T, P, N>::query(float x, float y, float width, float height) const
{
    if(!_coveredZone.intersects(sf::Rect<float>(x, y, width, height))) return std::list<T>();
    std::list<T> res;
    _query(x,y,width,height,res);
    return res;
}

template <size_t CAPACITY, typename T, typename P, typename N>
std::list<T> QuadTree<CAPACITY, T, P, N>::data() const
{
    std::list<T> ans;
    _getData(ans);
    return ans;
}

template <size_t CAPACITY, typename T, typename P, typename N>
std::list<N> QuadTree<CAPACITY, T, P, N>::nodeData() const
{
    return _data;
}

template <size_t CAPACITY, typename T, typename P, typename N>
size_t QuadTree<CAPACITY, T, P, N>::shrinkToFit()
{
    size_t s = _data.size();
    if(!_children) return s;

    s += _children->nw.shrinkToFit();
    s += _children->ne.shrinkToFit();
    s += _children->sw.shrinkToFit();
    s += _children->se.shrinkToFit();

    if(s + _data.size() <= CAPACITY)
    {
        _data.splice(_data.end(),_children->nw._data);
        _data.splice(_data.end(),_children->ne._data);
        _data.splice(_data.end(),_children->sw._data);
        _data.splice(_data.end(),_children->se._data);
    }

    if(s == _data.size())
    {
        _deleteChildren();
    }
    _size = s;
    return s;
}

template <size_t CAPACITY, typename T, typename P, typename N>
size_t QuadTree<CAPACITY, T, P, N>::depth()
{
    size_t ans = 1;

    if(_children)
    {
        ans += DMUtils::maths::max(_children->nw.depth(),_children->ne.depth(),_children->sw.depth(),_children->se.depth());
    }
    return ans;
}

/***************************************************************************/
/******************************** PROTECTED ********************************/
/***************************************************************************/

template <size_t CAPACITY, typename T, typename P, typename N>
QuadTree<CAPACITY, T, P, N>::QuadTreeChild::QuadTreeChild(const sf::Rect<float>& parentZone, int maximumDepth) :
    nw(parentZone.left, parentZone.top, parentZone.width / 2.0f, parentZone.height / 2.0f, maximumDepth),
    ne(parentZone.left + parentZone.width / 2.0f, parentZone.top, parentZone.width / 2.0f, parentZone.height / 2.0f, maximumDepth),
    sw(parentZone.left, parentZone.top + parentZone.height / 2.0f, parentZone.width / 2.0f, parentZone.height / 2.0f, maximumDepth),
    se(parentZone.left + parentZone.width / 2.0f, parentZone.top + parentZone.height / 2.0f, parentZone.width / 2.0f, parentZone.height / 2.0f, maximumDepth)
{
}

template <size_t CAPACITY, typename T, typename P, typename N>
void QuadTree<CAPACITY, T, P, N>::_subdivide(int newDepth)
{
    _deleteChildren();

    _children =  new QuadTreeChild(_coveredZone, newDepth);

    std::list<N> tmp = _data;
    _data.clear();
    _size = 0;
    for(auto& it : tmp)
    {
        insert(it.position, it.data);
    }
}

template <size_t CAPACITY, typename T, typename P, typename N>
void QuadTree<CAPACITY, T, P, N>::_insert(const P& position, const T& item)
{
    if(_children)
    {
        QuadTree<CAPACITY, T, P, N>* target = this;
        int insertCount = 0;

        if(N::belongsTo(_children->nw._coveredZone.left, _children->nw._coveredZone.top, _children->nw._coveredZone.width, _children->nw._coveredZone.height, position))
        {
            ++insertCount;
            target = &_children->nw;
        }
        if(N::belongsTo(_children->ne._coveredZone.left, _children->ne._coveredZone.top, _children->ne._coveredZone.width, _children->ne._coveredZone.height, position))
        {
            ++insertCount;
            target = &_children->ne;
        }
        if(N::belongsTo(_children->sw._coveredZone.left, _children->sw._coveredZone.top, _children->sw._coveredZone.width, _children->sw._coveredZone.height, position))
        {
            ++insertCount;
            target = &_children->sw;
        }
        if(N::belongsTo(_children->se._coveredZone.left, _children->se._coveredZone.top, _children->se._coveredZone.width, _children->se._coveredZone.height, position))
        {
            ++insertCount;
            target = &_children->se;
        }

        if(insertCount == 1)  //only one node wants it
        {
            ++_size;
            target->insert(position, item);
            return;
        }
        else //collides with several children, keep here
        {
            ++_size;
            _data.emplace_back(N(position, item));
            return;
        }
    }

    _data.emplace_back(N(position, item));
    if(_data.size() > CAPACITY)
    {
        if(_maximumDepth == -1)
        {
            _subdivide();
        }
        else if(_maximumDepth - 1 > 0)
        {
            _subdivide(_maximumDepth - 1);
        }
    }
}

template <size_t CAPACITY, typename T, typename P, typename N>
void QuadTree<CAPACITY, T, P, N>::_query(float x, float y, float width, float height, std::list<T>& res) const
{
    sf::Rect<float> region(x, y, width, height);
    for(auto& n : _data)
    {
        if(N::belongsTo(region.left, region.top, region.width, region.height, n.position)) res.push_back(n.data);
    }

    if(_children)
    {
        if(_children->nw._coveredZone.intersects(region)) _children->nw._query(x, y, width, height,res);
        if(_children->ne._coveredZone.intersects(region)) _children->ne._query(x, y, width, height,res);
        if(_children->sw._coveredZone.intersects(region)) _children->sw._query(x, y, width, height,res);
        if(_children->se._coveredZone.intersects(region)) _children->se._query(x, y, width, height,res);
    }
}

template <size_t CAPACITY, typename T, typename P, typename N>
void QuadTree<CAPACITY, T, P, N>::_getData(std::list<T>& ans) const
{
    for(auto n : _data)
    {
        ans.push_back(n.data);
    }

    if(_children)
    {
        _children->nw._getData(ans);
        _children->ne._getData(ans);
        _children->sw._getData(ans);
        _children->se._getData(ans);
    }
}

template <size_t CAPACITY, typename T, typename P, typename N>
void QuadTree<CAPACITY, T, P, N>::_deleteChildren()
{
    if(_children)
    {
        _children->nw._deleteChildren();
        _children->ne._deleteChildren();
        _children->sw._deleteChildren();
        _children->se._deleteChildren();

        delete _children;
        _children = nullptr;
    }
    _size = _data.size();
}
