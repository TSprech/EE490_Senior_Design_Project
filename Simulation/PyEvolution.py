#    This file is part of DEAP.
#
#    DEAP is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as
#    published by the Free Software Foundation, either version 3 of
#    the License, or (at your option) any later version.
#
#    DEAP is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with DEAP. If not, see <http://www.gnu.org/licenses/>.
import copy
#    example which maximizes the sum of a list of integers
#    each of which can be 0 or 1

import random
from timeit import Timer

import numpy as np

from deap import base
from deap import creator
from deap import tools
from deap import algorithms

creator.create("FitnessMin", base.Fitness, weights=(-1.0,))
creator.create("Individual", list, fitness=creator.FitnessMin)

toolbox = base.Toolbox()

# Attribute generator
#                      define 'attr_bool' to be an attribute ('gene')
#                      which corresponds to integers sampled uniformly
#                      from the range [0,1] (i.e. 0 or 1 with equal
#                      probability)
toolbox.register("attr_resistance", random.randint, 10, 100000)

# Structure initializers
#                         define 'individual' to be an individual
#                         consisting of 100 'attr_bool' elements ('genes')
toolbox.register("individual", tools.initRepeat, creator.Individual, toolbox.attr_resistance, 2) # Voltage divider
# toolbox.register("individual", tools.initRepeat, creator.Individual, toolbox.attr_resistance, 2) # Parallel resistance

# define the population to be a list of individuals
toolbox.register("population", tools.initRepeat, list, toolbox.individual)

# the goal ('fitness') function to be maximized
# @profile
def evalVoltDivider(individual):
    if individual[0] > 0 and individual[1] > 0:
        voltage = (10 * individual[1] / (individual[0] + individual[1]))
        return (3.85 - voltage)**2,  # 10V voltage divider target: 3.85V
    else:
        return 9999999,

#----------
# Operator registration
#----------
# register the goal / fitness function
# toolbox.register("evaluate", evalVoltDivider) # Optimizes the voltage divider problem
toolbox.register("evaluate", evalVoltDivider) # Optimizes the parallel resistance problem

# register the crossover operator
toolbox.register("mate", tools.cxTwoPoint)

def FastClone(x):
    ind = toolbox.individual()
    for elem in range(0, len(ind)):
        ind[elem] = x[elem]
    return ind

toolbox.register("clone", FastClone)

# register a mutation operator with a probability to
# flip each attribute/gene of 0.05
toolbox.register("mutate", tools.mutFlipBit, indpb=0.05)

# operator for selecting individuals for breeding the next
# generation: each individual of the current generation
# is replaced by the 'fittest' (best) of three individuals
# drawn randomly from the current generation.
toolbox.register("select", tools.selTournament, tournsize=3)


# @profile
def main():
    random.seed(64)
    NGEN=500
    population = toolbox.population(n=300)
    for gen in range(NGEN):
        offspring = algorithms.varAnd(population, toolbox, cxpb=0.5, mutpb=0.1)
        fits = toolbox.map(toolbox.evaluate, offspring)
        for fit, ind in zip(fits, offspring):
            ind.fitness.values = fit
        population = toolbox.select(offspring, k=len(population))
    # top10 = tools.selBest(population, k=10)

    best_ind = tools.selBest(population, 1)[0]
    print("Best individual is %s, %s" % (best_ind, best_ind.fitness.values))


# @profile  # https://github.com/pyutils/line_profiler
# def main():
#     random.seed(64)
#
#     pop = toolbox.population(n=300) # Create a population of 300 to start
#
#     # CXPB  is the probability with which two individuals
#     #       are crossed
#     #
#     # MUTPB is the probability for mutating an individual
#     CXPB, MUTPB = 0.5, 0.1
#
#     print("Start of evolution")
#
#     # Evaluate the entire population
#     fitnesses = list(map(toolbox.evaluate, pop))
#
#     for ind, fit in zip(pop, fitnesses):
#         ind.fitness.values = fit
#
#     print("  Evaluated %i individuals" % len(pop))
#
#     # Extracting all the fitnesses of
#     fits = [ind.fitness.values[0] for ind in pop]
#
#     # Variable keeping track of the number of generations
#     g = 0
#
#     # Begin the evolution
#     # while max(fits) < 9.87 and g < 3000: # Voltage divider
#     while g < 500:
#         # A new generation
#         g = g + 1
#         # print("-- Generation %i --" % g)
#
#         # Select the next generation individuals
#         offspring = toolbox.select(pop, len(pop))
#         # Clone the selected individuals
#         # result = map(toolbox.clone, offspring)
#         # offspring = copy.deepcopy(offspring)
#         result = []
#         for off in offspring:
#             func = toolbox.clone
#             clone = func(off)
#             result.append(clone)
#         offspring = result  # Huge time sink
#
#         # Apply crossover and mutation on the offspring
#         for child1, child2 in zip(offspring[::2], offspring[1::2]):
#
#             # cross two individuals with probability CXPB
#             if random.random() < CXPB:
#                 toolbox.mate(child1, child2)
#
#                 # fitness values of the children
#                 # must be recalculated later
#                 del child1.fitness.values
#                 del child2.fitness.values
#
#         for mutant in offspring:
#
#             # mutate an individual with probability MUTPB
#             if random.random() < MUTPB:
#                 toolbox.mutate(mutant)
#                 del mutant.fitness.values
#
#         # Evaluate the individuals with an invalid fitness
#         invalid_ind = [ind for ind in offspring if not ind.fitness.valid]
#         fitnesses = map(toolbox.evaluate, invalid_ind)
#         for ind, fit in zip(invalid_ind, fitnesses):
#             ind.fitness.values = fit
#
#         # print("  Evaluated %i individuals" % len(invalid_ind))
#
#         # The population is entirely replaced by the offspring
#         pop[:] = offspring
#
#         # Gather all the fitnesses in one list and print the stats
#         fits = [ind.fitness.values[0] for ind in pop]
#
#         length = len(pop)
#         mean = sum(fits) / length
#         sum2 = sum(x*x for x in fits)
#         std = abs(sum2 / length - mean**2)**0.5
#
#         # print("  Min %s" % min(fits))
#         # print("  Max %s" % max(fits))
#         # print("  Avg %s" % mean)
#         # print("  Std %s" % std)
#
#     print("-- End of (successful) evolution --")
#
#     best_ind = tools.selBest(pop, 1)[0]
#     print("Best individual is %s, %s" % (best_ind, best_ind.fitness.values))


if __name__ == "__main__":
    main()
    # t = Timer(main)
    # print(f'Time: {t.timeit(number = 1)}S')  # Time: 8.3934376 Seconds
