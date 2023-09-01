/* Copyright (C) 2018-2023 Michael Pohl - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license, which unfortunately won't be
 * written for another century.
 */

#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <iostream>
#include <map>
#include <optional>
#include <random>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>



namespace evol{

// -----------------------------------------------------------------------------------------------
// class Rng
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


// -----------------------------------------------------------------------------------------------
// Evolution concepts

template<class T>
concept Chromosome = std::semiregular<T> && requires (T t)
{
    t.crossover(t);
    t.mutate();
} && requires (const T t){
    {t.toString()} -> std::same_as<std::string>;
};

template<class T, class C>
concept Challenge = std::semiregular<T> &&  Chromosome<C> && requires (T const t, C c)
{
    {t.score(c)} -> std::same_as<double>;
}; 

// -----------------------------------------------------------------------------------------------
// Evolution Impl

template<Chromosome Chrom, class Chall>
requires Challenge<Chall, Chrom>
std::multimap<double, const Chrom*>
fitnessCalculation(std::vector<Chrom> const& candidates, Chall const& challenge)
{
	std::multimap<double, const Chrom*> ret;
	for (auto& cand : candidates)
		ret.insert(std::make_pair(challenge.score(cand), &cand));
	return ret;
}

template<Chromosome Chrom>
std::vector<const Chrom*>
selectMatingPool(std::multimap<double, const Chrom*> const& fitness, int sep = 2)
{
	std::vector<const Chrom*> ret;
	int i = 0;
	for (auto it = fitness.rbegin(); it != fitness.rend(); ++it)
	{
		ret.push_back(it->second);
		if (++i > int(fitness.size()) / sep - 1)
			break;
	}
	return ret;
}

template<Chromosome Chrom>
void
crossover(
	typename std::vector<Chrom>::iterator parentsBeg,
	typename std::vector<Chrom>::iterator parentsEnd,
	typename std::vector<Chrom>::iterator offspringBeg,
	typename std::vector<Chrom>::iterator offspringEnd
)
{
	int dadIdx = 0; // the winner gets to spread his genes everywhere
	for (; parentsBeg != parentsEnd; ++parentsBeg) {
		*offspringBeg = *parentsBeg;
		offspringBeg->crossover(*(parentsBeg + dadIdx));
		offspringBeg++;
		if (offspringBeg == offspringEnd)
			return;
	}
}

template<Chromosome Chrom>
void randomMutation(
	typename std::vector<Chrom>::iterator offspringBeg,
	typename std::vector<Chrom>::iterator offspringEnd
)
{
	for (; offspringBeg != offspringEnd; ++offspringBeg)
		offspringBeg->mutate();
}


// -----------------------------------------------------------------------------------------------
// Evolution 

// This library encapsulates evolutional learning through genetic algorithms 
// The library is header only and easy to use. 
// You have a type (the chromosome) whose values you want to optimize through evolutional learning
// and you have a challenge your type needs to master
// 

// These functions must be added to your chromosome type (the one you want to optimize)
//struct Chromosome {
//	void crossover(Chromosome const& other); make sure to randomly choose what data members should be taken from this or other
//	void mutate(); make sure to not randomly mutate too much of your chromosome type (only one data member at a time is recommended)
// std::string toString() const; provide some output for the shape of the chromosome
//};

// These functions must be added to your challenge type
//struct Challenge {
//	double score(Chromosome const& chromosome); the chromosome faces the challenge and its performance needs to be evaluated with a double (0 means bad, the higher the better the performance)
//};

struct EvolutionOptions{
	size_t num_generations; // the number of generations to cross
	size_t log_level = 1;  // logging level to see how far the algorithm progressed
	std::ostream* out; // the ostream to stream the logging to
};

template<Chromosome Chrom, class Chall> 
requires Challenge<Chall, Chrom>
std::vector<Chrom>
evolution(
	std::vector<Chrom> population, // feed with a few (20 recommended) random chromosomes
	Chall const& challenge, // the challenge 
	double& winningAccuracy, // the winning performance is an out parameter
	EvolutionOptions const& options // the evolution options
)
{
	auto candidates = population;
	for (size_t i = 0; i < options.num_generations; ++i) {
		// let the chromosomes face the challenge
		std::multimap<double, const Chrom*> fitness = fitnessCalculation(candidates, challenge);
		// logging
		if (options.log_level >= 1) {
			if(options.out) *options.out << "generation nr. " << i + 1 << " / " << options.num_generations << '\n';
			if (options.log_level >= 2) {
				for (auto& f : fitness)
					if(options.out) *options.out << "  fitness: " << f.first << '\n' << f.second->toString() << '\n';
				if(options.out) *options.out << '\n';
			}
		}
		// half of the chromosomes are winners
		std::vector<const Chrom*> winners = selectMatingPool(fitness, 2);
		// return the winner of the last generation
		if (i >= options.num_generations - 1)
		{
			std::vector<Chrom> ret;
			for (auto w : winners)
				ret.push_back(*w);
			winningAccuracy = fitness.rbegin()->first;
			return ret;
		}
		// sort the chromosomes according to their fitness
		typename std::vector<Chrom>::iterator sep = std::stable_partition(
			candidates.begin(), candidates.end(), 
			[&winners](Chrom const& ch) {
			return std::find_if(winners.begin(), winners.end(),
				[&ch](const Chrom* address) { return &ch == address; }) != winners.end();
		});
		// cross parents
		crossover<Chrom>(candidates.begin(), sep, sep, candidates.end());
		// mutate children
		randomMutation<Chrom>(sep, candidates.end());
	}
	return candidates;
}

namespace partial {

// -----------------------------------------------------------------------------------------------
// Partial Evolution concepts 

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


// -----------------------------------------------------------------------------------------------
// Partial Evolution challenge

// inherit from this type in order to use the default implementation of grow generation
template<PartialChromosome Chrom>
struct DefaultPartialChallenge{
	std::vector<Chrom> grow_generation(Chrom parent1, const Chrom& parent2, size_t num_children /*the number of partial chromosomes in the next generation*/, double min_magnitude /*the minimum magnitude of the partial chromosomes*/, double max_magnitude /*the maximum magnitude of the partial chromosomes*/ ) const
	{
		std::vector<Chrom> ret;
		const auto develop = [min_magnitude, max_magnitude](Chrom& chrom, bool initialMutate){
			if(initialMutate){
				chrom.mutate();
			}
            const auto try_n_times = [min_magnitude, max_magnitude](Chrom chrom, size_t num) -> std::optional<Chrom>
            {
                size_t i = 0;
			    while(min_magnitude > chrom.magnitude() || chrom.magnitude() > max_magnitude){
				    chrom.mutate();
				    i++;
                    if(i >= num){
                        break;
                    }
			    }
                if(min_magnitude <= chrom.magnitude() && max_magnitude <= max_magnitude){
                    return chrom;
                }
                else return std::nullopt;
            };
            std::optional<Chrom> opt_chrom = std::nullopt;
            size_t j = 0;
            while(opt_chrom == std::nullopt && j <= 1000){
                opt_chrom = try_n_times(chrom, 1000);
                ++j;
            }
            if(!opt_chrom){
                throw std::runtime_error("could not create a chromosome in the magnitude range min: " + std::to_string(min_magnitude) + " and max: " + std::to_string(max_magnitude));
            }
            else{
                chrom = *opt_chrom;
            }
		};
        develop(parent1, false); // this will do nothing if the parent is part of the interesting magnitude range
        ret.push_back(parent1); // the winner parent should be part of the next gen to defend his title
        auto crossed_over = parent1;
		crossed_over.crossover(parent2);
		develop(crossed_over, false); 
		for(size_t i = 0; i < num_children - 1; ++i){
			auto chrom = crossed_over;
			develop(chrom, true);
			ret.push_back(chrom);
		}
		return ret;
	}
};

// -----------------------------------------------------------------------------------------------
// Partial Evolution challenge

// This library encapsulates evolutional learning through genetic algorithms 
// The library is header only and easy to use. 
// You have a type (the partial chromosome) whose values you want to optimize through evolutional learning
// and you have a partial challenge your type needs to master
// the difference from partial chromosomes to chromosomes is that only partial chromsomes that are in a defined magnitude range are considered
// this enables parallelization of the evolution on multiple threads or processes and ensuring that no redundant work is done
// 

// These functions must be added to your chromosome type (the one you want to optimize)
//struct PartialChromosome {
//	void crossover(Chromosome const& other); make sure to randomly choose what data members should be taken from this or other
//	void mutate(); make sure to not randomly mutate too much of your chromosome type (only one data member at a time is recommended)
//  std::string toString() const; provide some output for the shape of the chromosome
//  double magnitude() const; the square root of the sum of the squares of all members
//};

// These functions must be added to your challenge type
//struct PartialChallenge {
//	double score(PatialChromosome const& chromosome); the chromosome faces the challenge and its performance needs to be evaluated with a double (0 means bad, the higher the better the performance)
//};

struct PartialEvolutionOptions{
	size_t num_generations = 100; // the number of generations to cross
	size_t log_level = 1;  // logging level to see how far the algorithm progressed
	std::ostream* out; // the ostream to stream the logging to
	double min_magnitude = 0.0;
	double max_magnitude = 1.0;
};

template<PartialChromosome Chrom, class Chall> 
requires PartialChallenge<Chall, Chrom>
std::vector<Chrom>
evolution(
	Chall const& challenge, // the challenge 
	double& winningAccuracy, // the winning performance is an out parameter
	PartialEvolutionOptions const& options // the evolution options
)
{
	size_t num_children = 20;
	auto candidates = challenge.grow_generation(Chrom{}, Chrom{}, num_children, options.min_magnitude, options.max_magnitude);
	for (size_t i = 0; i < options.num_generations; ++i) {
		// let the chromosomes face the challenge
		std::multimap<double, const Chrom*> fitness = fitnessCalculation(candidates, challenge);
		// logging
		if (options.log_level >= 1) {
			if(options.out) *options.out << "generation nr. " << i + 1 << " / " << options.num_generations << '\n';
			if (options.log_level >= 2) {
				for (auto& f : fitness)
					if(options.out) *options.out << "  fitness: " << f.first << '\n' << f.second->toString() << '\n';
				if(options.out) *options.out << '\n';
			}
		}
		// half of the chromosomes are winners
		std::vector<const Chrom*> winners = selectMatingPool(fitness, 2);
		// return the winner of the last generation
		if (i >= options.num_generations - 1)
		{
			std::vector<Chrom> ret;
			for (auto w : winners)
				ret.push_back(*w);
			winningAccuracy = fitness.rbegin()->first;
			return ret;
		}
		candidates = challenge.grow_generation(*winners[0], *winners[1], num_children, options.min_magnitude, options.max_magnitude);
	}
	return candidates;
}

} // namespace partial

} // namespace evol

