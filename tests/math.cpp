#include <catch2/catch_all.hpp>

#include "examples/math/Math.h"

#include <cmath>

namespace {

auto calculateEvolution(math::XCoordinate xCoordinate, size_t random_seed,
                        int num_parents) {
  auto evolParams = evol::EvolutionOptions{};
  evolParams.num_generations = 100;
  evolParams.log_level = 0;
  evolParams.num_parents = num_parents;
  evolParams.out = &std::cout;

  auto rng = evol::Rng{random_seed};

  auto starting_chrom = xCoordinate;
  return evol::evolution(starting_chrom, math::MathFunction{}, evolParams, rng);
}

auto calculatePartialEvolution(math::XCoordinate xCoordinate,
                               size_t random_seed, int num_parents,
                               std::pair<double, double> minMax) {
  auto evolParams = evol::partial::PartialEvolutionOptions{};
  evolParams.num_generations = 1000;
  evolParams.log_level = 0;
  evolParams.num_parents = num_parents;
  evolParams.out = &std::cout;
  evolParams.min_magnitude = minMax.first;
  evolParams.max_magnitude = minMax.second;

  auto rng = evol::Rng{random_seed};

  auto starting_chrom = xCoordinate;
  return evol::partial::evolution(starting_chrom, math::MathFunctionPartial{},
                                  evolParams, rng);
}

auto calculateAdjustEvolution(math::XCoordinate xCoordinate, size_t random_seed,
                              int num_parents) {
  auto evolParams = evol::adjust::AdjustEvolutionOptions{};
  evolParams.num_generations = 100;
  evolParams.log_level = 0;
  evolParams.num_parents = num_parents;
  evolParams.out = &std::cout;
  evolParams.max_num_mutations = 10000;

  auto rng = evol::Rng{random_seed};

  auto starting_chrom = xCoordinate;
  return evol::adjust::evolution(starting_chrom, math::MathFunctionAdjust{}, evolParams, rng);
}


TEST_CASE("Evol", "[evol]") {
  SECTION("Evolution") {
    const auto starting_values = std::vector<double>{0.0, 10.0, 20.0};
    const auto num_parents = std::vector<int>{1, 2, 3};
    const auto random_seeds = std::vector<size_t>{0, 1, 2, 3, 4};
    for (const auto random_seed : random_seeds) {
      for (const auto num_parent : num_parents) {
        for (const auto starting_value : starting_values) {
          const auto evolutionResult = calculateEvolution(
              math::XCoordinate{starting_value}, random_seed, num_parent);
          CHECK(std::abs(evolutionResult.winner.x() - 2.0) <= 0.05);
          CHECK(evolutionResult.fitness >= 400);
        }
      }
    }
  }
  SECTION("PartialEvolution") {
    const auto num_parents = std::vector<int>{1, 2, 3};
    const auto random_seeds = std::vector<size_t>{0, 1, 2, 3, 4};
    const auto minMaxs = std::vector<std::pair<double, double>>{
        {2.0, 1000000}, {3.0, 1000000}, {4.0, 1000000}};
    for (const auto random_seed : random_seeds) {
      for (const auto num_parent : num_parents) {
        std::vector<evol::EvolutionResult<math::XCoordinate>> results;
        for (const auto &minMax : minMaxs) {
          const auto evolutionResult =
              calculatePartialEvolution(math::XCoordinate{minMax.second},
                                        random_seed, num_parent, minMax);
          CHECK(std::abs(evolutionResult.winner.x() - minMax.first) <= 0.1);
          results.push_back(evolutionResult);
        }
        CHECK(std::is_sorted(results.begin(), results.end(),
                             [](const auto &l, const auto &r) {
                               return l.winner.x() < r.winner.x();
                             }));
        CHECK(std::is_sorted(results.begin(), results.end(),
                             [](const auto &l, const auto &r) {
                               return l.fitness > r.fitness;
                             }));
      }
    }
  }
  SECTION("AdjustEvolution") {
    const auto starting_values = std::vector<double>{0.0, 1e6, 2e6};
    const auto num_parents = std::vector<int>{1, 2, 3};
    const auto random_seeds = std::vector<size_t>{0, 1, 2, 3, 4};
    for (const auto random_seed : random_seeds) {
      for (const auto num_parent : num_parents) {
        for (const auto starting_value : starting_values) {
          const auto evolutionResult = calculateAdjustEvolution(
              math::XCoordinate{starting_value}, random_seed, num_parent);
          CHECK(std::abs(evolutionResult.winner.x() - 2.0) <= 0.05);
          CHECK(evolutionResult.fitness >= 400);
        }
      }
    }
  }
}

} // namespace