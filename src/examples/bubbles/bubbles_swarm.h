#include "Canvas2D.h"

#include "evol.h"



namespace bubbles {

class BubblesSwarm {
public:
    double area() const;
};

BubblesSwarm bubbles_algorithm(const Canvas2D& canvas, const Point& point);

} 