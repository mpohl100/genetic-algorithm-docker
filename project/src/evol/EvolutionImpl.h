#pragma once

#include "EvolutionConcepts.h"

#include <map>
#include <vector>

namespace evol {

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

}