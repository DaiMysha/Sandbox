#include <iostream>
#include <ctime>
#include <list>

#include "QuadTree.hpp"

int  N_ITEMS = 10000;
#define SIZE  1000

int placeholderValue = 0;

struct Point {
    float x,y;

    Point() : x(0), y(0) {}
    Point(float xx, float yy) : x(xx), y(yy) {}
    Point(const Point& p) : x(p.x), y(p.y) {}
    bool operator==(const Point& p) const {
        return this == &p;
    }
};

Point* points = nullptr;

std::vector<Point> vPoint;
std::vector<Point*> vPointPtr;
QuadTree<4, int> qPoint4(SIZE,SIZE);
QuadTree<8, int> qPoint10(SIZE,SIZE);

Point makeRandomPoint(float xmax, float ymax) {
    return Point(rand()%(int)xmax, rand()%(int)ymax);
}

sf::Rect<float> makeRandomAABB(float xmax, float ymax) {
    float x = rand()%(int)xmax;
    float y = rand()%(int)ymax;
    return sf::Rect<float>(x,y,rand()%(int)(xmax-x),rand()%(int)(ymax-y));
}

void fillVec() {
    for(int i=0;i<N_ITEMS;++i) vPoint.push_back(points[i]);
}

void fillVecPtr() {
    for(int i=0;i<N_ITEMS;++i) vPointPtr.push_back(&points[i]);
}

template<size_t CAPACITY, typename T>
void fillTree(QuadTree<CAPACITY, T>& tree) {
    for(int i=0;i<N_ITEMS;++i) tree.insert(sf::Vector2f(points[i].x, points[i].y), placeholderValue);
}

void fill() {
    fillVec();
    fillVecPtr();
    fillTree(qPoint4);
    fillTree(qPoint10);
}

void reset() {
    vPoint.clear();
    vPointPtr.clear();
    qPoint4.clear();
    qPoint10.clear();
    fill();
}

std::list<const Point*> query(const std::vector<Point>& vec, const sf::Rect<float>& map) {
    std::list<const Point*> ans;
    for(size_t i=0;i<vec.size();++i) {
        if(map.contains(vec[i].x,vec[i].y)) ans.push_back(&vec[i]);
    }
    return ans;
}

std::list<const Point*> query(const std::vector<Point*>& vec, const sf::Rect<float>& map) {
    std::list<const Point*> ans;
    for(size_t i=0;i<vec.size();++i) {
        if(map.contains(vec[i]->x,vec[i]->y)) ans.push_back(vec[i]);
    }
    return ans;
}

int main(int argc, char** argv) {
    srand(time(0));

    std::cout << "Point amount ?\n>";
    std::cin>>N_ITEMS;
    std::cout<<std::endl;

    points = new Point[N_ITEMS];
    //fills the two data
    clock_t clock_before, clock_after;

    for(int i = 0; i < N_ITEMS; ++i) {
        points[i] = makeRandomPoint(SIZE,SIZE);
    }

    clock_t clock_vector, clock_tree;

    {
        std::cout << "****** FILLING POINTS *****" << std::endl;
        clock_before = clock();
        fillVec();
        clock_after = clock();

        clock_vector = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tvector     : " << clock_vector << " ms" << std::endl;

        clock_before = clock();
        fillVecPtr();
        clock_after = clock();

        clock_vector = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tvectorPtr  : " << clock_vector << " ms" << std::endl;

        clock_before = clock();
        fillTree(qPoint4);
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree4  : " << clock_tree << " ms" << std::endl;
        std::cout << "\t\tdepth : " << qPoint4.depth() << std::endl;

        clock_before = clock();
        fillTree(qPoint10);
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree10 : " << clock_tree << " ms" << std::endl;
        std::cout << "\t\tdepth : " << qPoint10.depth() << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "****** GET SIZE *****" << std::endl;
        clock_before = clock();
        size_t s = vPoint.size();
        clock_after = clock();

        clock_vector = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tvector     : " << clock_vector << " ms (" << s << ")" << std::endl;

        clock_before = clock();
        s = qPoint4.size();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree4  : " << clock_tree << " ms (" << s << ")" << std::endl;

        clock_before = clock();
        s = qPoint10.size();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree10 : " << clock_tree << " ms (" << s << ")" << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "****** CLEAR POINTS *****" << std::endl;
        clock_before = clock();
        vPoint.clear();
        clock_after = clock();

        clock_vector = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tvector     : " << clock_vector << " ms" << std::endl;

        clock_before = clock();
        qPoint4.clear();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree4  : " << clock_tree << " ms" << std::endl;

        clock_before = clock();
        qPoint10.clear();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree10 : " << clock_tree << " ms" << std::endl;
        std::cout << std::endl;
        reset();
    }
    if(false)
    {
        std::cout << "****** DEL 1/2 POINTS *****" << std::endl;
        clock_before = clock();
        for(int i=N_ITEMS-1;i>=0;i-=2) std::remove(vPoint.begin(),vPoint.end(),vPoint[i]);
        clock_after = clock();

        clock_vector = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tvector     : " << clock_vector << " ms" << std::endl;

        auto ll = qPoint4.data();
        clock_before = clock();
        for(auto it = ll.begin(); it != ll.end();) {
            qPoint4.remove(*it);
            ++it; ++it;
        }
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree4  : " << clock_tree << " ms" << std::endl;
        std::cout << std::endl;

        ll = qPoint10.data();
        clock_before = clock();
        for(auto it = ll.begin(); it != ll.end();) {
            qPoint10.remove(*it);
            ++it; ++it;
        }
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree10 : " << clock_tree << " ms" << std::endl;
        reset();
    }

    {
        sf::Rect<float> map(0,0,SIZE,SIZE);
        size_t n;
        std::cout << "****** QUERY WHOLE MAP POINTS *****" << std::endl;
        clock_before = clock();
        n = query(vPoint,map).size();
        clock_after = clock();

        clock_vector = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tvector     : " << clock_vector << " ms (" << n << ")" << std::endl;

        clock_before = clock();
        n = query(vPointPtr,map).size();
        clock_after = clock();

        clock_vector = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tvectorPtr  : " << clock_vector << " ms (" << n << ")" << std::endl;

        auto ll = qPoint4.data();
        clock_before = clock();
        n = qPoint4.query(map.left, map.top, map.width, map.height).size();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree4  : " << clock_tree << " ms (" << n << ")" << std::endl;

        ll = qPoint10.data();
        clock_before = clock();
        n = qPoint10.query(map.left, map.top, map.width, map.height).size();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree10 : " << clock_tree << " ms (" << n << ")" << std::endl;
        std::cout << std::endl;
    }

    {
        sf::Rect<float> map(makeRandomAABB(SIZE,SIZE));
        size_t n;
        std::cout << "****** QUERY RANDOM MAP POINTS *****" << std::endl;
        std::cout << "\tmap : " << map.left << ", " << map.top << ", " << map.width << ", " << map.height << std::endl;
        clock_before = clock();
        n = query(vPoint,map).size();
        clock_after = clock();

        clock_vector = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tvector     : " << clock_vector << " ms (" << n << "/" << N_ITEMS << ")" << std::endl;

        clock_before = clock();
        n = qPoint4.query(map.left, map.top, map.width, map.height).size();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree4  : " << clock_tree << " ms (" << n << "/" << N_ITEMS << ")" << std::endl;

        clock_before = clock();
        n = qPoint10.query(map.left, map.top, map.width, map.height).size();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree10 : " << clock_tree << " ms (" << n << "/" << N_ITEMS << ")" << std::endl;
        std::cout << std::endl;
    }

    {
        sf::Rect<float> map(SIZE/4,SIZE/4,SIZE/2,SIZE/2);
        size_t n;
        std::cout << "****** QUERY HALF MAP POINTS *****" << std::endl;
        std::cout << "\tmap : " << map.left << ", " << map.top << ", " << map.width << ", " << map.height << std::endl;
        clock_before = clock();
        n = query(vPoint,map).size();
        clock_after = clock();

        clock_vector = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tvector     : " << clock_vector << " ms (" << n << "/" << N_ITEMS << ")" << std::endl;

        clock_before = clock();
        n = qPoint4.query(map.left, map.top, map.width, map.height).size();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree4  : " << clock_tree << " ms (" << n << "/" << N_ITEMS << ")" << std::endl;

        clock_before = clock();
        n = qPoint10.query(map.left, map.top, map.width, map.height).size();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree10 : " << clock_tree << " ms (" << n << "/" << N_ITEMS << ")" << std::endl;
        std::cout << std::endl;
    }

    {
        std::cout << "****** GET DATA (QuadTree only) *****" << std::endl;
        clock_before = clock();
        auto l = qPoint4.data();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree4  : " << clock_tree << " ms (" << l.size() << ")" << std::endl;

        clock_before = clock();
        l = qPoint4.data();
        clock_after = clock();
        clock_tree = (clock_after - clock_before)*1000/CLOCKS_PER_SEC;
        std::cout << "\tquadTree10 : " << clock_tree << " ms (" << l.size() << ")" << std::endl;
        std::cout << std::endl;
    }

    delete[] points;

    return 0;
}
