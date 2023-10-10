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
    Rng(unsigned int random_seed)
        : rd_("hw")
        , gen_()
    {
		gen_ = std::mt19937{random_seed};
	} 

    std::stack<int> fetchUniform(int from, int to, size_t num)
    {
        double from_d = double(from);
        double to_d = double(to) - std::numeric_limits<double>::min();
        std::uniform_real_distribution<double> dist{from_d, to_d};
        std::stack<int> ret;
        for(size_t i = 0; i < num; ++i)
            ret.push(int(dist(gen_)));
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

template<class T, class RNG>
concept Chromosome = std::semiregular<T> && requires (T t, RNG& rng)
{
    t.crossover(t);
    t.mutate(rng);
} && requires (const T t){
    {t.toString()} -> std::same_as<std::string>;
};

template<class T, class C, class RNG>
concept Challenge = std::semiregular<T> &&  Chromosome<C, RNG> && requires (T const t, C c, RNG& rng)
{
    {t.score(c, rng)} -> std::same_as<double>;
	{t.breed(std::vector<C>{}, rng, 20)} ->std::same_as<std::vector<C>>;
}; 

// -----------------------------------------------------------------------------------------------
// Partial Evolution concepts 
namespace partial{

template<class T, class RNG>
concept PartialChromosome = evol::Chromosome<T, RNG> && requires (const T t)
{
    {t.magnitude()} -> std::same_as<double>; // the square root of the sum of the squares of all members 
};

template<class T, class C, class RNG>
concept PartialChallenge = std::semiregular<T> &&  PartialChromosome<C, RNG> && requires (T const t, C c, RNG& rng)
{
    {t.score(c, rng)} -> std::same_as<double>;
    {t.breed(std::vector<C>{}, rng, 20 /*the number of partial chromosomes in the next generation*/, 0.0 /*the minimum magnitude of the partial chromosomes*/, 1.0 /*the maximum magnitude of the partial chromosomes*/ )} -> std::same_as<std::vector<C>>;
}; 

}

// -----------------------------------------------------------------------------------------------
// Evolution Impl

template<class Chrom, class Chall, class RNG>
requires Chromosome<Chrom, RNG> && (Challenge<Chall, Chrom, RNG> || partial::PartialChallenge<Chall, Chrom, RNG>)
std::multimap<double, const Chrom*>
fitnessCalculation(std::vector<Chrom> const& candidates, Chall const& challenge, RNG& rng)
{
	std::multimap<double, const Chrom*> ret;
	for (auto& cand : candidates)
		ret.insert(std::make_pair(challenge.score(cand, rng), &cand));
	return ret;
}

template<class Chrom, class RNG>
requires Chromosome<Chrom, RNG>
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

// -----------------------------------------------------------------------------------------------
// Evolution challenge

// inherit from this type in order to use the default implementation of grow generation
template<class Chrom, class RNG>
requires Chromosome<Chrom, RNG>
struct DefaultChallenge{
	std::vector<Chrom> breed(std::vector<Chrom> parents, RNG& rng, size_t num_children /*the number of partial chromosomes in the next generation*/ ) const
	{
		if(parents.empty()){
			throw std::runtime_error("no parents passed to breed");
			return {};
		}
		std::vector<Chrom> ret;
        ret.push_back(parents[0]); // the winner parent should be part of the next gen to defend his title
		for(size_t i = 1; i < parents.size(); ++i){
			auto chrom = parents[0]; // the winner parent gets to spread their genes everywhere
			chrom.crossover(parents[i]);  // the runner-ups are the cross over partners
			chrom.mutate(rng);
			ret.push_back(chrom);
		}
		for(size_t i = parents.size(); i < num_children; ++i){
			auto chrom = parents[0]; // the rest of the generation are the winning parent with a random mutation
			chrom.mutate(rng);
			ret.push_back(chrom);
		}
		return ret;
	}
};


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
	size_t num_parents = 2; // the number of parents to grow a new generation
	std::ostream* out; // the ostream to stream the logging to
};

template<class Chrom, class Chall, class RNG> 
requires Chromosome<Chrom, RNG> && Challenge<Chall, Chrom, RNG>
std::vector<Chrom>
evolution(
	const Chrom& starting_value, // the starting value
	const Chall& challenge, // the challenge 
	double& winningAccuracy, // the winning performance is an out parameter
	const EvolutionOptions& options, // the evolution options
	RNG& rng // the random number generator
)
{
	size_t num_children = 20;
	auto candidates = challenge.breed({starting_value}, rng, num_children);

	for (size_t i = 0; i < options.num_generations; ++i) {
		// let the chromosomes face the challenge
		std::multimap<double, const Chrom*> fitness = fitnessCalculation(candidates, challenge, rng);
		// logging
		if (options.log_level >= 1) {
			if(options.out) *options.out << "generation nr. " << i + 1 << " / " << options.num_generations << '\n';
			if (options.log_level >= 2) {
				for (auto it = fitness.rbegin(); it != fitness.rend(); ++it){
					const auto& f = *it;
					if(options.out) *options.out << "  fitness: " << f.first << ": " << f.second->toString() << '\n';
				}
				if(options.out) *options.out << '\n';
			}
		}
		// half of the chromosomes are winners
		std::vector<const Chrom*> winners = selectMatingPool<Chrom, RNG>(fitness, 2);
		// return the winner of the last generation
		if (i >= options.num_generations - 1)
		{
			std::vector<Chrom> ret;
			for (auto w : winners)
				ret.push_back(*w);
			winningAccuracy = fitness.rbegin()->first;
			return ret;
		}
		std::vector<Chrom> parents;
		size_t j = 0;
		for (auto* winner : winners){
			parents.push_back(*winner);
			if(++j >= options.num_parents){
				break;
			}
		}
		candidates = challenge.breed(parents, rng, num_children);
	}
	return candidates;
}

namespace partial {

// -----------------------------------------------------------------------------------------------
// Partial Evolution challenge

// inherit from this type in order to use the default implementation of grow generation
template<class Chrom, class RNG>
requires PartialChromosome<Chrom, RNG>
struct DefaultPartialChallenge{
	std::vector<Chrom> breed(std::vector<Chrom> parents, RNG& rng, size_t num_children /*the number of partial chromosomes in the next generation*/, double min_magnitude /*the minimum magnitude of the partial chromosomes*/, double max_magnitude /*the maximum magnitude of the partial chromosomes*/ ) const
	{
		if(parents.empty()){
			throw std::runtime_error("no parents passed to grow generation partial.");
			return {};
		}
		std::vector<Chrom> ret;
		const auto develop = [min_magnitude, max_magnitude, &rng](Chrom& chrom, bool initialMutate){
			if(initialMutate){
				chrom.mutate(rng);
			}
            const auto try_n_times = [min_magnitude, max_magnitude, &rng](Chrom chrom, size_t num) -> std::optional<Chrom>
            {
                size_t i = 0;
			    while(min_magnitude > chrom.magnitude() || chrom.magnitude() > max_magnitude){
				    chrom.mutate(rng);
				    i++;
                    if(i >= num){
                        break;
                    }
			    }
                if(min_magnitude <= chrom.magnitude() && chrom.magnitude() <= max_magnitude){
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
        develop(parents[0], false); // this will do nothing if the parent is part of the interesting magnitude range
        ret.push_back(parents[0]); // the winner parent should be part of the next gen to defend his title
		for(size_t i = 1; i < parents.size(); ++i){
			auto chrom = parents[0]; // the winning parent gets to spread his genes everywhere
			chrom.crossover(parents[i]); // each of the runner-ups is a cross over partner
			develop(chrom, true);
			ret.push_back(chrom);
		}
		for(size_t i = parents.size(); i < num_children; ++i){
			auto chrom = parents[0]; // the rest of the generation is the winning parent with a random mutation
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

struct PartialEvolutionOptions : public EvolutionOptions{
	double min_magnitude = 0.0;
	double max_magnitude = 1.0;
};

template<class Chrom, class Chall, class RNG>
requires PartialChromosome<Chrom, RNG> && PartialChallenge<Chall, Chrom, RNG>
std::vector<Chrom>
evolution(
	const Chrom& starting_value, // the starting value
	const Chall& challenge, // the challenge 
	double& winningAccuracy, // the winning performance is an out parameter
	const PartialEvolutionOptions& options, // the evolution options
	RNG& rng // the random number generator
)
{
	size_t num_children = 20;
	auto candidates = challenge.breed({starting_value}, rng, num_children, options.min_magnitude, options.max_magnitude);
	for (size_t i = 0; i < options.num_generations; ++i) {
		// let the chromosomes face the challenge
		std::multimap<double, const Chrom*> fitness = fitnessCalculation(candidates, challenge, rng);
		// logging
		if (options.log_level >= 1) {
			if(options.out) *options.out << "generation nr. " << i + 1 << " / " << options.num_generations << '\n';
			if (options.log_level >= 2) {
				for (auto it = fitness.rbegin(); it != fitness.rend(); ++it){
					const auto& f = *it;
					if(options.out) *options.out << "  fitness: " << f.first << ": " << f.second->toString() << '\n';
				}
				if(options.out) *options.out << '\n';
			}
		}
		// half of the chromosomes are winners
		std::vector<const Chrom*> winners = selectMatingPool<Chrom, RNG>(fitness, 2);
		// return the winner of the last generation
		if (i >= options.num_generations - 1)
		{
			std::vector<Chrom> ret;
			for (auto w : winners)
				ret.push_back(*w);
			winningAccuracy = fitness.rbegin()->first;
			return ret;
		}
		std::vector<Chrom> parents;
		size_t j = 0;
		for (auto* winner : winners){
			parents.push_back(*winner);
			if(++j >= options.num_parents){
				break;
			}
		}
		candidates = challenge.breed(parents, rng, num_children, options.min_magnitude, options.max_magnitude);
	}
	return candidates;
}

} // namespace partial

} // namespace evol

