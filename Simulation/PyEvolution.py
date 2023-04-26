import random
import multiprocessing
import numpy

from deap import base
from deap import creator
from deap import tools
from deap import algorithms

import BuckParameters as bp

import PyLTSpice as pylt
from PyLTSpice import SimCommander

import time


creator.create("FitnessMax", base.Fitness, weights=(1.0,))  # Positive weight means the goal is to achieve a maximum, negative would be useful if the evaluate function returned an error and the GA would work to minimize error
creator.create("Individual", list, fitness=creator.FitnessMax)  # Create the individual definition which is just a list trying to achieve the goal specified above

toolbox = base.Toolbox()  # Create a toolbox which manages the specific parameters for evolution

# Attribute Generators (Genes)
toolbox.register("attr_frequency", random.randint, 100000, 3000000)  # Attribute to describe frequency, it will be generated using randint between 100kHz and 3MHz
toolbox.register("attr_inductance", random.uniform, 0.000000001, 0.00005)  # Attribute to describe inductance, same as above but it uses uniform to generate an inductance
toolbox.register("attr_capacitance", random.uniform, 0.000000001, 0.0001)  # Same as inductance

# Individual Structure Initializer
toolbox.register("individual", tools.initCycle, creator.Individual, (toolbox.attr_frequency, toolbox.attr_inductance, toolbox.attr_capacitance))  #

# Population Structure Initializer
toolbox.register("population", tools.initRepeat, list, toolbox.individual)  # Define the population to be a list of individuals

# def delay(ind):
#     time.sleep(0.1)
#     return 0,

# Evolution Evaluation Metric
toolbox.register("evaluate", bp.calculate_efficiency)  # Set evaluate up to use whatever evaluates the fitness of an individual, in this case, it works to maximize the return value of the passed function
# toolbox.register("evaluate", delay)  # Set evaluate up to use whatever evaluates the fitness of an individual, in this case, it works to maximize the return value of the passed function

# Crossover Operator
toolbox.register("mate", tools.cxTwoPoint)  # Used when crossing 2 individuals

def mutBuck(individual, indpb):  # TODO: Make this shift a small amount based on the current attribute value, not just random
    if random.random() < indpb:
        individual[0] = (individual[0] + toolbox.attr_frequency()) / 2
        # individual[0] = toolbox.attr_frequency()
    if random.random() < indpb:
        individual[1] = (individual[1] + toolbox.attr_inductance()) / 2
        # individual[1] = toolbox.attr_inductance()
    if random.random() < indpb:
        individual[2] = (individual[2] + toolbox.attr_capacitance()) / 2
        # individual[2] = toolbox.attr_capacitance()
    return individual,


toolbox.register("mutate", mutBuck, indpb=0.1)  # This defines the function used to mutate an individual, along with the probability threshold used inside the passed function

# operator for selecting individuals for breeding the next
# generation: each individual of the current generation
# is replaced by the 'fittest' (best) of three individuals
# drawn randomly from the current generation.
toolbox.register("select", tools.selTournament, tournsize=3)

if __name__ == "__main__":
    random.seed(128)
    # print(time.process_time())
    # cpu_count = multiprocessing.cpu_count()
    # print(f"CPU count: {cpu_count}")
    # pool = multiprocessing.Pool(cpu_count)
    # toolbox.register("map", pool.map)

    pop = toolbox.population(n=50)
    hof = tools.HallOfFame(1)
    stats = tools.Statistics(lambda ind: ind.fitness.values)
    stats.register("avg", numpy.mean)
    stats.register("std", numpy.std)
    stats.register("min", numpy.min)
    stats.register("max", numpy.max)

    algorithms.eaSimple(pop, toolbox, cxpb=0.5, mutpb=0.8, ngen=100, stats=stats, halloffame=hof)
    best_ind = tools.selBest(pop, 1)[0]
    print("Best individual is %s, %s" % (best_ind, best_ind.fitness.values))

    # print(time.process_time())
    # pool.close()
