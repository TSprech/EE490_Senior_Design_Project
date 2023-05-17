import random
import multiprocessing

import numpy as np
from PyLTSpice import RawRead, SimRunner, SpiceEditor
from deap import algorithms, base, creator, tools

import CoilcraftRandomSelect as crs
import MurataRandomSelect as mrs
from LTTraceData import LTTraceData

high_side = True
low_side = False

# top_netlist = SpiceEditor('LTFiles/EPC23102_Mine.asc')

# top_netlist = copy.deepcopy(SpiceEditor('LTFiles/EPC23102_Mine.asc'))


def init(top_netlist):
    global netlist
    netlist = top_netlist


def frequencytoltpulse(frequency, side: bool):
    deadband = 1  # ns
    period = 1 / (frequency / 1E9)  # Convert to Gigahertz to ensure the period will be in nanoseconds
    if side == low_side:
        return f"PULSE(3.3 0 0n 1n 1n {period + (deadband * 2)}n {period * 2}n)"
    else:
        return f"PULSE(0 3.3 {deadband}n 1n 1n {period}n {period * 2}n)"


def simulate_circuit(fsw: int, ind_index: int, cap1_index: int, cap2_index: int, cap3_index: int, cap4_index: int):
    ltc = SimRunner(output_folder='LTFiles', verbose=False)
    # netlist = shared.getConst('top_netlist')
    # netlist = SpiceEditor('LTFiles/EPC23102_Mine.asc')
    # global top_netlist
    # global top_netlist
    # netlist = copy.deepcopy(top_netlist)
    # netlist = test()

    netlist.set_component_value('VHSin', frequencytoltpulse(fsw, high_side))
    netlist.set_component_value('VLSin', frequencytoltpulse(fsw, low_side))

    netlist.add_instruction(crs.indexed_coilcraft_inductor(ind_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C1', cap1_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C2', cap2_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C3', cap3_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C4', cap4_index)['SubCkt'])

    return ltc.run_now(netlist, run_filename=f"I-{ind_index}_C1-{cap1_index}_C2-{cap2_index}_C3-{cap3_index}_C4-{cap4_index}")


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


creator.create("FitnessMax", base.Fitness, weights=(1.0,))  # Positive weight means the goal is to achieve a maximum, negative would be if the evaluate function returned an error and the GA would work to minimize error
creator.create("Individual", list, fitness=creator.FitnessMax)  # Create the individual definition which is just a list trying to achieve the goal specified above

toolbox = base.Toolbox()  # Create a toolbox which manages the specific parameters for evolution

# Attribute Generators (Genes)
toolbox.register("attr_frequency", random.randint, 80000, 120000)  # Attribute to describe frequency, it will be generated using randint between 100kHz and 3MHz
toolbox.register("attr_inductance", random.randint, 0, crs.number_of_models())  # Attribute to describe inductance, same as above but it uses uniform to generate an inductance
toolbox.register("attr_capacitance_1", random.randint, 0, mrs.number_of_models())  # Same as inductance
toolbox.register("attr_capacitance_2", random.randint, 0, mrs.number_of_models())  # Same as inductance
toolbox.register("attr_capacitance_3", random.randint, 0, mrs.number_of_models())  # Same as inductance
toolbox.register("attr_capacitance_4", random.randint, 0, mrs.number_of_models())  # Same as inductance

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


def mutBuck(individual, indpb):  # TODO: Make this shift a small amount based on the current attribute value, not just random
    shift_amount = 10
    if random.random() < indpb:
        individual[0] = round((individual[0] + toolbox.attr_frequency()) / shift_amount)
    if random.random() < indpb:
        individual[1] = round((individual[1] + toolbox.attr_inductance()) / shift_amount)
    if random.random() < indpb:
        individual[2] = round((individual[2] + toolbox.attr_capacitance_1()) / shift_amount)
    if random.random() < indpb:
        individual[3] = round((individual[3] + toolbox.attr_capacitance_2()) / shift_amount)
    if random.random() < indpb:
        individual[4] = round((individual[4] + toolbox.attr_capacitance_3()) / shift_amount)
    if random.random() < indpb:
        individual[5] = round((individual[5] + toolbox.attr_capacitance_4()) / shift_amount)
    return individual,


toolbox.register("mutate", mutBuck,
                 indpb=0.4)  # This defines the function used to mutate an individual, along with the probability threshold used inside the passed function

# operator for selecting individuals for breeding the next
# generation: each individual of the current generation
# is replaced by the 'fittest' (best) of three individuals
# drawn randomly from the current generation.
toolbox.register("select", tools.selTournament, tournsize=3)


def main():
    # shared.setConst(top_netlist=SpiceEditor('LTFiles/EPC23102_Mine.asc'))  # This is the LTSpice schematic, it must be a scoop const to prevent simultaneous access to the file
    # shared_memory.SharedMemory('schematic', size=3000000, create=True)
    # global top_netlist
    # top_netlist = copy.deepcopy(SpiceEditor('LTFiles/EPC23102_Mine.asc'))
    top_netlist = SpiceEditor('LTFiles/EPC23102_Mine.asc')

    random.seed(128)

    print(f"CPU count: {multiprocessing.cpu_count()}")
    # pool = ProcessingPool(5)
    pool = multiprocessing.Pool(multiprocessing.cpu_count(), initializer=init, initargs=(top_netlist,))  # Huge thanks to: https://gist.github.com/AvalZ/f019c9adbc15c505578b99041fb803d7
    toolbox.register("map", pool.map)

    pop = toolbox.population(n=multiprocessing.cpu_count())
    hof = tools.HallOfFame(1)
    stats = tools.Statistics(lambda ind: ind.fitness.values)
    stats.register("avg", np.mean)
    stats.register("std", np.std)
    stats.register("min", np.min)
    stats.register("max", np.max)

    algorithms.eaSimple(pop, toolbox, cxpb=0.5, mutpb=0.8, ngen=5, stats=stats, halloffame=hof)
    best_ind = tools.selBest(pop, 1)[0]
    print("Best individual is %s, %s" % (best_ind, best_ind.fitness.values))

    pool.close()

if __name__ == "__main__":
    main()
