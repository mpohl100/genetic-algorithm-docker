#pragma once

#include <concepts>
#include <string>

namespace evol {

template<class T>
concept Chromosome = std::semiregular<T> && requires (T t)
{
    t.crossover(t);
    t.mutate();
    {t.toString()} -> std::same_as<std::string>;
};

template<class T, class C>
concept Challenge = std::semiregular<T> &&  Chromosome<C> && requires (T const t, C c)
{
    {t.score(c)} -> std::same_as<double>;
}; 

};