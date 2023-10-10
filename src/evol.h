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
concept Phenotype = std::semiregular<T> && requires (T t, RNG& rng)
{
    t.crossover(t);
    t.mutate(rng);
} && requires (const T t){
    {t.toString()} -> std::same_as<std::string>;
};

template<class T, class C, class RNG>
concept Challenge = std::semiregular<T> &&  Phenotype<C, RNG> && requires (T const t, C c, RNG& rng)
{
    {t.score(c, rng)} -> std::same_as<double>;
	{t.breed(std::vector<C>{}, rng, 20)} ->std::same_as<std::vector<C>>;
}; 

// -----------------------------------------------------------------------------------------------
// Partial Evolution concepts 
namespace partial{

template<class T, class RNG>
concept PartialPhenotype = evol::Phenotype<T, RNG> && requires (const T t)
{
    {t.magnitude()} -> std::same_as<double>; // the square root of the sum of the squares of all members 
};

template<class T, class C, class RNG>
concept PartialChallenge = std::semiregular<T> &&  PartialPhenotype<C, RNG> && requires (T const t, C c, RNG& rng)
{
    {t.score(c, rng)} -> std::same_as<double>;
    {t.breed(std::vector<C>{}, rng, 20 /*the number of partial Phenotypes in the next generation*/, 0.0 /*the minimum magnitude of the partial Phenotypes*/, 1.0 /*the maximum magnitude of the partial Phenotypes*/ )} -> std::same_as<std::vector<C>>;
}; 

}

// -----------------------------------------------------------------------------------------------
// Evolution Impl

namespace detail{

template<class Pheno, class Chall, class RNG>
requires Phenotype<Pheno, RNG> && (Challenge<Chall, Pheno, RNG> || partial::PartialChallenge<Chall, Pheno, RNG>)
std::multimap<double, const Pheno*>
fitnessCalculation(std::vector<Pheno> const& candidates, Chall const& challenge, RNG& rng)
{
	std::multimap<double, const Pheno*> ret;
	for (auto& cand : candidates)
		ret.insert(std::make_pair(challenge.score(cand, rng), &cand));
	return ret;
}

template<class Pheno, class RNG>
requires Phenotype<Pheno, RNG>
std::vector<const Pheno*>
selectMatingPool(std::multimap<double, const Pheno*> const& fitness, int sep = 2)
{
	std::vector<const Pheno*> ret;
	int i = 0;
	for (auto it = fitness.rbegin(); it != fitness.rend(); ++it)
	{
		ret.push_back(it->second);
		if (++i > int(fitness.size()) / sep - 1)
			break;
	}
	return ret;
}

template<class Pheno, class Chall, class RNG, class EvolutionOpts> 
std::vector<Pheno>
evolution_impl(
	const Pheno& starting_value, // the starting value
	const Chall& challenge, // the challenge 
	double& winningAccuracy, // the winning performance is an out parameter
	const EvolutionOpts& options, // the evolution options
	RNG& rng // the random number generator
)
{
	size_t num_children = 20;
	std::vector<Pheno> candidates;
	if constexpr(evol::partial::PartialChallenge<Chall, Pheno, RNG>){
		candidates = challenge.breed({starting_value}, rng, num_children, options.min_magnitude, options.max_magnitude);
	}
	else if constexpr(evol::Challenge<Chall, Pheno, RNG>){
		candidates = challenge.breed({starting_value}, rng, num_children);
	}

	for (size_t i = 0; i < options.num_generations; ++i) {
		// let the Phenotypes face the challenge
		std::multimap<double, const Pheno*> fitness = fitnessCalculation(candidates, challenge, rng);
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
		// half of the Phenotypes are winners
		std::vector<const Pheno*> winners = selectMatingPool<Pheno, RNG>(fitness, 2);
		// return the winner of the last generation
		if (i >= options.num_generations - 1)
		{
			std::vector<Pheno> ret;
			for (auto w : winners)
				ret.push_back(*w);
			winningAccuracy = fitness.rbegin()->first;
			return ret;
		}
		std::vector<Pheno> parents;
		size_t j = 0;
		for (auto* winner : winners){
			parents.push_back(*winner);
			if(++j >= options.num_parents){
				break;
			}
		}

		if constexpr(evol::partial::PartialChallenge<Chall, Pheno, RNG>){
			candidates = challenge.breed(parents, rng, num_children, options.min_magnitude, options.max_magnitude);
		}
		else if constexpr(evol::Challenge<Chall, Pheno, RNG>){
			candidates = challenge.breed(parents, rng, num_children);
		}
	}
	return candidates;
}

template<class Pheno, class RNG>
std::vector<Pheno> breed(std::vector<Pheno> parents, RNG& rng, size_t num_children /*the number of partial Phenotypes in the next generation*/, std::function<void(Pheno&, RNG&, bool)> develop)
{
	if(parents.empty()){
		throw std::runtime_error("no parents passed to breed");
		return {};
	}
	std::vector<Pheno> ret;
	develop(parents[0], rng, false);
	ret.push_back(parents[0]); // the winner parent should be part of the next gen to defend his title
	for(size_t i = 1; i < parents.size(); ++i){
		auto pheno = parents[0]; // the winner parent gets to spread their genes everywhere
		pheno.crossover(parents[i]);  // the runner-ups are the cross over partners
		develop(pheno, rng, true);
		ret.push_back(pheno);
	}
	for(size_t i = parents.size(); i < num_children; ++i){
		auto pheno = parents[0]; // the rest of the generation are the winning parent with a random mutation
		develop(pheno, rng, true);
		ret.push_back(pheno);
	}
	return ret;
}

}
// -----------------------------------------------------------------------------------------------
// Evolution challenge

// inherit from this type in order to use the default implementation of grow generation
template<class Pheno, class RNG>
requires Phenotype<Pheno, RNG>
struct DefaultChallenge{
	std::vector<Pheno> breed(std::vector<Pheno> parents, RNG& rng, size_t num_children /*the number of partial Phenotypes in the next generation*/ ) const
	{
		return detail::breed<Pheno, RNG>(parents, rng, num_children, [](Pheno& pheno, RNG& rng, bool doMutate){ if(doMutate) pheno.mutate(rng);});
	}
};


// -----------------------------------------------------------------------------------------------
// Evolution 

// This library encapsulates evolutional learning through genetic algorithms 
// The library is header only and easy to use. 
// You have a type (the Phenotype) whose values you want to optimize through evolutional learning
// and you have a challenge your type needs to master
// 

// These functions must be added to your Phenotype type (the one you want to optimize)
//struct Phenotype {
//	void crossover(Phenotype const& other); make sure to randomly choose what data members should be taken from this or other
//	void mutate(); make sure to not randomly mutate too much of your Phenotype type (only one data member at a time is recommended)
// std::string toString() const; provide some output for the shape of the Phenotype
//};

// These functions must be added to your challenge type
//struct Challenge {
//	double score(Phenotype const& Phenotype); the Phenotype faces the challenge and its performance needs to be evaluated with a double (0 means bad, the higher the better the performance)
//};

struct EvolutionOptions{
	size_t num_generations; // the number of generations to cross
	size_t log_level = 1;  // logging level to see how far the algorithm progressed
	size_t num_parents = 2; // the number of parents to grow a new generation
	std::ostream* out; // the ostream to stream the logging to
};

template<class Pheno, class Chall, class RNG> 
requires Phenotype<Pheno, RNG> && Challenge<Chall, Pheno, RNG>
std::vector<Pheno>
evolution(
	const Pheno& starting_value, // the starting value
	const Chall& challenge, // the challenge 
	double& winningAccuracy, // the winning performance is an out parameter
	const EvolutionOptions& options, // the evolution options
	RNG& rng // the random number generator
)
{
	return detail::evolution_impl(starting_value, challenge, winningAccuracy, options, rng);
}

namespace partial {

// -----------------------------------------------------------------------------------------------
// Partial Evolution challenge

// inherit from this type in order to use the default implementation of grow generation
template<class Pheno, class RNG>
requires PartialPhenotype<Pheno, RNG>
struct DefaultPartialChallenge{
	std::vector<Pheno> breed(std::vector<Pheno> parents, RNG& rng, size_t num_children /*the number of partial Phenotypes in the next generation*/, double min_magnitude /*the minimum magnitude of the partial Phenotypes*/, double max_magnitude /*the maximum magnitude of the partial Phenotypes*/ ) const
	{
		if(parents.empty()){
			throw std::runtime_error("no parents passed to grow generation partial.");
			return {};
		}
		std::vector<Pheno> ret;
		const auto develop = [min_magnitude, max_magnitude](Pheno& pheno, RNG& rng, bool initialMutate){
			if(initialMutate){
				pheno.mutate(rng);
			}
            const auto try_n_times = [min_magnitude, max_magnitude, &rng](Pheno pheno, size_t num) -> std::optional<Pheno>
            {
                size_t i = 0;
			    while(min_magnitude > pheno.magnitude() || pheno.magnitude() > max_magnitude){
				    pheno.mutate(rng);
				    i++;
                    if(i >= num){
                        break;
                    }
			    }
                if(min_magnitude <= pheno.magnitude() && pheno.magnitude() <= max_magnitude){
                    return pheno;
                }
                else return std::nullopt;
            };
            std::optional<Pheno> opt_pheno = std::nullopt;
            size_t j = 0;
            while(opt_pheno == std::nullopt && j <= 1000){
                opt_pheno = try_n_times(pheno, 1000);
                ++j;
            }
            if(!opt_pheno){
                throw std::runtime_error("could not create a Phenotype in the magnitude range min: " + std::to_string(min_magnitude) + " and max: " + std::to_string(max_magnitude));
            }
            else{
                pheno = *opt_pheno;
            }
		};
		return detail::breed<Pheno, RNG>(parents, rng, num_children, develop);
	}
};

// -----------------------------------------------------------------------------------------------
// Partial Evolution challenge

// This library encapsulates evolutional learning through genetic algorithms 
// The library is header only and easy to use. 
// You have a type (the partial Phenotype) whose values you want to optimize through evolutional learning
// and you have a partial challenge your type needs to master
// the difference from partial Phenotypes to Phenotypes is that only partial Phenosomes that are in a defined magnitude range are considered
// this enables parallelization of the evolution on multiple threads or processes and ensuring that no redundant work is done
// 

// These functions must be added to your Phenotype type (the one you want to optimize)
//struct PartialPhenotype {
//	void crossover(Phenotype const& other); make sure to randomly choose what data members should be taken from this or other
//	void mutate(); make sure to not randomly mutate too much of your Phenotype type (only one data member at a time is recommended)
//  std::string toString() const; provide some output for the shape of the Phenotype
//  double magnitude() const; the square root of the sum of the squares of all members
//};

// These functions must be added to your challenge type
//struct PartialChallenge {
//	double score(PatialPhenotype const& Phenotype); the Phenotype faces the challenge and its performance needs to be evaluated with a double (0 means bad, the higher the better the performance)
//};

struct PartialEvolutionOptions : public EvolutionOptions{
	double min_magnitude = 0.0;
	double max_magnitude = 1.0;
};

template<class Pheno, class Chall, class RNG>
requires PartialPhenotype<Pheno, RNG> && PartialChallenge<Chall, Pheno, RNG>
std::vector<Pheno>
evolution(
	const Pheno& starting_value, // the starting value
	const Chall& challenge, // the challenge 
	double& winningAccuracy, // the winning performance is an out parameter
	const PartialEvolutionOptions& options, // the evolution options
	RNG& rng // the random number generator
)
{
	return detail::evolution_impl(starting_value, challenge, winningAccuracy, options, rng);
}

} // namespace partial

} // namespace evol

