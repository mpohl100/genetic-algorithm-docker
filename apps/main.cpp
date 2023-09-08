#include "Math.h"

#include <clara.hpp>

#include <chrono>
#include <iostream>
#include <vector>
 
int main(int argc, char** argv)
{
    using namespace clara;


    int number_generations = 100;
    unsigned int random_seed = 0;
    bool help = false;
    auto cli = Opt(number_generations, "number_generations")["-n"]["--number-generations"]("The number of generations to calculate") 
    | Opt(random_seed, "random_seed")["-r"]["--rand"]("The random seed of the evolution algorithm, a positive integer") 
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

    auto evolParams = evol::EvolutionOptions{};
    evolParams.num_generations = number_generations;
    evolParams.out = &std::cout;

    if(random_seed == 0){
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime.time_since_epoch()).count();
        random_seed = static_cast<unsigned int>(timestamp);
    }
    auto rng = evol::Rng{random_seed};

    double winningFitness = 0.0;
    const auto winningXCoordinates = evol::evolution<math::XCoordinate>(math::MathFunction{}, winningFitness, evolParams, rng);

    std::cout << '\n';
    std::cout << "winning x: " << winningXCoordinates[0].x() <<"; winning f(x): " << winningFitness << '\n';
    std::cout << "random seed used: " << random_seed << ". Pass this seed with -r to get the same results with a rerun.\n";
    return 0;
}

