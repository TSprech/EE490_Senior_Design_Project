import random
import time
# import multiprocessing
import pathos
import numpy
from PyLTSpice import RawRead, SimRunner, SpiceEditor
from deap import algorithms
from deap import base
from deap import creator
from deap import tools
from pathos.multiprocessing import ProcessingPool
from scoop import futures

import CoilcraftRandomSelect as crs
import MurataRandomSelect as mrs
from LTTraceData import LTTraceData

# if __name__ == "__main__":
high_side = True
low_side = False

def frequencytoltpulse(frequency, side: bool):
    deadband = 8  # ns
    period = 1 / (frequency / 1E9)  # Convert to Gigahertz to ensure the period will be in nanoseconds
    if side == low_side:
        return f"PULSE(3.3 0 0n 1n 1n {period + (deadband * 2)}n {period * 2}n)"
    else:
        return f"PULSE(0 3.3 {deadband}n 1n 1n {period}n {period * 2}n)"


def simulate_circuit(fsw: int, ind_index: int, cap1_index: int, cap2_index: int, cap3_index: int, cap4_index: int):
    ltc = SimRunner(output_folder='LTFiles', verbose=False)
    netlist = SpiceEditor('LTFiles/EPC23102_Mine.asc')

    netlist.set_component_value('VHSin', frequencytoltpulse(fsw, high_side))
    netlist.set_component_value('VLSin', frequencytoltpulse(fsw, low_side))

    netlist.add_instruction(crs.indexed_coilcraft_inductor(ind_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C1', cap1_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C2', cap2_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C3', cap3_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C4', cap4_index)['SubCkt'])

    return ltc.run_now(netlist,
                       run_filename=f"I-{ind_index}_C1-{cap1_index}_C2-{cap2_index}_C3-{cap3_index}_C4-{cap4_index}")

def evaluate_individual(individual):
    print(f'{individual[0]}, {individual[1]}, {individual[2]}, {individual[3]}, {individual[4]}, {individual[5]}')
    raw, log = simulate_circuit(individual[0], individual[1], individual[2], individual[3], individual[4], individual[5])
    if raw is None:
        return 0,

    ltr = RawRead(raw, verbose=False)

    v_in = LTTraceData(ltr, 'V(vin)')
    v_out = LTTraceData(ltr, 'V(vout)')
    i_in = LTTraceData(ltr, 'I(Vs)')
    i_out = LTTraceData(ltr, 'I(Il)')

    if v_out.peak_to_peak > 1:
        return 0,

    if v_out.min < 3 or v_out.max > 7:
        return 0,

    return ((v_out.average * i_out.average) / (v_in.average * i_in.average)),

creator.create("FitnessMax", base.Fitness, weights=(
    1.0,))  # Positive weight means the goal is to achieve a maximum, negative would be useful if the evaluate function returned an error and the GA would work to minimize error
creator.create("Individual", list,
               fitness=creator.FitnessMax)  # Create the individual definition which is just a list trying to achieve the goal specified above

toolbox = base.Toolbox()  # Create a toolbox which manages the specific parameters for evolution

# Attribute Generators (Genes)
# toolbox.register("attr_frequency", random.randint, 100000, 3000000)  # Attribute to describe frequency, it will be generated using randint between 100kHz and 3MHz
# toolbox.register("attr_frequency", random.randint, 100000, 300000)  # Attribute to describe frequency, it will be generated using randint between 100kHz and 3MHz
toolbox.register("attr_frequency", random.randint, 100, 300)  # Attribute to describe frequency, it will be generated using randint between 100kHz and 3MHz
toolbox.register("attr_inductance", random.randint, 0,
                 3409)  # Attribute to describe inductance, same as above but it uses uniform to generate an inductance
toolbox.register("attr_capacitance_1", random.randint, 0, 11672)  # Same as inductance
toolbox.register("attr_capacitance_2", random.randint, 0, 11672)  # Same as inductance
toolbox.register("attr_capacitance_3", random.randint, 0, 11672)  # Same as inductance
toolbox.register("attr_capacitance_4", random.randint, 0, 11672)  # Same as inductance

# Individual Structure Initializer
toolbox.register("individual", tools.initCycle, creator.Individual, (
    toolbox.attr_frequency, toolbox.attr_inductance, toolbox.attr_capacitance_1, toolbox.attr_capacitance_2,
    toolbox.attr_capacitance_3, toolbox.attr_capacitance_4))  #

# Population Structure Initializer
toolbox.register("population", tools.initRepeat, list,
                 toolbox.individual)  # Define the population to be a list of individuals

# Evolution Evaluation Metric
toolbox.register("evaluate",
                 evaluate_individual)  # Set evaluate up to use whatever evaluates the fitness of an individual, in this case, it works to maximize the return value of the passed function

# Crossover Operator
toolbox.register("mate", tools.cxTwoPoint)  # Used when crossing 2 individuals


def mutBuck(individual,
            indpb):  # TODO: Make this shift a small amount based on the current attribute value, not just random
    if random.random() < indpb:
        individual[0] = round((individual[0] + toolbox.attr_frequency()) / 6)
    if random.random() < indpb:
        individual[1] = round((individual[1] + toolbox.attr_inductance()) / 6)
    if random.random() < indpb:
        individual[2] = round((individual[2] + toolbox.attr_capacitance_1()) / 6)
    if random.random() < indpb:
        individual[3] = round((individual[3] + toolbox.attr_capacitance_2()) / 6)
    if random.random() < indpb:
        individual[4] = round((individual[4] + toolbox.attr_capacitance_3()) / 6)
    if random.random() < indpb:
        individual[5] = round((individual[5] + toolbox.attr_capacitance_4()) / 6)
    return individual,


toolbox.register("mutate", mutBuck,
                 indpb=0.4)  # This defines the function used to mutate an individual, along with the probability threshold used inside the passed function

# operator for selecting individuals for breeding the next
# generation: each individual of the current generation
# is replaced by the 'fittest' (best) of three individuals
# drawn randomly from the current generation.
toolbox.register("select", tools.selTournament, tournsize=3)

def main():
    random.seed(128)
    print(time.process_time())
    # cpu_count = multiprocessing.cpu_count()
    # print(f"CPU count: {cpu_count}")
    # pool = multiprocessing.Pool(cpu_count)
    # toolbox.register("map", pool.map)

    # pool = ProcessingPool(20)
    # toolbox.register("map", pool.map)

    toolbox.register("map", futures.map)

    pop = toolbox.population(n=5)
    hof = tools.HallOfFame(1)
    stats = tools.Statistics(lambda ind: ind.fitness.values)
    stats.register("avg", numpy.mean)
    stats.register("std", numpy.std)
    stats.register("min", numpy.min)
    stats.register("max", numpy.max)

    algorithms.eaSimple(pop, toolbox, cxpb=0.5, mutpb=0.8, ngen=10, stats=stats, halloffame=hof)
    best_ind = tools.selBest(pop, 1)[0]
    print("Best individual is %s, %s" % (best_ind, best_ind.fitness.values))

    # print(time.process_time())
    # pool.close()

if __name__ == "__main__":
    main()
