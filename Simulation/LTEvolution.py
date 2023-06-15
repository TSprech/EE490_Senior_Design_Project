import random
from enum import Enum
from pathlib import Path

from quantiphy import Quantity

random.seed(128)

import logging
from rich.logging import RichHandler

FORMAT = "%(message)s"
logging.basicConfig(
    level="ERROR", format=FORMAT, datefmt="[%X]", handlers=[RichHandler()]
)

import os.path
import multiprocessing
import numpy as np
from deap import algorithms, base, creator, tools
from rich.console import Console
from rich.table import Table
from MyEASimple import my_ea_simple

import CoilcraftRandomSelect as crs
import MurataRandomSelect as mrs
from LTTraceData import LTTraceData

import PyLTSpice

PyLTSpice.set_log_level(logging.ERROR)
PyLTSpice.add_log_handler(RichHandler())
from PyLTSpice.LTSteps import LTSpiceLogReader
from PyLTSpice import RawRead, SimRunner, SpiceEditor


def init(top_netlist):
    global netlist
    netlist = top_netlist


high_side = True
low_side = False

deadband = 8  # ns


def pulse_period(frequency):
    half_period = 1 / frequency
    return {"LS": half_period + (deadband * 1E-9 * 2), "HS": half_period, "Period": half_period * 2}


def frequencytoltpulse(frequency, side: bool):
    period = 1 / (frequency / 1E9)  # Convert to Gigahertz to ensure the period will be in nanoseconds
    if side == low_side:
        return f"PULSE(3.3 0 0n 1n 1n {period + (deadband * 2)}n {period * 2}n)"
    else:
        return f"PULSE(0 3.3 {deadband}n 1n 1n {period}n {period * 2}n)"


def simulate_circuit(fsw: int, ind_index: int, cap1_index: int, cap2_index: int, cap3_index: int, cap4_index: int):
    ltc = SimRunner(output_folder='LTFiles', verbose=False)

    netlist.set_component_value('VHSin', frequencytoltpulse(fsw, high_side))
    netlist.set_component_value('VLSin', frequencytoltpulse(fsw, low_side))

    netlist.add_instruction(crs.indexed_coilcraft_inductor(ind_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C1', cap1_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C2', cap2_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C3', cap3_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C4', cap4_index)['SubCkt'])

    filename = f"_F-{fsw}_I-{ind_index}_C1-{cap1_index}_C2-{cap2_index}_C3-{cap3_index}_C4-{cap4_index}"
    if not os.path.isfile(f"LTFiles/{filename}.raw"):
        return ltc.run_now(netlist, run_filename=filename)
    else:
        logging.debug("Sim already ran")
        try:
            ltr = RawRead(f'LTFiles/{filename}.raw', verbose=False)
        except:
            return ltc.run_now(netlist, run_filename=filename)
        return f'LTFiles/{filename}.raw', f'LTFiles/{filename}.log'


def evaluate_individual(individual):
    periods = pulse_period(individual[Genes.FREQUENCY.value])

    raw, log = simulate_circuit(individual[Genes.FREQUENCY.value], individual[Genes.L1.value], individual[Genes.C1.value], individual[Genes.C2.value], individual[Genes.C3.value], individual[Genes.C4.value])

    log_data = LTSpiceLogReader(log_filename=log)

    reward_efficiency = raw_efficiency = log_data.get_measure_value('eff')
    v_out_ptp = log_data.get_measure_value('v_out_ptp')
    v_out_min = log_data.get_measure_value('v_out_min')
    v_out_max = log_data.get_measure_value('v_out_max')

    if raw is None:
        reward_efficiency = 0

    acceptable_ripple_ptp = 0.1  # Volts
    if v_out_ptp > acceptable_ripple_ptp:
        reward_efficiency = reward_efficiency / (1 * (acceptable_ripple_ptp - v_out_ptp))

    if v_out_min < 4 or v_out_max > 7:
        reward_efficiency = 0

    data = {"SimOK": raw is not None, "RawFilename": Path(raw).name, "LogFilename": Path(log).name, "RawEfficiency": raw_efficiency, "AdjustedEfficiency": reward_efficiency, "VOutPtP": Quantity(v_out_ptp, 'V'),
            "VOutMin": Quantity(v_out_min, 'V'), "VOutMax": Quantity(v_out_max, 'V'), "PWM": {
            "Frequency": Quantity(individual[Genes.FREQUENCY.value], 'Hz'),
            "Period": Quantity(periods["Period"], 's'),
            "PeriodLS": Quantity(periods["LS"], 's'),
            "PeriodHS": Quantity(periods["HS"], 's'),
            "Deadtime": Quantity(deadband, 'ns')
        }, "Inductors": [{
            "Index": individual[Genes.L1.value],
            "Value": Quantity(crs.indexed_coilcraft_inductor(individual[Genes.L1.value])["Inductance"], "H"),
            "PartNumber": crs.indexed_coilcraft_inductor(individual[Genes.L1.value])["PartNumber"]
        }, ], 'Capacitors': [{
            "Name": f"C{i}",
            "Index": individual[i],
            "Value": Quantity(mrs.indexed_murata_capacitor("", individual[i])["Capacitance"], "F"),
            "PartNumber": mrs.indexed_murata_capacitor("", individual[i])["PartNumber"]
        } for i in range(Genes.C1.value, Genes.C4.value+1)]}

    return data


creator.create("FitnessMax", base.Fitness, weights=(1.0,))  # Positive weight means the goal is to achieve a maximum, negative would be if the evaluate function returned an error and the GA would work to minimize error
creator.create("Individual", list, fitness=creator.FitnessMax)  # Create the individual definition which is just a list trying to achieve the goal specified above

toolbox = base.Toolbox()  # Create a toolbox which manages the specific parameters for evolution

# Attribute Generators (Genes)
toolbox.register("attr_history", str, "")  # Attribute to describe inductance, same as above but it uses uniform to generate an inductance
toolbox.register("attr_frequency", random.randint, 100000, 3000000)  # Attribute to describe frequency, it will be generated using randint between 100kHz and 3MHz
# toolbox.register("attr_frequency", random.randint, 100, 300)  # Attribute to describe frequency, it will be generated using randint between 100kHz and 3MHz
toolbox.register("attr_inductance", random.randint, 0, crs.number_of_models())  # Attribute to describe inductance, same as above but it uses uniform to generate an inductance
toolbox.register("attr_capacitance_1", random.randint, 0, mrs.number_of_models())  # Same as inductance
toolbox.register("attr_capacitance_2", random.randint, 0, mrs.number_of_models())  # Same as inductance
toolbox.register("attr_capacitance_3", random.randint, 0, mrs.number_of_models())  # Same as inductance
toolbox.register("attr_capacitance_4", random.randint, 0, mrs.number_of_models())  # Same as inductance

class Genes(Enum):
    HISTORY = 0
    FREQUENCY = 1
    L1 = 2
    C1 = 3
    C2 = 4
    C3 = 5
    C4 = 6

# Individual Structure Initializer
toolbox.register("individual", tools.initCycle, creator.Individual, (
    toolbox.attr_history, toolbox.attr_frequency, toolbox.attr_inductance, toolbox.attr_capacitance_1, toolbox.attr_capacitance_2,
    toolbox.attr_capacitance_3, toolbox.attr_capacitance_4))  #

# Population Structure Initializer
toolbox.register("population", tools.initRepeat, list,
                 toolbox.individual)  # Define the population to be a list of individuals

# Evolution Evaluation Metric
toolbox.register("evaluate",
                 evaluate_individual)  # Set evaluate up to use whatever evaluates the fitness of an individual, in this case, it works to maximize the return value of the passed function


def cxSpice(ind1, ind2, indpb=0.2):
    for i in range(len(ind1) - 1):
        if random.random() < indpb:
            ind1[i], ind2[i] = ind2[i], ind1[i]

    return ind1, ind2


# Crossover Operator
toolbox.register("mate", cxSpice)  # Used when crossing 2 individuals


def clamp(num, min_value, max_value):
    return max(min(num, max_value), min_value)


def mutate_frequency(current: int) -> int:
    min_freq, max_freq = 100E3, 3000E3
    # min_freq, max_freq = 100, 300
    random_multiplier = random.uniform(1E-2, 5E-1)
    offset = round(current * random_multiplier)
    offset = 1 if random.random() > 0.5 else -1 * offset
    mutated_freq = current + offset
    return clamp(mutated_freq, min_freq, max_freq)


def mutate_indexed(current: int, min_index: int, max_index: int):
    random_multiplier = random.uniform(1E-3, 1E-1)
    offset = round(current * random_multiplier)
    offset = 1 if random.random() > 0.5 else -1 * offset
    mutated_index = current + offset
    return clamp(mutated_index, min_index, max_index)


def mutate_netlist(individual, indpb):  # TODO: Make this shift a small amount based on the current attribute value, not just random
    if random.random() < indpb:
        option = random.random()
        if option < 1 / 6:
            individual[Genes.FREQUENCY.value] = mutate_frequency(individual[Genes.FREQUENCY.value])
        elif option < 2 / 6:
            individual[Genes.L1.value] = mutate_indexed(individual[Genes.L1.value], 0, crs.number_of_models())
        elif option < 3 / 6:
            individual[Genes.C1.value] = mutate_indexed(individual[Genes.C1.value], 0, mrs.number_of_models())
        elif option < 4 / 6:
            individual[Genes.C2.value] = mutate_indexed(individual[Genes.C2.value], 0, mrs.number_of_models())
        elif option < 5 / 6:
            individual[Genes.C3.value] = mutate_indexed(individual[Genes.C3.value], 0, mrs.number_of_models())
        else:
            individual[Genes.C4.value] = mutate_indexed(individual[Genes.C4.value], 0, mrs.number_of_models())
    return individual,


toolbox.register("mutate", mutate_netlist, indpb=0.6)  # This defines the function used to mutate an individual, along with the probability threshold used inside the passed function

# operator for selecting individuals for breeding the next
# generation: each individual of the current generation
# is replaced by the 'fittest' (best) of three individuals
# drawn randomly from the current generation.
toolbox.register("select", tools.selTournament, tournsize=3)


class myHOF(tools.HallOfFame):
    def __init__(self, maxsize):
        super().__init__(maxsize)
        self.table = Table(show_header=True)
        self.console = Console()

    def update(self, population):
        self.table = Table(show_header=True)
        self.table.add_column('Frequency')
        self.table.add_column('Inductance')
        self.table.add_column('Capacitor 1')
        self.table.add_column('Capacitor 2')
        self.table.add_column('Capacitor 3')
        self.table.add_column('Capacitor 4')
        self.table.add_column('Efficiency')
        self.table.add_column('Voltage Out')
        self.table.add_column('Peak to Peak')

        """Update the hall of fame with the *population* by replacing the
        worst individuals in it by the best individuals present in
        *population* (if they are better). The size of the hall of fame is
        kept constant.

        :param population: A list of individual with a fitness attribute to
                           update the hall of fame with.
        """
        for ind in population:
            if len(self) == 0 and self.maxsize != 0:
                # Working on an empty hall of fame is problematic for the
                # "for else"
                self.insert(population[0])
                continue
            if ind.fitness > self[-1].fitness or len(self) < self.maxsize:
                for hofer in self:
                    # Loop through the hall of fame to check for any
                    # similar individual
                    if self.similar(ind, hofer):
                        break
                else:
                    # The individual is unique and strictly better than
                    # the worst
                    if len(self) >= self.maxsize:
                        self.remove(-1)
                    self.insert(ind)

            self.table.add_row(f'{ind[0]}', f'{ind[1]}', f'{ind[2]}', f'{ind[3]}', f'{ind[4]}', f'{ind[5]}', f'{ind.fitness.values[0]}', f'{1}', f'{1}')
        # self.console.print(self.table)


def main():
    top_netlist = SpiceEditor('LTFiles/EPC23102_Mine.asc')

    # cpu_count = multiprocessing.cpu_count()
    cpu_count = 5  # TODO: Doesn't work for # of individuals = 2
    logging.info(f"CPU count: {cpu_count}")
    pool = multiprocessing.Pool(cpu_count - 1, initializer=init, initargs=(top_netlist,))  # Huge thanks to: https://gist.github.com/AvalZ/f019c9adbc15c505578b99041fb803d7
    toolbox.register("map", pool.map)

    pop = toolbox.population(n=cpu_count - 1)
    hof = myHOF(1)
    stats = tools.Statistics(lambda ind: ind.fitness.values)
    stats.register("avg", np.mean)
    stats.register("std", np.std)
    stats.register("min", np.min)
    stats.register("max", np.max)

    my_ea_simple(pop, toolbox, cxpb=0.5, mutpb=0.8, ngen=1, stats=stats, halloffame=hof)
    best_ind = tools.selBest(pop, 1)[0]
    logging.info("Best individual is %s, %s" % (best_ind, best_ind.fitness.values))

    pool.close()


if __name__ == "__main__":
    main()
