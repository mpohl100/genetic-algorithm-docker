#pragma once

#include "EvolutionConcepts.h"
#include "EvolutionImpl.h"
#include <iostream>
#include <algorithm>

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

namespace evol::partial {

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

}
