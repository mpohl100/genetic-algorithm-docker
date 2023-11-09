#include "examples/math/Math.h"

#include <clara.hpp>

#include <chrono>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  using namespace clara;

  int number_generations = 100;
  size_t log_level = 0;
  double min_magnitude = 0.0;
  double max_magnitude = 1.0;
  size_t random_seed = 0;
  double starting_value = 0.0;
  size_t num_parents = 2;
  size_t num_children = 20;
  bool help = false;
  auto cli =
      Opt(number_generations,
          "number_generations")["-n"]["--number-generations"](
          "The number of generations to calculate") |
      Opt(min_magnitude, "min_magnitude")["-b"]["--min"](
          "The min magnitude of the x choordinates to examine") |
      Opt(max_magnitude, "max_magnitude")["-t"]["--max"](
          "The max magnitude of the x choordinates to examine") |
      Opt(random_seed, "random_seed")["-r"]["--rand"](
          "The random seed of the evolution algorithm, a positive integer") |
      Opt(log_level, "log_level")["-l"]["--log"](
          "The level of detail of the output. The higher the integer the more "
          "detailed the output.") |
      Opt(starting_value, "starting_value")["-s"]["--start"](
          "The starting value for the x coordinate.") |
      Opt(num_parents, "num_parents")["-p"]["--num-parents"](
          "The number of parents per generation.") |
      Opt(num_children, "num_children")["-c"]["--num-children"](
          "The number of children per generation.") |
      Help(help);

  auto result = cli.parse(Args(argc, argv));
  if (!result) {
    std::cerr << "Error in command line: " << result.errorMessage() << '\n';
    exit(1);
  }
  if (help) {
    std::cout << cli;
    exit(0);
  }

  auto evolParams = evol::partial::PartialEvolutionOptions{};
  evolParams.num_generations = number_generations;
  evolParams.log_level = log_level;
  evolParams.num_parents = num_parents;
  evolParams.num_children = num_children;
  evolParams.out = &std::cout;
  evolParams.min_magnitude = min_magnitude;
  evolParams.max_magnitude = max_magnitude;

  if (random_seed == 0) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                         currentTime.time_since_epoch())
                         .count();
    random_seed = static_cast<size_t>(timestamp);
  }
  auto rng = evol::Rng{random_seed};
  auto starting_chrom = math::XCoordinate{starting_value};
  const auto evolResult = evol::partial::evolution(
      starting_chrom, math::MathFunctionPartial{}, evolParams, rng);

  std::cout << '\n';
  std::cout << "winning x: " << evolResult.winner.x()
            << "; winning f(x): " << evolResult.fitness << '\n';
  std::cout
      << "random seed used: " << random_seed
      << ". Pass this seed with -r to get the same results with a rerun.\n";
  return 0;
}
