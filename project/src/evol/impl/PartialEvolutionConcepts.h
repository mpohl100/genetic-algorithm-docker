#pragma once

#include "EvolutionConcepts.h"

#include <concepts>
#include <string>
#include <vector>

namespace evol::partial {

template<class T>
concept PartialChromosome = evol::Chromosome<T> && requires (const T t)
{
    {t.magnitude()} -> std::same_as<double>; // the square root of the sum of the squares of all members 
};

template<class T, class C>
concept PartialChallenge = std::semiregular<T> &&  PartialChromosome<C> && requires (T const t, C c)
{
    {t.score(c)} -> std::same_as<double>;
    {t.grow_generation(c,c, 20 /*the number of partial chromosomes in the next generation*/, 0.0 /*the minimum magnitude of the partial chromosomes*/, 1.0 /*the maximum magnitude of the partial chromosomes*/ )} -> std::same_as<std::vector<C>>;
}; 

}