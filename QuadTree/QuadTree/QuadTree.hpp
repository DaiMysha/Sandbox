#ifndef HEADER_QUADTREE
#define HEADER_QUADTREE

#include <SFML/Graphics.hpp>
#include <list>

/**

The N type represents the Node structure
it needs to be defined as such (template can be an actual type):

template <typename T, typename P>
struct Node
{
    Node();
    Node(const P& p, const T& d); ///mandatory
    static inline bool belongsTo(float left, float top, float width, float height, const P& p); ///mandatory
    P position;
    T data;
};

P represents the position element of the node
T represents the data stored in the node

belongsTo must return true if P is inside the box defined by {left, top, width, height} according
to your specifications

**/

template <size_t CAPACITY, typename T, typename P, typename N>
class QuadTree
{
    public:
        QuadTree(const QuadTree& other) = delete;
        QuadTree& operator=(const QuadTree& other) = delete;

        QuadTree(QuadTree&& other) = default;
        QuadTree& operator=(QuadTree&& other) = default;

        QuadTree(float width, float height, int maximumDepth = -1);
        QuadTree(float left, float top, float width, float height, int maximumDepth = -1);
        ~QuadTree();

        void insert(const P& item, const T& data);

        bool remove(const T& item);
        size_t remove(const sf::Rect<float>& zone);

        void clear();

        size_t size() const;
        size_t getNodeCount() const;
        size_t getCapacity() const;

        std::list<T> query(float x, float y, float width, float height) const;
        std::list<T> data() const;
        std::list<N> nodeData() const;

        size_t shrinkToFit();

        size_t depth();

    protected:
        void _subdivide(int newDepth = -1);
        inline void _insert(const P& position, const T& item);
        inline void _query(float x, float y, float width, float height, std::list<T>& res) const;
        inline void _getData(std::list<T>& ans) const;
        inline void _deleteChildren();

        struct QuadTreeChild
        {
            QuadTreeChild(const sf::Rect<float>& parentZone, int maximumDepth);
            QuadTree<CAPACITY, T, P, N> nw, ne, sw, se;
        };
        sf::Rect<float> _coveredZone;
        std::list<N> _data;
        QuadTreeChild* _children;
        size_t _size; //represents the size of the current tree and all the trees under it
        int _maximumDepth;
};

#include "QuadTree.tpl"

#endif // HEADER_QUADTREE

