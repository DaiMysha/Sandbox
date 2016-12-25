#include <DMUtils/maths.hpp>

#include "QuadTree.hpp"

/***************** NODE *****************/

template <size_t CAPACITY, typename T>
QuadTree<CAPACITY, T>::Node::Node()
{
}

template <size_t CAPACITY, typename T>
QuadTree<CAPACITY, T>::Node::Node(const sf::Vector2f& p, const T& d) : position(p), data(d)
{
}

template <size_t CAPACITY, typename T>
QuadTree<CAPACITY, T>::QuadTree(float width, float height) : QuadTree(0, 0, width, height)
{
}

template <size_t CAPACITY, typename T>
QuadTree<CAPACITY, T>::QuadTree(float left, float top, float width, float height) :
    _coveredZone(left, top, width, height),
    _children(nullptr),
    _size(0)
{
}

template<size_t CAPACITY, typename T>
QuadTree<CAPACITY, T>::~QuadTree()
{
    _deleteChildren();
}

template <size_t CAPACITY, typename T>
void QuadTree<CAPACITY, T>::insert(const sf::Vector2f& position, const T& item)
{
    if(_coveredZone.contains(position)) _insert(position, item);
}

template <size_t CAPACITY, typename T>
bool QuadTree<CAPACITY, T>::remove(const T& item)
{
    auto it = std::find_if(_data.begin(),_data.end(),[&item](const Node& n) {
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

template <size_t CAPACITY, typename T>
size_t QuadTree<CAPACITY, T>::remove(const sf::Rect<float>& zone)
{
    size_t res = 0;
    if(!_coveredZone.intersects(zone)) return res;

    _data.remove_if([&res,&zone](const Node& n){
        if(zone.contains(n.position))
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

template <size_t CAPACITY, typename T>
void QuadTree<CAPACITY, T>::clear()
{
    if(_children)
    {
        _deleteChildren();
    }
    _size = 0;
    _data.clear();
}

template <size_t CAPACITY, typename T>
size_t QuadTree<CAPACITY, T>::size() const
{
    return _size;
}

//void QuadTree<CAPACITY, T>::setArea(const physics::AABB<float>& area)

template <size_t CAPACITY, typename T>
std::list<T> QuadTree<CAPACITY, T>::query(float x, float y, float width, float height) const
{
    if(!_coveredZone.intersects(sf::Rect<float>(x, y, width, height))) return std::list<T>();
    std::list<T> res;
    _query(x,y,width,height,res);
    return res;
}

template <size_t CAPACITY, typename T>
std::list<T> QuadTree<CAPACITY, T>::data() const
{
    std::list<T> ans;
    _getData(ans);
    return ans;
}

template <size_t CAPACITY, typename T>
std::list<typename QuadTree<CAPACITY, T>::Node> QuadTree<CAPACITY, T>::nodeData() const
{
    return _data;
}

//std::list<QuadTree<CAPACITY, T>::Node> QuadTree<CAPACITY, T>::nodeData() const

template <size_t CAPACITY, typename T>
size_t QuadTree<CAPACITY, T>::shrinkToFit()
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

template <size_t CAPACITY, typename T>
size_t QuadTree<CAPACITY, T>::depth()
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

template <size_t CAPACITY, typename T>
QuadTree<CAPACITY, T>::QuadTreeChild::QuadTreeChild(const sf::Rect<float>& parentZone) :
    nw(parentZone.left, parentZone.top, parentZone.width / 2.0f, parentZone.height / 2.0f),
    ne(parentZone.left + parentZone.width / 2.0f, parentZone.top, parentZone.width / 2.0f, parentZone.height / 2.0f),
    sw(parentZone.left, parentZone.top + parentZone.height / 2.0f, parentZone.width / 2.0f, parentZone.height / 2.0f),
    se(parentZone.left + parentZone.width / 2.0f, parentZone.top + parentZone.height / 2.0f, parentZone.width / 2.0f, parentZone.height / 2.0f)
{
}

template <size_t CAPACITY, typename T>
void QuadTree<CAPACITY, T>::_subdivide()
{
    _deleteChildren();

    _children =  new QuadTreeChild(_coveredZone);

    std::list<Node> tmp = _data;
    _data.clear();
    _size = 0;
    for(auto& it : tmp)
    {
        insert(it.position, it.data);
    }
}


template <size_t CAPACITY, typename T>
void QuadTree<CAPACITY, T>::_insert(const sf::Vector2f& position, const T& item)
{
    if(_children)
    {
        QuadTree<CAPACITY, T>* target = this;
        int insertCount = 0;

        if(_children->nw._coveredZone.contains(position))
        {
            ++insertCount;
            target = &_children->nw;
        }
        if(_children->ne._coveredZone.contains(position))
        {
            ++insertCount;
            target = &_children->ne;
        }
        if(_children->sw._coveredZone.contains(position))
        {
            ++insertCount;
            target = &_children->sw;
        }
        if(_children->se._coveredZone.contains(position))
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
            _data.emplace_back(Node(position, item));
            return;
        }
    }

    _data.emplace_back(Node(position, item));
    if(_data.size() > CAPACITY) {
        _subdivide();
    }
}

template <size_t CAPACITY, typename T>
void QuadTree<CAPACITY, T>::_query(float x, float y, float width, float height, std::list<T>& res) const
{
    sf::Rect<float> region(x, y, width, height);
    for(auto& n : _data)
    {
        if(region.contains(n.position)) res.push_back(n.data);
    }

    if(_children)
    {
        if(_children->nw._coveredZone.intersects(region)) _children->nw._query(x, y, width, height,res);
        if(_children->ne._coveredZone.intersects(region)) _children->ne._query(x, y, width, height,res);
        if(_children->sw._coveredZone.intersects(region)) _children->sw._query(x, y, width, height,res);
        if(_children->se._coveredZone.intersects(region)) _children->se._query(x, y, width, height,res);
    }
}

template <size_t CAPACITY, typename T>
void QuadTree<CAPACITY, T>::_getData(std::list<T>& ans) const
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

template <size_t CAPACITY, typename T>
void QuadTree<CAPACITY, T>::_deleteChildren()
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

