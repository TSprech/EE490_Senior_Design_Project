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

import BuckParameters as bp

from deap import base
from deap import creator
from deap import tools
from deap import algorithms

class Indi:
    freq: int
    ind: float
    cap: float

creator.create("FitnessMax", base.Fitness, weights=(1.0,))
creator.create("Individual", list, fitness=creator.FitnessMax)

toolbox = base.Toolbox()

# Attribute generator
#                      define 'attr_bool' to be an attribute ('gene')
#                      which corresponds to integers sampled uniformly
#                      from the range [0,1] (i.e. 0 or 1 with equal
#                      probability)
toolbox.register("attr_frequency", random.randint, 100000, 3000000)
toolbox.register("attr_inductance", random.uniform, 0.000000001, 0.001)
toolbox.register("attr_capacitance", random.uniform, 0.000000001, 0.0001)

# Structure initializers
#                         define 'individual' to be an individual
#                         consisting of 100 'attr_bool' elements ('genes')
toolbox.register("individual", tools.initCycle, creator.Individual, (toolbox.attr_frequency, toolbox.attr_inductance, toolbox.attr_capacitance), 1) # Voltage divider
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
toolbox.register("evaluate", bp.calculate_efficiency) # Optimizes the parallel resistance problem

# register the crossover operator
toolbox.register("mate", tools.cxTwoPoint)

def FastClone(x):
    ind = toolbox.individual()
    for elem in range(0, len(ind)):
        ind[elem] = x[elem]
    return ind

toolbox.register("clone", FastClone)  # Enable for faster copy but slightly less optimized result

def mutBuck(individual, indpb):
    """Mutate an individual by replacing attributes, with probability *indpb*,
    by a integer uniformly drawn between *low* and *up* inclusively.

    :param individual: :term:`Sequence <sequence>` individual to be mutated.
    :param low: The lower bound or a :term:`python:sequence` of
                of lower bounds of the range from which to draw the new
                integer.
    :param up: The upper bound or a :term:`python:sequence` of
               of upper bounds of the range from which to draw the new
               integer.
    :param indpb: Independent probability for each attribute to be mutated.
    :returns: A tuple of one individual.
    """
    if random.random() < indpb:
        individual[0] = toolbox.attr_frequency()
    if random.random() < indpb:
        individual[1] = toolbox.attr_inductance()
    if random.random() < indpb:
        individual[2] = toolbox.attr_capacitance()

    return individual,

# register a mutation operator with a probability to
# flip each attribute/gene of 0.05
toolbox.register("mutate", mutBuck, indpb=0.1) # TODO: make this not FlipBit

# operator for selecting individuals for breeding the next
# generation: each individual of the current generation
# is replaced by the 'fittest' (best) of three individuals
# drawn randomly from the current generation.
toolbox.register("select", tools.selTournament, tournsize=3)


# @profile
def main():
    random.seed(128)
    NGEN=500
    population = toolbox.population(n=100)
    for gen in range(NGEN):
        offspring = algorithms.varAnd(population, toolbox, cxpb=0.5, mutpb=0.25)  # cxpb is probability two individuals are cross, mutpb is probability an individual mutates
        fits = toolbox.map(toolbox.evaluate, offspring)
        for fit, ind in zip(fits, offspring):
            ind.fitness.values = fit
        population = toolbox.select(offspring, k=len(population))

        # # Gather all the fitnesses in one list and print the stats
        # fits = [ind.fitness.values[0] for ind in pop]
        #
        # length = len(pop)
        # mean = sum(fits) / length
        # sum2 = sum(x*x for x in fits)
        # std = abs(sum2 / length - mean**2)**0.5
        #
        # # print("  Min %s" % min(fits))
        # # print("  Max %s" % max(fits))
        # # print("  Avg %s" % mean)
        # # print("  Std %s" % std)

    best_ind = tools.selBest(population, 1)[0]
    print("Best individual is %s, %s" % (best_ind, best_ind.fitness.values))
    print(bp.calculate_efficiency(best_ind))

if __name__ == "__main__":
    # main()
    t = Timer(main)
    print(f'Time: {t.timeit(number = 10)}S')  # Time: 8.3934376 Seconds
