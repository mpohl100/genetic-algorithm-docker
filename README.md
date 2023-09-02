# Genetic algorithm

## What it is
A genetic algorithm is a method to improve a classes member values by trying to breed a generation of instances of the class (a new "generation") and then letting each of those instances face a challenge (a "score" function). The two instances with the best fitness value are chosen to breed a new generation of instances. They again face the challenge and the winners breed the third generation. This algorithm continues until the specified number of generations is reached. In this implementation of a genetic algorithm the instances are called "Chromosomes" as chromosomes have the ability to cross over with other chromosomes and they also have the ability to apply random mutations to their genes. Both operations must be implemented to guarantee a smart way of breeding new generations, as one wants to preserve the quality already reached by crossing over but one also wants to get lucky to hit an even better chromosome by applying a random mutation which by chance outperforms the parents in the "score" function. 

## When you can use it

### It is possible to write a good fitness function
It is very important that one is able to write a good fitness function. It is very useful if you don't return 0 or 1 as a fitness value but rather a gradient of values so that the evolution function is guided in the right direction.
If implementing a good gradual fitness function is not possible then you will not reach good performance with the evolution function and it is subject to chance if you even converge to the best chromosome.

### Brute force takes too long
In case iterating over all chromosomes and calculating their fitness value takes too long too complete, one can try to use the genetic algorithm implementation of this library to get there way faster. It is even better if you have a good initial guess of your chromosome which can then lead to fast convergence.

## How you can use it
This library uses the C++20 feature concepts to guide you what kind of requirements your types must fulfill.
A chromosome is defined as follows:
``` 
template<class T>
concept Chromosome = std::semiregular<T> && requires (T t)
{
    t.crossover(t);
    t.mutate();
} && requires (const T t){
    {t.toString()} -> std::same_as<std::string>;
};
``` 
A challenge function is defined as follows:
``` 
template<class T, class C>
concept Challenge = std::semiregular<T> &&  Chromosome<C> && requires (T const t, C c)
{
    {t.score(c)} -> std::same_as<double>;
}; 
``` 
A simple example for implementing a chromosome class and a challenge class can be found in the file src/Math.h.




