#include <DMUtils/maths.hpp>

#include "QuadTree.hpp"

template <size_t CAPACITY>
QuadTree<CAPACITY>::QuadTree(float width, float height) : QuadTree(0, 0, width, height)
{
}

template <size_t CAPACITY>
QuadTree<CAPACITY>::QuadTree(float left, float top, float width, float height) : _coveredZone(left, top, width, height), _children(nullptr)
{
}

template<size_t CAPACITY>
QuadTree<CAPACITY>::~QuadTree()
{
    _deleteChildren();
}

template <size_t CAPACITY>
void QuadTree<CAPACITY>::insert(const sf::Vector2f& item)
{
    if(_coveredZone.contains(item)) _insert(item);
}

template <size_t CAPACITY>
bool QuadTree<CAPACITY>::remove(const sf::Vector2f& item)
{
    auto it = std::find_if(_data.begin(),_data.end(),[&item](const sf::Vector2f& n) {
			return n == item;
		});

    if(it != _data.end())
    {
        _data.erase(it);
        return true;
    }

    if(_children)
    {
        bool res = false;
        res = _children->nw.remove(item);
        if(!res && _children->ne.remove(item)) res =  true;
        if(!res && _children->sw.remove(item)) res =  true;
        if(!res && _children->se.remove(item)) res =  true;
    }
    return false;
}

template <size_t CAPACITY>
size_t QuadTree<CAPACITY>::remove(const sf::Rect<float>& zone)
{
    size_t res = 0;
    if(!_coveredZone.intersects(zone)) return res;

    _data.remove_if([&res,&zone](const sf::Vector2f& n){
        if(zone.contains(n))
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

    shrinkToFit();

    return res;
}

template <size_t CAPACITY>
void QuadTree<CAPACITY>::clear()
{
    if(_children)
    {
        _deleteChildren();
    }
    _data.clear();
}

template <size_t CAPACITY>
size_t QuadTree<CAPACITY>::size() const
{
    size_t s = _data.size();
    if(_children)
    {
        s += _children->nw.size() + _children->ne.size() + _children->sw.size() + _children->se.size();
    }
    return s;
}

//void QuadTree<CAPACITY>::setArea(const physics::AABB<float>& area)

template <size_t CAPACITY>
std::list<sf::Vector2f> QuadTree<CAPACITY>::query(float x, float y, float width, float height) const
{
    if(!_coveredZone.intersects(sf::Rect<float>(x, y, width, height))) return std::list<sf::Vector2f>();
    std::list<sf::Vector2f> res;
    _query(x,y,width,height,res);
    return res;
}

template <size_t CAPACITY>
std::list<sf::Vector2f> QuadTree<CAPACITY>::data() const
{
    std::list<sf::Vector2f> ans;
    _getData(ans);
    return ans;
}

//std::list<QuadTree<CAPACITY>::Node> QuadTree<CAPACITY>::nodeData() const

template <size_t CAPACITY>
size_t QuadTree<CAPACITY>::shrinkToFit()
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
    return s;
}

template <size_t CAPACITY>
size_t QuadTree<CAPACITY>::depth()
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

template <size_t CAPACITY>
QuadTree<CAPACITY>::QuadTreeChild::QuadTreeChild(const sf::Rect<float>& parentZone) :
    nw(parentZone.left, parentZone.top, parentZone.width / 2.0f, parentZone.height / 2.0f),
    ne(parentZone.left + parentZone.width / 2.0f, parentZone.top, parentZone.width / 2.0f, parentZone.height / 2.0f),
    sw(parentZone.left, parentZone.top + parentZone.height / 2.0f, parentZone.width / 2.0f, parentZone.height / 2.0f),
    se(parentZone.left + parentZone.width / 2.0f, parentZone.top + parentZone.height / 2.0f, parentZone.width / 2.0f, parentZone.height / 2.0f)
{
}

template <size_t CAPACITY>
void QuadTree<CAPACITY>::_subdivide()
{
    _deleteChildren();

    _children =  new QuadTreeChild(_coveredZone);

    std::list<sf::Vector2f> tmp = _data;
    _data.clear();
    for(auto& it : tmp) {
        insert(it);
    }
}


template <size_t CAPACITY>
void QuadTree<CAPACITY>::_insert(const sf::Vector2f& item)
{
    if(_children)
    {
        QuadTree<CAPACITY>* target = this;
        int insertCount = 0;

        if(_children->nw._coveredZone.contains(item))
        {
            ++insertCount;
            target = &_children->nw;
        }
        if(_children->ne._coveredZone.contains(item))
        {
            ++insertCount;
            target = &_children->ne;
        }
        if(_children->sw._coveredZone.contains(item))
        {
            ++insertCount;
            target = &_children->sw;
        }
        if(_children->se._coveredZone.contains(item))
        {
            ++insertCount;
            target = &_children->se;
        }

        if(insertCount == 1)  //only one node wants it
        {
            target->insert(item);
            return;
        }
        else //collides with several children, keep here
        {
            _data.emplace_back(item);
            return;
        }
    }

    _data.emplace_back(item);
    if(_data.size() > CAPACITY) {
        _subdivide();
    }
}

template <size_t CAPACITY>
void QuadTree<CAPACITY>::_query(float x, float y, float width, float height, std::list<sf::Vector2f>& res) const
{
    sf::Rect<float> region(x, y, width, height);
    for(auto& n : _data)
    {
        if(region.contains(n)) res.push_back(n);
    }

    if(_children)
    {
        if(_children->nw._coveredZone.intersects(region)) _children->nw._query(x, y, width, height,res);
        if(_children->ne._coveredZone.intersects(region)) _children->ne._query(x, y, width, height,res);
        if(_children->sw._coveredZone.intersects(region)) _children->sw._query(x, y, width, height,res);
        if(_children->se._coveredZone.intersects(region)) _children->se._query(x, y, width, height,res);
    }
}

template <size_t CAPACITY>
void QuadTree<CAPACITY>::_getData(std::list<sf::Vector2f>& ans) const
{
    ans.insert(ans.end(), _data.begin(), _data.end());

    if(_children) {
        _children->nw._getData(ans);
        _children->ne._getData(ans);
        _children->sw._getData(ans);
        _children->se._getData(ans);
    }
}

template <size_t CAPACITY>
void QuadTree<CAPACITY>::_deleteChildren()
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
}

