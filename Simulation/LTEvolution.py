import random

import quantiphy

random.seed(128)  # Allows for easy debugging of the evolution

from enum import Enum
from pathlib import Path
import os.path
import multiprocessing
import numpy as np
from quantiphy import Quantity
from deap import base, creator, tools

from MyEASimple import logging_ea_simple
import CoilcraftRandomSelect as crs
import MurataRandomSelect as mrs

import logging
from rich.logging import RichHandler

logging.basicConfig(level="INFO", format="%(message)s", datefmt="[%X]", handlers=[RichHandler()])
import PyLTSpice

PyLTSpice.set_log_level(logging.ERROR)
PyLTSpice.add_log_handler(RichHandler())
from PyLTSpice.LTSteps import LTSpiceLogReader
from PyLTSpice import RawRead, SimRunner, SpiceEditor


class PWMSide(Enum):
    LOW = 0,
    HIGH = 1


class Genes(Enum):
    HISTORY = 0
    FREQUENCY = 1
    L1 = 2
    C1 = 3
    C2 = 4
    C3 = 5
    C4 = 6


min_frequency, max_frequency = Quantity(100, "kHz"), Quantity(3, "MHz")
c_kHz_Hz = quantiphy.UnitConversion('Hz', 'kHz', 1000, 0)
c_MHz_Hz = quantiphy.UnitConversion('Hz', 'MHz', 1000000, 0)
c_GHz_Hz = quantiphy.UnitConversion('Hz', 'GHz', 1000000000, 0)


def pulse_period(frequency, deadband: quantiphy.Quantity(0, units='ns')):
    half_period = 1 / frequency
    return {"LS": half_period + (int(deadband) * 1E-9 * 2), "HS": half_period, "Period": half_period * 2}


def frequency_to_ltpulse(frequency, deadband: quantiphy.Quantity(0, units='ns'), side: PWMSide):
    period = 1 / (frequency / 1E9)  # Convert to Gigahertz to ensure the period will be in nanoseconds
    if side == PWMSide.LOW:
        return f"PULSE(3.3 0 0n 1n 1n {period + (int(deadband) * 2)}n {period * 2}n)"
    else:
        return f"PULSE(0 3.3 {int(deadband)}n 1n 1n {period}n {period * 2}n)"


def simulate_circuit(fsw: int, ind_index: int, cap1_index: int, cap2_index: int, cap3_index: int, cap4_index: int):
    ltc = SimRunner(output_folder='LTFiles', verbose=False)  # Handles running the simulation using LTSpice

    netlist.set_component_value('VHSin', frequency_to_ltpulse(fsw, quantiphy.Quantity(8, 'ns'), PWMSide.HIGH))  # Set the high side PWM signal pulse duration
    netlist.set_component_value('VLSin', frequency_to_ltpulse(fsw, quantiphy.Quantity(8, 'ns'), PWMSide.LOW))  # Set the low side PWM signal pulse duration

    netlist.add_instruction(crs.indexed_coilcraft_inductor(ind_index)['SubCkt'])  # Add the spice directive which describes the inductor
    netlist.add_instruction(mrs.indexed_murata_capacitor('C1', cap1_index)['SubCkt'])  # Add the spice directive which describes all the capacitors
    netlist.add_instruction(mrs.indexed_murata_capacitor('C2', cap2_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C3', cap3_index)['SubCkt'])
    netlist.add_instruction(mrs.indexed_murata_capacitor('C4', cap4_index)['SubCkt'])

    filename = f"_F-{fsw}_I-{ind_index}_C1-{cap1_index}_C2-{cap2_index}_C3-{cap3_index}_C4-{cap4_index}"  # Generate a unique file name (this will be used to also check if the simulation has been run already)
    if not os.path.isfile(f"LTFiles/{filename}.raw"):  # If the simulation has NOT already run with the given part choices
        return ltc.run_now(netlist, run_filename=filename)  # Run the simulation
    else:
        logging.debug("Sim already ran")  # The simulation has already run, so just pull the results from the existing file
        try:
            ltr = RawRead(f'LTFiles/{filename}.raw', verbose=False)  # Try to open the raw file, to make sure the simulation ran to completion
        except:  # If the raw file cannot be read properly for whatever reason (sim didn't finish, there was a sim error, corrupt file, etc.), run the simulation again
            return ltc.run_now(netlist, run_filename=filename)
        return f'LTFiles/{filename}.raw', f'LTFiles/{filename}.log'  # Return the raw and log file name pair


def evaluate_individual(individual):
    raw, log = simulate_circuit(individual[Genes.FREQUENCY.value], individual[Genes.L1.value], individual[Genes.C1.value], individual[Genes.C2.value], individual[Genes.C3.value], individual[Genes.C4.value])

    log_data = LTSpiceLogReader(log_filename=log)

    try:
        reward_efficiency = raw_efficiency = log_data.get_measure_value('eff')
        v_out_ptp = log_data.get_measure_value('v_out_ptp')
        v_out_min = log_data.get_measure_value('v_out_min')
        v_out_max = log_data.get_measure_value('v_out_max')
    except:  # The sim has failed in such a way that useful data is not available, as such, penalize the individual
        reward_efficiency = raw_efficiency = -1
        v_out_ptp = -1
        v_out_min = -1
        v_out_max = -1

    if raw is None:  # If for some reason the raw fails, penalize the individual
        reward_efficiency = 0

    acceptable_ripple_ptp = Quantity(0.05, 'V')  # Volts
    if v_out_ptp > float(acceptable_ripple_ptp):
        reward_efficiency = reward_efficiency / (1 * (float(acceptable_ripple_ptp) - v_out_ptp))

    if v_out_min < 4 or v_out_max > 7:
        reward_efficiency = 0

    periods = pulse_period(individual[Genes.FREQUENCY.value], quantiphy.Quantity(8, 'ns'))

    # Log all the individual's traits, so they can be used to reconstruct an evolution graph
    individual[Genes.HISTORY.value].update({
        "SimOK": raw is not None,
        "RawFilename": Path(raw).name,
        "LogFilename": Path(log).name,
        "RawEfficiency": raw_efficiency,
        "AdjustedEfficiency": reward_efficiency,
        "VOutPtP": Quantity(v_out_ptp, 'V'),
        "VOutMin": Quantity(v_out_min, 'V'),
        "VOutMax": Quantity(v_out_max, 'V'),
        "PWM": {
            "Frequency": Quantity(individual[Genes.FREQUENCY.value], 'Hz'),
            "Period": Quantity(periods["Period"], 's'),
            "PeriodLS": Quantity(periods["LS"], 's'),
            "PeriodHS": Quantity(periods["HS"], 's'),
            "Deadtime": Quantity(int(quantiphy.Quantity(8, 'ns')), 'ns')
        }, "Inductors": [{
            "Index": individual[Genes.L1.value],
            "Value": Quantity(crs.indexed_coilcraft_inductor(individual[Genes.L1.value])["Inductance"], "H"),
            "PartNumber": crs.indexed_coilcraft_inductor(individual[Genes.L1.value])["PartNumber"]
        }, ], 'Capacitors': [{
            "Name": f"C{i}",
            "Index": individual[i],
            "Value": Quantity(mrs.indexed_murata_capacitor("", individual[i])["Capacitance"], "F"),
            "PartNumber": mrs.indexed_murata_capacitor("", individual[i])["PartNumber"]
        } for i in range(Genes.C1.value, Genes.C4.value + 1)]
    })

    return individual


creator.create("FitnessMax", base.Fitness, weights=(1.0,))  # Positive weight means the goal is to achieve a maximum, negative would be if the evaluate function returned an error and the GA would work to minimize error
creator.create("Individual", list, fitness=creator.FitnessMax)  # Create the individual definition which is just a list trying to achieve the goal specified above

toolbox = base.Toolbox()  # Create a toolbox which manages the specific parameters for evolution

# Attribute Generators (Genes)
toolbox.register("attr_history", dict, {})  # Attribute to describe the history of this individual, will contain parents, crosses, mutations, etc. also used to generate the history JSON file
toolbox.register("attr_frequency", random.randint, 100000, 3000000)  # Attribute to describe frequency, it will be generated using randint between 100kHz and 3MHz
toolbox.register("attr_inductance", random.randint, 0, crs.number_of_models())  # Attribute to describe inductance (by index), same as above but it uses uniform to generate an inductance
toolbox.register("attr_capacitance_1", random.randint, 0, mrs.number_of_models())  # Same as inductance
toolbox.register("attr_capacitance_2", random.randint, 0, mrs.number_of_models())  # Same as inductance
toolbox.register("attr_capacitance_3", random.randint, 0, mrs.number_of_models())  # Same as inductance
toolbox.register("attr_capacitance_4", random.randint, 0, mrs.number_of_models())  # Same as inductance

# Individual Structure Initializer
toolbox.register("individual", tools.initCycle, creator.Individual, (
    toolbox.attr_history,
    toolbox.attr_frequency,
    toolbox.attr_inductance,
    toolbox.attr_capacitance_1,
    toolbox.attr_capacitance_2,
    toolbox.attr_capacitance_3,
    toolbox.attr_capacitance_4))  # Register all the attributes and initialize all individuals for the first population

# Population Structure Initializer
toolbox.register("population", tools.initRepeat, list, toolbox.individual)  # Define the population to be a list of individuals

# Evolution Evaluation Metric
toolbox.register("evaluate",
                 evaluate_individual)  # Set evaluate up to use whatever evaluates the fitness of an individual, in this case, it works to maximize the return value of the passed function


def cxSpice(ind1, ind2, indpb=0.2):
    for i in range(Genes.FREQUENCY.value, Genes.C4.value):  # Offset by 1 to avoid crossing history
        if random.random() < indpb:
            ind1[Genes.HISTORY.value]['Cross'] = ind2[Genes.HISTORY.value]['Parent']
            ind2[Genes.HISTORY.value]['Cross'] = ind1[Genes.HISTORY.value]['Parent']
            ind1[Genes.HISTORY.value]['Crosses'].append(Genes(i).name)
            ind2[Genes.HISTORY.value]['Crosses'].append(Genes(i).name)
            ind1[i], ind2[i] = ind2[i], ind1[i]

    return ind1, ind2


# Crossover Operator
toolbox.register("mate", cxSpice)  # Used when crossing 2 individuals


def clamp(num, min_value, max_value):
    return max(min(num, max_value), min_value)


def mutate_frequency(current: int) -> int:
    """
    Defines how the frequency gene will be mutated.
    :param current: The current frequency (before mutation).
    :return: The new, mutated frequency.
    """
    min_freq, max_freq = 100E3, 3000E3
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
            new_frequency = mutate_frequency(individual[Genes.FREQUENCY.value])
            individual[0]['Mutations'].append({f'{Genes.FREQUENCY.name}': new_frequency - individual[Genes.FREQUENCY.value]})
            individual[Genes.FREQUENCY.value] = new_frequency
        elif option < 2 / 6:
            new_index = mutate_indexed(individual[Genes.L1.value], 0, crs.number_of_models())
            individual[0]['Mutations'].append({f'{Genes.L1.name}': new_index - individual[Genes.L1.value]})
            individual[Genes.L1.value] = new_index
        elif option < 3 / 6:
            new_index = mutate_indexed(individual[Genes.C1.value], 0, mrs.number_of_models())
            individual[0]['Mutations'].append({f'{Genes.C1.name}': new_index - individual[Genes.C1.value]})
            individual[Genes.C1.value] = new_index
        elif option < 4 / 6:
            new_index = mutate_indexed(individual[Genes.C2.value], 0, mrs.number_of_models())
            individual[0]['Mutations'].append({f'{Genes.C2.name}': new_index - individual[Genes.C2.value]})
            individual[Genes.C2.value] = new_index
        elif option < 5 / 6:
            new_index = mutate_indexed(individual[Genes.C3.value], 0, mrs.number_of_models())
            individual[0]['Mutations'].append({f'{Genes.C3.name}': new_index - individual[Genes.C3.value]})
            individual[Genes.C3.value] = new_index
        else:
            new_index = mutate_indexed(individual[Genes.C4.value], 0, mrs.number_of_models())
            individual[0]['Mutations'].append({f'{Genes.C4.name}': new_index - individual[Genes.C4.value]})
            individual[Genes.C4.value] = new_index
    return individual,


toolbox.register("mutate", mutate_netlist, indpb=0.6)  # This defines the function used to mutate an individual, along with the probability threshold used inside the passed function

toolbox.register("select", tools.selTournament, tournsize=3)  # How many of the fittest individuals will be pulled for the next generation


def init_core_netlist(global_netlist):
    """
    Initializes each process (core) with the global LTSpice netlist so that each process does not try to open and read the netlist at the same time.
    :param global_netlist: The global instance of the netlist, it will be shared among all the cores.
    """
    global netlist
    netlist = global_netlist


def main():
    top_netlist = SpiceEditor('LTFiles/EPC23102_Mine.asc')  # The first (and only) read of the netlist into memory so file access does not have to be managed among cores

    cpu_count = multiprocessing.cpu_count()  # Get a count of how many CPUs the current computer has
    # cpu_count = 5  # TODO: Doesn't work for # of individuals = 2
    logging.info(f"CPU count: {cpu_count}")
    pool = multiprocessing.Pool(cpu_count, initializer=init_core_netlist, initargs=(top_netlist,))  # Huge thanks to: https://gist.github.com/AvalZ/f019c9adbc15c505578b99041fb803d7
    toolbox.register("map", pool.map)  # Tell DEAP to use the multiprocessing cores

    pop = toolbox.population(n=cpu_count * 2)  # Give the evolutionary algorithm all the cores to run simulations on

    logging_ea_simple(pop, toolbox, cxpb=0.5, mutpb=0.8, ngen=40)  # Run the modified EA simple function which will log all the evolution changes
    best_ind = tools.selBest(pop, 1)[0]  # Pick the best individual
    logging.info(f"Best individual is {best_ind}, {best_ind.fitness.values}")

    pool.close()


if __name__ == "__main__":
    main()
