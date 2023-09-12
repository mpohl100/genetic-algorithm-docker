#include "Canvas2D.h"

#include <stdexcept>
#include <iostream>

namespace path{

Point::Point(int xx, int yy)
: x(xx)
, y(yy)
{}

Canvas2D::Canvas2D(int xx, int yy)
: _x(xx)
, _y(yy)
, _pixels(xx, std::vector<int>(yy, 0))
{}

void Canvas2D::draw_rectangle([[maybe_unused]] const Point& tl, [[maybe_unused]] const Point& br)
{
    draw_line(tl, Point(br.x, tl.y));
    draw_line(Point(br.x, tl.y), br);
    draw_line(br, Point(tl.x, br.y));
    draw_line(Point(tl.x, br.y), tl);
}

std::string Canvas2D::getPixels() const
{
    std::string ret;
    for(const auto& line : _pixels){
        for(const auto& val : line){
            if(val > 0){
                ret += "X";
            }
            else{
                ret += ".";
            }
        }
        ret += "\n";
    }
    return ret;
}

void Canvas2D::draw_line(const Point& start, const Point& end)
{
    int dX = end.x - start.x;
    int dY = end.y - start.y;
    Point current_point = start;
    //std::cout << "dX=" << dX << "; dY=" << dY << '\n';  
    const auto move_coord = [](int& d, int& point){
        if(d <= 0) {
            d++;
            point--;
        }   
        else {
            d--;
            point++;
        }
    };
    while(true){
        if(dX == 0 && dY == 0){
            if(current_point != end){
                throw std::runtime_error("end point not hit in draw_line.");
            }
            break;
        }
        //std::cout << "setting point to 1: x=" << current_point.x << "; y=" << current_point.y << "; dX=" << dX << "; dY=" << dY << '\n';  
        _pixels[current_point.x][current_point.y] = 1;
        if(std::abs(dX) >= std::abs(dY)){
            move_coord(dX, current_point.x);
        }
        else{
            move_coord(dY, current_point.y);
        }
    }
}

}