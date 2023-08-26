#include "Rng.h"

namespace evol{

Rng::Rng()
    : rd_()
    , gen_(rd_())
{} 

std::stack<int> Rng::fetchUniform(int from, int to, size_t num) const
{
    static thread_local std::mt19937 gen;
    double from_d = double(from);
    double to_d = double(to) - std::numeric_limits<double>::min();
    std::uniform_real_distribution<double> dist{from_d, to_d};
    std::stack<int> ret;
    for(size_t i = 0; i < num; ++i)
        ret.push(int(dist(gen)));
    return ret;
}

std::stack<double> Rng::fetchNormal( double expValue, double stdDev, size_t num) const
{
    std::normal_distribution<double> dist{expValue, stdDev};
    std::stack<double> ret;
    for(size_t i = 0; i < num; ++i)
        ret.push(dist(gen_));
    return ret;
}

}

