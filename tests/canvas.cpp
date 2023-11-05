#include <catch2/catch_all.hpp>

#include "examples/bubbles/Canvas2D.h"

#include <iostream>

namespace {

TEST_CASE("Canvas", "[canvas]"){
    SECTION("Canvas_Line"){
        auto canvas = bubbles::Canvas2D(10,10);
        canvas.draw_line(bubbles::Point(1,1), bubbles::Point(7,3));
        const auto canvas_pixels = canvas.getPixels();
        std::string result = "..........\n"
                             ".XX.......\n"
                             "..X.......\n"
                             "..X.......\n"
                             "..XX......\n"
                             "...X......\n"
                             "...X......\n"
                             "...X......\n"
                             "..........\n"
                             "..........\n";
        CHECK(canvas_pixels == result);
    }
    SECTION("Canvas_Rectangle"){
        auto canvas = bubbles::Canvas2D(5,5);
        canvas.draw_rectangle(bubbles::Point(1,1), bubbles::Point(3,3));
        const auto canvas_pixels = canvas.getPixels();
        CHECK(canvas_pixels == std::string(".....\n.XXX.\n.X.X.\n.XXX.\n.....\n"));
    }
    SECTION("Canvas_Line_Fuzztest"){
        for(size_t i = 1; i < 9; ++i){
            for(size_t j = 1; j < 9; ++j){
                auto canvas = bubbles::Canvas2D(10,10);
                canvas.draw_line(bubbles::Point(5,5), bubbles::Point(i,j));
                const auto canvas_pixels = canvas.getPixels();
                std::cout << "i: " << i << " j: " << j << std::endl;
                //std::cout << "fixed x:" << 5 << " fixed y:" << 5 << std::endl;
                CHECK(canvas_pixels[5*11+5] == 'X');
                if(canvas_pixels[5*11+5] != 'X'){
                    //std::cout << canvas_pixels << std::endl;
                }
                CHECK(canvas_pixels[i*11+j] == 'X');
                if(canvas_pixels[i*11+j] != 'X'){
                    //std::cout << canvas_pixels << std::endl;
                }
            }
        }
    }
}

}