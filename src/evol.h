/* Copyright (C) 2018-2023 Michael Pohl - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license, which unfortunately won't be
 * written for another century.
 */

#pragma once

#ifndef EVOL_USE_CONCEPTS
#define EVOL_USE_CONCEPTS 1
#endif

#include <algorithm>
#include <chrono>
#include <concepts>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <random>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace evol {

// -----------------------------------------------------------------------------------------------
// class Rng
class Rng {
public:
  Rng() : gen_() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                         currentTime.time_since_epoch())
                         .count();
    size_t random_seed = static_cast<size_t>(timestamp);
    gen_ = std::mt19937_64{random_seed};
  }

  Rng(size_t random_seed) : gen_() { gen_ = std::mt19937_64{random_seed}; }

  std::stack<int> fetchUniform(int from, int to, size_t num) {
    double from_d = double(from);
    double to_d = double(to) - std::numeric_limits<double>::min();
    std::uniform_real_distribution<double> dist{from_d, to_d};
    std::stack<int> ret;
    for (size_t i = 0; i < num; ++i)
      ret.push(int(dist(gen_)));
    return ret;
  }

  std::stack<double> fetchNormal(double expValue, double stdDev,
                                 size_t num) const {
    std::normal_distribution<double> dist{expValue, stdDev};
    std::stack<double> ret;
    for (size_t i = 0; i < num; ++i)
      ret.push(dist(gen_));
    return ret;
  }

private:
  mutable std::mt19937_64 gen_;
};

class EvolutionCoordinator {
public:
  template <class EvolOptions>
  EvolutionCoordinator(const EvolOptions &options)
      : nb_generations_(options.num_generations) {}

  EvolutionCoordinator(const EvolutionCoordinator &) = default;
  EvolutionCoordinator &operator=(const EvolutionCoordinator &) = default;
  EvolutionCoordinator(EvolutionCoordinator &&) = default;
  EvolutionCoordinator &operator=(EvolutionCoordinator &&) = default;

  void set_generation_nr(size_t num) { current_generation_ = num; }

  void set_fitness(double fitness) { fitness_.push_back(fitness); }

  double progress() const {
    return double(current_generation_) / double(nb_generations_);
  }

  double get_improvement_factor() const {
    if (fitness_.size() < 2) {
      return 1.0;
    }
    const auto fitness_improvement = fitness_.back() / *(++fitness_.rbegin());
    if (std::abs(fitness_improvement) < 1e-4) {
      return 1.0;
    }
    constexpr const auto normal_improvement = 0.01;
    // this yields a number which smaller than 1 if the fitness improvement is
    // higher than normal
    const auto improvement_factor = normal_improvement / fitness_improvement;
    return std::clamp(improvement_factor, 0.0, 1.0);
  }

private:
  size_t current_generation_ = 0;
  size_t nb_generations_;
  std::vector<double> fitness_;
};

struct EvolutionOptions {
  size_t num_generations; // the number of generations to cross
  size_t log_level = 1; // logging level to see how far the algorithm progressed
  size_t num_parents = 2;   // the number of parents to grow a new generation
  size_t num_children = 20; // the number of phenotypes to breed per generation
  std::ostream *out;        // the ostream to stream the logging to
};

namespace partial {

struct PartialEvolutionOptions : public EvolutionOptions {
  double min_magnitude = 0.0;
  double max_magnitude = 1.0;
};

} // namespace partial

namespace adjust {

struct AdjustEvolutionOptions : public EvolutionOptions {
  size_t max_num_mutations = 100;
};

} // namespace adjust

#if EVOL_USE_CONCEPTS == 1
// -----------------------------------------------------------------------------------------------
// Evolution concepts

template <class T, class RNG>
concept Phenotype = std::semiregular<T> &&
    requires(T t, RNG &rng, const EvolutionCoordinator &evolCoordinator) {
  t.crossover(t);
  t.mutate(rng, evolCoordinator);
} && requires(const T t) {
  { t.toString() } -> std::same_as<std::string>;
};

template <class T, class C, class RNG>
concept Challenge = std::semiregular<T> && Phenotype<C, RNG> &&
    requires(T const t, C c, RNG &rng,
             const EvolutionCoordinator &evolCoordinator,
             const EvolutionOptions &evolOpts) {
  { t.score(c, rng) } -> std::same_as<double>;
  {
    t.breed(std::vector<C>{}, rng, evolCoordinator, evolOpts)
    } -> std::same_as<std::vector<C>>;
};

// -----------------------------------------------------------------------------------------------
// Partial Evolution concepts
namespace partial {

template <class T, class RNG>
concept PartialPhenotype = evol::Phenotype<T, RNG> && requires(const T t) {
  {
    t.magnitude()
    } -> std::same_as<double>; // the square root of the sum of the squares of
                               // all members
};

template <class T, class C, class RNG>
concept PartialChallenge = std::semiregular<T> && PartialPhenotype<C, RNG> &&
    requires(T const t, C c, RNG &rng,
             const EvolutionCoordinator &evolCoordinator,
             const PartialEvolutionOptions &partialEvolOpts) {
  { t.score(c, rng) } -> std::same_as<double>;
  {
    t.breed(std::vector<C>{}, rng, evolCoordinator, partialEvolOpts)
    } -> std::same_as<std::vector<C>>;
};

} // namespace partial

namespace adjust {

template <class T, class C, class RNG>
concept AdjustChallenge = std::semiregular<T> && evol::partial::PartialPhenotype<C, RNG> &&
    requires(T const t, C c, RNG &rng,
             const EvolutionCoordinator &evolCoordinator,
             const AdjustEvolutionOptions &adjustEvolOpts) {
  { t.score(c, rng) } -> std::same_as<double>;
  {
    t.breed(std::vector<C>{}, rng, evolCoordinator, adjustEvolOpts)
    } -> std::same_as<std::vector<C>>;
};

}

#endif

// -----------------------------------------------------------------------------------------------
// Evolution Impl

template <class Pheno> struct EvolutionResult {
  Pheno winner = {};
  double fitness = std::numeric_limits<double>::quiet_NaN();
};

namespace detail {

template <class Pheno, class Chall, class RNG>
#if EVOL_USE_CONCEPTS == 1
requires Phenotype<Pheno, RNG> &&(Challenge<Chall, Pheno, RNG> ||
                                  partial::PartialChallenge<Chall, Pheno, RNG> ||
                                  adjust::AdjustChallenge<Chall, Pheno, RNG>)
#endif
    std::multimap<double, const Pheno *> fitnessCalculation(
        std::vector<Pheno> const &candidates, Chall const &challenge,
        RNG &rng) {
  std::multimap<double, const Pheno *> ret;
  for (auto &cand : candidates)
    ret.insert(std::make_pair(challenge.score(cand, rng), &cand));
  return ret;
}

template <class Pheno, class Chall, class RNG, class EvolutionOpts>
EvolutionResult<Pheno>
evolution_impl(const Pheno &starting_value,  // the starting value
               const Chall &challenge,       // the challenge
               const EvolutionOpts &options, // the evolution options
               RNG &rng                      // the random number generator
) {
  auto evolCoordinator = EvolutionCoordinator(options);
  std::vector<Pheno> candidates;
  std::multimap<double, const Pheno *> fitness;
  std::vector<Pheno> parents = {starting_value};
  for (size_t i = 0; i < options.num_generations; ++i) {
    evolCoordinator.set_generation_nr(i + 1);
    // breed the new generation
    candidates = challenge.breed(parents, rng, evolCoordinator, options);
    // let the Phenotypes face the challenge
    fitness = fitnessCalculation(candidates, challenge, rng);
    evolCoordinator.set_fitness(fitness.rbegin()->first);
    // logging
    if (options.log_level >= 1) {
      if (options.out)
        *options.out << "generation nr. " << i + 1 << " / "
                     << options.num_generations << '\n';
      if (options.log_level >= 2) {
        for (auto it = fitness.rbegin(); it != fitness.rend(); ++it) {
          const auto &f = *it;
          if (options.out)
            *options.out << "  fitness: " << f.first << ": "
                         << f.second->toString() << '\n';
        }
        if (options.out)
          *options.out << '\n';
      }
    }
    parents.clear();
    size_t j = 0;
    for (auto fitnessPhenoIt = fitness.rbegin();
         fitnessPhenoIt != fitness.rend(); ++fitnessPhenoIt) {
      parents.push_back(*fitnessPhenoIt->second);
      if (++j >= options.num_parents) {
        break;
      }
    }
  }
  EvolutionResult<Pheno> ret;
  ret.winner = *fitness.rbegin()->second;
  ret.fitness = fitness.rbegin()->first;
  return ret;
}

template <class Pheno, class RNG>
std::vector<Pheno>
breed(std::vector<Pheno> parents, RNG &rng,
      const EvolutionCoordinator &evolCoordinator,
      size_t num_children /*the number of partial Phenotypes in the next
                             generation*/
      ,
      std::function<void(Pheno &, RNG &, const EvolutionCoordinator &, bool)>
          develop) {
  if (parents.empty()) {
    throw std::runtime_error("no parents passed to breed");
    return {};
  }
  std::vector<Pheno> ret;
  develop(parents[0], rng, evolCoordinator, false);
  ret.push_back(parents[0]); // the winner parent should be part of the next gen
                             // to defend his title
  for (size_t i = 1; i < parents.size(); ++i) {
    auto pheno =
        parents[0]; // the winner parent gets to spread their genes everywhere
    pheno.crossover(parents[i]); // the runner-ups are the cross over partners
    develop(pheno, rng, evolCoordinator, true);
    ret.push_back(pheno);
  }
  for (size_t i = parents.size(); i < num_children; ++i) {
    auto pheno = parents[0]; // the rest of the generation are the winning
                             // parent with a random mutation
    develop(pheno, rng, evolCoordinator, true);
    ret.push_back(pheno);
  }
  return ret;
}

} // namespace detail
// -----------------------------------------------------------------------------------------------
// Evolution challenge

// inherit from this type in order to use the default implementation of grow
// generation
template <class Pheno, class RNG>
#if EVOL_USE_CONCEPTS == 1
requires Phenotype<Pheno, RNG>
#endif
struct DefaultChallenge {
  std::vector<Pheno> breed(std::vector<Pheno> parents, RNG &rng,
                           const EvolutionCoordinator &evolCoordinator,
                           const EvolutionOptions &options) const {
    return detail::breed<Pheno, RNG>(
        parents, rng, evolCoordinator, options.num_children,
        [](Pheno &pheno, RNG &rng, const EvolutionCoordinator &evolCoordinator,
           bool doMutate) {
          if (doMutate)
            pheno.mutate(rng, evolCoordinator);
        });
  }
};

// -----------------------------------------------------------------------------------------------
// Evolution

// This library encapsulates evolutional learning through genetic algorithms
// The library is header only and easy to use.
// You have a type (the Phenotype) whose values you want to optimize through
// evolutional learning and you have a challenge your type needs to master
//

// These functions must be added to your Phenotype type (the one you want to
// optimize)
// struct Phenotype {
//	void crossover(Phenotype const& other); make sure to randomly choose
// what data members should be taken from this or other 	void mutate();
// make sure to not randomly mutate too much of your Phenotype type (only one
// data member at a time is recommended) std::string toString() const; provide
// some output for the shape of the Phenotype
//};

// These functions must be added to your challenge type
// struct Challenge {
//	double score(Phenotype const& Phenotype); the Phenotype faces the
// challenge and its performance needs to be evaluated with a double (0 means
// bad, the higher the better the performance)
//};

template <class Pheno, class Chall, class RNG>
#if EVOL_USE_CONCEPTS == 1
requires Phenotype<Pheno, RNG> && Challenge<Chall, Pheno, RNG>
#endif
    EvolutionResult<Pheno>
    evolution(const Pheno &starting_value,     // the starting value
              const Chall &challenge,          // the challenge
              const EvolutionOptions &options, // the evolution options
              RNG &rng                         // the random number generator
    ) {
  return detail::evolution_impl(starting_value, challenge, options, rng);
}

namespace partial {

// -----------------------------------------------------------------------------------------------
// Partial Evolution challenge

// inherit from this type in order to use the default implementation of grow
// generation
template <class Pheno, class RNG>
#if EVOL_USE_CONCEPTS == 1
requires PartialPhenotype<Pheno, RNG>
#endif
struct DefaultPartialChallenge {
  std::vector<Pheno> breed(std::vector<Pheno> parents, RNG &rng,
                           const EvolutionCoordinator &evolCoordinator,
                           const PartialEvolutionOptions &options) const {
    if (parents.empty()) {
      throw std::runtime_error("no parents passed to grow generation partial.");
      return {};
    }
    std::vector<Pheno> ret;
    const auto develop = [&options](Pheno &pheno, RNG &rng,
                                    const EvolutionCoordinator &evolCoordinator,
                                    bool initialMutate) {
      if (initialMutate) {
        pheno.mutate(rng, evolCoordinator);
      }
      const auto try_n_times =
          [&options, &rng,
           &evolCoordinator](Pheno pheno, size_t num) -> std::optional<Pheno> {
        size_t i = 0;
        double magnitude = pheno.magnitude();
        while (options.min_magnitude > magnitude ||
               magnitude > options.max_magnitude) {
          pheno.mutate(rng, evolCoordinator);
          magnitude = pheno.magnitude();
          i++;
          if (i >= num) {
            break;
          }
        }
        if (options.min_magnitude <= magnitude &&
            magnitude <= options.max_magnitude) {
          return pheno;
        } else
          return std::nullopt;
      };
      std::optional<Pheno> opt_pheno = std::nullopt;
      size_t j = 0;
      while (opt_pheno == std::nullopt && j <= 1000) {
        opt_pheno = try_n_times(pheno, 1000);
        ++j;
      }
      if (!opt_pheno) {
        throw std::runtime_error(
            "could not create a Phenotype in the magnitude range min: " +
            std::to_string(options.min_magnitude) +
            " and max: " + std::to_string(options.max_magnitude));
      } else {
        pheno = *opt_pheno;
      }
    };
    return detail::breed<Pheno, RNG>(parents, rng, evolCoordinator,
                                     options.num_children, develop);
  }
};

// -----------------------------------------------------------------------------------------------
// Partial Evolution challenge

// This library encapsulates evolutional learning through genetic algorithms
// The library is header only and easy to use.
// You have a type (the partial Phenotype) whose values you want to optimize
// through evolutional learning and you have a partial challenge your type needs
// to master the difference from partial Phenotypes to Phenotypes is that only
// partial Phenosomes that are in a defined magnitude range are considered this
// enables parallelization of the evolution on multiple threads or processes and
// ensuring that no redundant work is done
//

// These functions must be added to your Phenotype type (the one you want to
// optimize)
// struct PartialPhenotype {
//	void crossover(Phenotype const& other); make sure to randomly choose
// what data members should be taken from this or other 	void mutate();
// make sure to not randomly mutate too much of your Phenotype type (only one
// data member at a time is recommended)
//  std::string toString() const; provide some output for the shape of the
//  Phenotype double magnitude() const; the square root of the sum of the
//  squares of all members
//};

// These functions must be added to your challenge type
// struct PartialChallenge {
//	double score(PatialPhenotype const& Phenotype); the Phenotype faces the
// challenge and its performance needs to be evaluated with a double (0 means
// bad, the higher the better the performance)
//};

template <class Pheno, class Chall, class RNG>
#if EVOL_USE_CONCEPTS == 1
requires PartialPhenotype<Pheno, RNG> && PartialChallenge<Chall, Pheno, RNG>
#endif
    EvolutionResult<Pheno>
    evolution(const Pheno &starting_value,            // the starting value
              const Chall &challenge,                 // the challenge
              const PartialEvolutionOptions &options, // the evolution options
              RNG &rng // the random number generator
    ) {
  return detail::evolution_impl(starting_value, challenge, options, rng);
}

} // namespace partial

namespace adjust {

// inherit from this type in order to use the default implementation of grow
// generation
template <class Pheno, class RNG>
#if EVOL_USE_CONCEPTS == 1
requires evol::partial::PartialPhenotype<Pheno, RNG>
#endif
struct DefaultAdjustChallenge {
  std::vector<Pheno> breed(std::vector<Pheno> parents, RNG &rng,
                           const EvolutionCoordinator &evolCoordinator,
                           const AdjustEvolutionOptions &options) const {
    if (parents.empty()) {
      throw std::runtime_error("no parents passed to breed adjust.");
      return {};
    }
    std::vector<Pheno> ret;
    const auto develop = [this, &options](Pheno &pheno, RNG &rng,
                                    const EvolutionCoordinator &evolCoordinator,
                                    bool initialMutate) {
      if (initialMutate) {
        pheno.mutate(rng, evolCoordinator);
      }
      else{
        return;
      }
      const auto number_mutates =
          calculate_number_mutates(options, evolCoordinator);
      for (size_t i = 0; i < number_mutates; ++i) {
        pheno.mutate(rng, evolCoordinator);
      }
    };
    return detail::breed<Pheno, RNG>(parents, rng, evolCoordinator,
                                     options.num_children, develop);
  }

private:
  size_t
  calculate_number_mutates(const AdjustEvolutionOptions &options,
                           const EvolutionCoordinator &evolCoordinator) const {
    // result between 0 and 1
    double result = evolCoordinator.get_improvement_factor();
    double number_mutates = options.max_num_mutations * result;
    return static_cast<size_t>(number_mutates);
  }
};

// -----------------------------------------------------------------------------------------------
// Adjust Evolution challenge

// This library encapsulates evolutional learning through genetic algorithms
// The library is header only and easy to use.
// You have a type (the partial Phenotype) whose values you want to optimize
// through evolutional learning and you have a partial challenge your type needs
// to master the difference from partial Phenotypes to Phenotypes is that only
// partial Phenosomes that are in a defined magnitude range are considered this
// enables parallelization of the evolution on multiple threads or processes and
// ensuring that no redundant work is done
//

// These functions must be added to your Phenotype type (the one you want to
// optimize)
// struct PartialPhenotype {
//	void crossover(Phenotype const& other); make sure to randomly choose
// what data members should be taken from this or other 	void mutate();
// make sure to not randomly mutate too much of your Phenotype type (only one
// data member at a time is recommended)
//  std::string toString() const; provide some output for the shape of the
//  Phenotype double magnitude() const; the square root of the sum of the
//  squares of all members
//};

// These functions must be added to your challenge type
// struct AdjustChallenge {
//	double score(PatialPhenotype const& Phenotype); the Phenotype faces the
// challenge and its performance needs to be evaluated with a double (0 means
// bad, the higher the better the performance)
//};

template <class Pheno, class Chall, class RNG>
#if EVOL_USE_CONCEPTS == 1
requires evol::partial::PartialPhenotype<Pheno, RNG> && AdjustChallenge<Chall, Pheno, RNG>
#endif
    EvolutionResult<Pheno>
    evolution(const Pheno &starting_value,           // the starting value
              const Chall &challenge,                // the challenge
              const AdjustEvolutionOptions &options, // the evolution options
              RNG &rng // the random number generator
    ) {
  return detail::evolution_impl(starting_value, challenge, options, rng);
}

} // namespace adjust

} // namespace evol
