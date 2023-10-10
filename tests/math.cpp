#include <catch2/catch_all.hpp>

#include "Math.h"

#include <cmath>
#include <limits>

namespace {

struct EvolutionResult{
    double xCoordinate = std::numeric_limits<double>::quiet_NaN();
    double fitness = std::numeric_limits<double>::quiet_NaN();
};

EvolutionResult calculateEvolution(math::XCoordinate xCoordinate, unsigned int random_seed, int num_parents)
{
    auto evolParams = evol::EvolutionOptions{};
    evolParams.num_generations = 100;
    evolParams.log_level = 0;
    evolParams.num_parents = num_parents;
    evolParams.out = &std::cout;

    auto rng = evol::Rng{random_seed};

    double winningFitness = std::numeric_limits<double>::quiet_NaN();
    auto starting_chrom = xCoordinate;
    const auto winningXCoordinates = evol::evolution(starting_chrom, math::MathFunction{}, winningFitness, evolParams, rng);
    return EvolutionResult{winningXCoordinates[0].x(), winningFitness};
}

EvolutionResult calculatePartialEvolution(math::XCoordinate xCoordinate, unsigned int random_seed, int num_parents, std::pair<double, double> minMax)
{
    auto evolParams = evol::partial::PartialEvolutionOptions{};
    evolParams.num_generations = 100;
    evolParams.log_level = 0;
    evolParams.num_parents = num_parents;
    evolParams.out = &std::cout;
    evolParams.min_magnitude = minMax.first;
    evolParams.max_magnitude = minMax.second;

    auto rng = evol::Rng{random_seed};

    double winningFitness = std::numeric_limits<double>::quiet_NaN();
    auto starting_chrom = xCoordinate;
    const auto winningXCoordinates = evol::partial::evolution(starting_chrom, math::MathFunctionPartial{}, winningFitness, evolParams, rng);
    return EvolutionResult{winningXCoordinates[0].x(), winningFitness};
}

TEST_CASE("Evol", "[evol]"){
    SECTION("Evolution"){
        const auto starting_values = std::vector<double>{0.0, 10.0, 20.0};
        const auto num_parents = std::vector<int>{1,2,3};
        const auto random_seeds = std::vector<unsigned int>{0, 1, 2, 3, 4};
        for(const auto random_seed : random_seeds){
            for(const auto num_parent : num_parents){
                for(const auto starting_value : starting_values){
                    const auto evolutionResult = calculateEvolution(math::XCoordinate{starting_value}, random_seed, num_parent);
                    CHECK(std::abs(evolutionResult.xCoordinate - 2.0) < 1e-10);
                    CHECK(evolutionResult.fitness > 1e10);
                }
            }
        }
    }
    SECTION("PartialEvolution"){
        const auto num_parents = std::vector<int>{1,2,3};
        const auto random_seeds = std::vector<unsigned int>{0, 1, 2, 3, 4};
        const auto minMaxs = std::vector<std::pair<double, double>>{{2.0, 5.0}, {3.0, 6.0}, {4.0, 7.0}};
        for(const auto random_seed : random_seeds){
            for(const auto num_parent : num_parents){
                std::vector<EvolutionResult> results;
                for(const auto& minMax : minMaxs){
                    const auto evolutionResult = calculatePartialEvolution(math::XCoordinate{minMax.second}, random_seed, num_parent, minMax);
                    CHECK(std::abs(evolutionResult.xCoordinate - minMax.first) <= 0.1);
                    results.push_back(evolutionResult);
                }
                CHECK(std::is_sorted(results.begin(), results.end(), [](const auto& l, const auto& r){ return l.xCoordinate < r.xCoordinate; }));
                CHECK(std::is_sorted(results.begin(), results.end(), [](const auto& l, const auto& r){ return l.fitness > r.fitness; }));
            }
        }
    }
}

}