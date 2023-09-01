#include "Math.h"

#include <clara.hpp>

#include <iostream>
#include <vector>
 
int main(int argc, char** argv)
{
    using namespace clara;


    int number_generations = 100;
    double min_magnitude = 0.0;
    double max_magnitude = 1.0;
    bool help = false;
    auto cli = Opt(number_generations, "number_generations")["-n"]["--number-generations"]("The number of generations to calculate") 
    | Opt(min_magnitude, "min_magnitude")["-b"]["--min"]("The min magnitude of the x choordinates to examine")
    | Opt(max_magnitude, "max_magnitude")["-t"]["--max"]("The max magnitude of the x choordinates to examine")
    | Help(help);
     

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
    evolParams.out = &std::cout;
    evolParams.min_magnitude = min_magnitude;
    evolParams.max_magnitude = max_magnitude;
    double winningFitness = 0.0;
    const auto winningXCoordinates = evol::partial::evolution<math::XCoordinate>(math::MathFunction{}, winningFitness, evolParams);

    std::cout << '\n';
    std::cout << "winning x: " << winningXCoordinates[0].x() <<"; winning f(x): " << winningFitness << '\n';
    return 0;
}

