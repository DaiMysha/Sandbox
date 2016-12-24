#ifndef HEADER_QUADTREE
#define HEADER_QUADTREE

#include <SFML/Graphics.hpp>
#include <list>

template <size_t CAPACITY>
class QuadTree
{
    public:

        QuadTree(const QuadTree& other) = delete;
        QuadTree& operator=(const QuadTree& other) = delete;

        QuadTree(QuadTree&& other) = default;
        QuadTree& operator=(QuadTree&& other) = default;

        QuadTree(float width, float height);
        QuadTree(float left, float top, float width, float height);
        ~QuadTree();

        void insert(const sf::Vector2f& item);

        bool remove(const sf::Vector2f& item);
        size_t remove(const sf::Rect<float>& zone);

        void clear();

        size_t size() const;
        //void setArea(const physics::AABB<float>& area);

        std::list<sf::Vector2f> query(float x, float y, float width, float height) const;
        std::list<sf::Vector2f> data() const;
        //std::list<QuadTree::Node> nodeData() const;

        size_t shrinkToFit();

        size_t depth();

    protected:
        void _subdivide();
        inline void _insert(const sf::Vector2f& item);
        inline void _query(float x, float y, float width, float height, std::list<sf::Vector2f>& res) const;
        inline void _getData(std::list<sf::Vector2f>& ans) const;
        inline void _deleteChildren();

        struct QuadTreeChild
        {
            QuadTreeChild(const sf::Rect<float>& parentZone);
            QuadTree<CAPACITY> nw, ne, sw, se;
        };
        sf::Rect<float> _coveredZone;
        std::list<sf::Vector2f> _data;
        QuadTreeChild* _children;

};

#include "QuadTree.tpl"

#endif // HEADER_QUADTREE

