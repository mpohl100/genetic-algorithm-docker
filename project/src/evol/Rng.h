#pragma once

#include <random>
#include <stack>

namespace evol{

class Rng{
public:
    Rng()
        : rd_("hw")
        , gen_(rd_())
    {} 

    std::stack<int> fetchUniform(int from, int to, size_t num)
    {
        std::mt19937 gen(rd_());
        double from_d = double(from);
        double to_d = double(to) - std::numeric_limits<double>::min();
        std::uniform_real_distribution<double> dist{from_d, to_d};
        std::stack<int> ret;
        for(size_t i = 0; i < num; ++i)
            ret.push(int(dist(gen)));
        return ret;
    }

    std::stack<double> fetchNormal( double expValue, double stdDev, size_t num) const
    {
        std::normal_distribution<double> dist{expValue, stdDev};
        std::stack<double> ret;
        for(size_t i = 0; i < num; ++i)
            ret.push(dist(gen_));
        return ret;
    }
private:
    std::random_device rd_;
    mutable std::mt19937 gen_;
};

}

