#include "PartialEvolutionConcepts.h"

#include <functional>
#include <vector>
#include <stdexcept>
#include <optional>

namespace evol::partial{
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

}