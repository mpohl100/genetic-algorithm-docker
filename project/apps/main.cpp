#include "Math.h"
#include "evol/Evolution.h"

#include <clara.hpp>

#include <iostream>
#include <vector>
 
int main(int argc, char** argv)
{
    using namespace clara;


    int number_generations = 100;
    bool help = false;
    auto cli = Opt(number_generations, "number_generations")["-n"]["--number-generations"]("The number of generations to calculate") | Help(help);
     

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
    std::vector<math::XCoordinate> initialGeneration;
    for(size_t i = 0; i < 20; ++i){
        initialGeneration.push_back(math::XCoordinate{0});
    }
    for(auto& xCoordinate : initialGeneration){
        xCoordinate.mutate();
    }
    double winningFitness = 0.0;
    const auto winningXCoordinates = evol::evolution(initialGeneration, math::MathFunction{}, winningFitness, evolParams);

    std::cout << '\n';
    std::cout << "winning x: " << winningXCoordinates[0].x() <<"; winning f(x): " << winningFitness << '\n';
    return 0;
}

