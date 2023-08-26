#pragma once

#include <random>
#include <stack>

namespace evol{

class Rng{
public:
    Rng();

    std::stack<int> fetchUniform(int from, int to, size_t num) const;
    std::stack<double> fetchNormal( double expValue, double stdDev, size_t num) const;
private:
    std::random_device rd_;
    mutable std::mt19937 gen_;
};

}

