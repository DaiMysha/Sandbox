#ifndef HEADER_QUADTREE
#define HEADER_QUADTREE

#include <SFML/Graphics.hpp>
#include <list>

template <size_t CAPACITY, typename T>
class QuadTree
{
    public:
        struct Node
        {
            Node();
            Node(const sf::Vector2f& p, const T& d);
            sf::Vector2f position;
            T data;
        };
        QuadTree(const QuadTree& other) = delete;
        QuadTree& operator=(const QuadTree& other) = delete;

        QuadTree(QuadTree&& other) = default;
        QuadTree& operator=(QuadTree&& other) = default;

        QuadTree(float width, float height);
        QuadTree(float left, float top, float width, float height);
        ~QuadTree();

        void insert(const sf::Vector2f& item, const T& data);

        bool remove(const T& item);
        size_t remove(const sf::Rect<float>& zone);

        void clear();

        size_t size() const;

        std::list<T> query(float x, float y, float width, float height) const;
        std::list<T> data() const;
        std::list<QuadTree::Node> nodeData() const;

        size_t shrinkToFit();

        size_t depth();

    protected:
        void _subdivide();
        inline void _insert(const sf::Vector2f& position, const T& item);
        inline void _query(float x, float y, float width, float height, std::list<T>& res) const;
        inline void _getData(std::list<T>& ans) const;
        inline void _deleteChildren();

        struct QuadTreeChild
        {
            QuadTreeChild(const sf::Rect<float>& parentZone);
            QuadTree<CAPACITY, T> nw, ne, sw, se;
        };
        sf::Rect<float> _coveredZone;
        std::list<Node> _data;
        QuadTreeChild* _children;
        size_t _size; //represents the size of the current tree and all the trees under it

};

#include "QuadTree.tpl"

#endif // HEADER_QUADTREE

