import colorsys
import copy
import json
from enum import Enum
from pathlib import Path

import numpy as np
import rich
from deap.algorithms import varAnd
from rich.box import *
from rich.console import Console
from rich.progress_bar import ProgressBar
from rich.table import Table
from rich.tree import Tree

console = Console()


class Genes(Enum):
    HISTORY = 0
    FREQUENCY = 1
    L1 = 2
    C1 = 3
    C2 = 4
    C3 = 5
    C4 = 6


def logging_ea_simple(population, toolbox, cxpb, mutpb, ngen):
    """This algorithm reproduce the simplest evolutionary algorithm as
    presented in chapter 7 of [Back2000]_.

    :param population: A list of individuals.
    :param toolbox: A :class:`~deap.base.Toolbox` that contains the evolution
                    operators.
    :param cxpb: The probability of mating two individuals.
    :param mutpb: The probability of mutating an individual.
    :param ngen: The number of generation.

    :returns: The final population
    :returns: A class:`~deap.tools.Logbook` with the statistics of the
              evolution

    The algorithm takes in a population and evolves it in place using the
    :meth:`varAnd` method. It returns the optimized population and a
    :class:`~deap.tools.Logbook` with the statistics of the evolution. The
    logbook will contain the generation number, the number of evaluations for
    each generation and the statistics if a :class:`~deap.tools.Statistics` is
    given as argument. The *cxpb* and *mutpb* arguments are passed to the
    :func:`varAnd` function. The pseudocode goes as follow ::

        evaluate(population)
        for g in range(ngen):
            population = select(population, len(population))
            offspring = varAnd(population, toolbox, cxpb, mutpb)
            evaluate(offspring)
            population = offspring

    As stated in the pseudocode above, the algorithm goes as follow. First, it
    evaluates the individuals with an invalid fitness. Second, it enters the
    generational loop where the selection procedure is applied to entirely
    replace the parental population. The 1:1 replacement ratio of this
    algorithm **requires** the selection procedure to be stochastic and to
    select multiple times the same individual, for example,
    :func:`~deap.tools.selTournament` and :func:`~deap.tools.selRoulette`.
    Third, it applies the :func:`varAnd` function to produce the next
    generation population. Fourth, it evaluates the new individuals and
    compute the statistics on this population. Finally, when *ngen*
    generations are done, the algorithm returns a tuple with the final
    population and a :class:`~deap.tools.Logbook` of the evolution.

    .. note::

        Using a non-stochastic selection method will result in no selection as
        the operator selects *n* individuals from a pool of *n*.

    This function expects the :meth:`toolbox.mate`, :meth:`toolbox.mutate`,
    :meth:`toolbox.select` and :meth:`toolbox.evaluate` aliases to be
    registered in the toolbox.

    .. [Back2000] Back, Fogel and Michalewicz, "Evolutionary Computation 1 :
       Basic Algorithms and Operators", 2000.
    """
    gen_console = Console(record=True)
    with open("outfile.json", 'w') as f:

        generation_data = []

        # Evaluate the individuals with an invalid fitness
        invalid_ind = [ind for ind in population if not ind.fitness.valid]
        for count, ind in enumerate(population):
            ind[Genes.HISTORY.value]['Mutations'] = []
            ind[Genes.HISTORY.value]['Parent'] = f'Origin'
            ind[Genes.HISTORY.value]['Name'] = f'G{0}I{count}'
            ind[Genes.HISTORY.value]['Crosses'] = []
            ind[Genes.HISTORY.value]['Cross'] = ''
        all_individuals = toolbox.map(toolbox.evaluate, invalid_ind)
        gen_console.rule(f"Generation {0}")
        PrintGeneration(0, all_individuals, gen_console)
        for ind in all_individuals:
            generation_data.append(ind[Genes.HISTORY.value])

        fitnesses = [[fit[Genes.HISTORY.value]["AdjustedEfficiency"], ] for fit in all_individuals]
        for ind, fit in zip(invalid_ind, fitnesses):
            ind.fitness.values = fit

        # Begin the generational process
        for gen in range(1, ngen + 1):
            # Select the next generation individuals
            offspring = []
            for count, ind in enumerate(toolbox.select(population, len(population))):
                new_offspring = copy.deepcopy(ind)
                new_offspring[Genes.HISTORY.value]['Mutations'] = []
                new_offspring[Genes.HISTORY.value]['Parent'] = ind[Genes.HISTORY.value]['Name']
                new_offspring[Genes.HISTORY.value]['Name'] = f'G{gen}I{count}'
                ind[Genes.HISTORY.value]['Crosses'] = []
                new_offspring[Genes.HISTORY.value]['Crosses'] = []
                new_offspring[Genes.HISTORY.value]['Cross'] = ''
                offspring.append(new_offspring)

            offspring = varAnd(offspring, toolbox, cxpb, mutpb)  # Apply mutations and crosses to vary the pool

            gen_console.print(f"[magenta]Starting Generation {gen} Simulations :brain:")
            all_individuals = toolbox.map(toolbox.evaluate, offspring)  # Start simulating and generating the new fitness values
            gen_console.print(f"[bold green]Finished Simulating Generation {gen} :white_check_mark:")
            gen_console.rule(f"Generation {gen} Summary")
            PrintGeneration(gen, all_individuals, gen_console)
            for ind in all_individuals:
                generation_data.append(ind[Genes.HISTORY.value])

            fitnesses = [[fit[Genes.HISTORY.value]["AdjustedEfficiency"], ] for fit in all_individuals]  # Extract the fitness value for each individual
            for ind, fit in zip(all_individuals, fitnesses):  # Then just use the extracted fitness value to score each individual
                ind.fitness.values = fit

            population[:] = offspring  # Replace the current population by the offspring

        gen_console.save_html('HTMLOUT.html')
        f.write(json.dumps(generation_data))

    return population


def hsv2rgb(h, s, v):
    return tuple(round(i * 255) for i in colorsys.hsv_to_rgb(h, s, v))


def rgb_to_hex(r, g, b):
    return '#{:02x}{:02x}{:02x}'.format(r, g, b).upper()


def PrintGeneration(gen: int, ind_data: dict, cons: rich.console):
    data = [i[0] for i in ind_data]
    tree = Tree(f"Generation {gen}")
    stats = tree.add("Statistics")
    stats.add(f"Population: {len(data)}")
    stats.add(f"Minimum: {np.min([i['AdjustedEfficiency'] for i in data]):05.2f}%")
    stats.add(f"Maximum: {np.max([i['AdjustedEfficiency'] for i in data]):05.2f}%")
    stats.add(f"Average: {np.average([i['AdjustedEfficiency'] for i in data]):05.2f}%")
    stats.add(f"Standard Deviation: {np.std([i['AdjustedEfficiency'] for i in data]):05.2f}")
    summary = tree.add("Summary")

    for count, ind in enumerate(data):
        eff_color = min(max(ind["AdjustedEfficiency"] * 100 * 0.88 / 255, 0), 1)
        r, g, b = hsv2rgb(eff_color, 0.8, 0.8)
        ind_color = rgb_to_hex(r, g, b)

        bar = ProgressBar(total=100, completed=ind["AdjustedEfficiency"] * 100, complete_style=f'{ind_color}')
        table = Table(show_header=False, pad_edge=False, show_edge=False, show_lines=False, show_footer=False, box=None)
        table.add_row(f"Individual {count:2}:", bar, f"{ind['AdjustedEfficiency']*100:05.2f} %")
        summary.add(table)

    ind_brk = tree.add("Individual Breakdown")
    ind_brk_dict = {}
    for count, ind in enumerate(data):
        eff_color = min(max(ind["AdjustedEfficiency"] * 100 * 0.88 / 255, 0), 1)
        r, g, b = hsv2rgb(eff_color, 0.8, 0.8)
        ind_color = rgb_to_hex(r, g, b)

        ind_brk_dict[f"ind_{count}"] = ind_brk.add(f"Individual {ind['Name']}:")
        try:
            ind_brk_dict[f"ind_{count}"] = ind_brk.add(f"Parent {ind['Parent']}:")
        except:
            ind_brk_dict[f"ind_{count}"] = ind_brk.add(f"Parent None")
        ind_brk_dict[f"ind_{count}"].add(f"File Name: {Path(ind['RawFilename']).stem}")
        ind_brk_dict[f"ind_{count}_param"] = ind_brk_dict[f"ind_{count}"].add(f"Parameters")
        ind_brk_dict[f"ind_{count}_table"] = Table(show_footer=False, box=SIMPLE_HEAD)
        ind_brk_dict[f"ind_{count}_table"].add_column(f'[{ind_color}]Frequency')
        ind_brk_dict[f"ind_{count}_table"].add_column(f'[{ind_color}]Deadtime')
        ind_brk_dict[f"ind_{count}_table"].add_column(f'[{ind_color}]Period')
        ind_brk_dict[f"ind_{count}_table"].add_column(f'[{ind_color}]Period HS')
        ind_brk_dict[f"ind_{count}_table"].add_column(f'[{ind_color}]Period LS')
        ind_brk_dict[f"ind_{count}_table"].add_row(f"{ind['PWM']['Frequency']}", f"{ind['PWM']['Deadtime']}", f"{ind['PWM']['Period']}", f"{ind['PWM']['PeriodHS']}", f"{ind['PWM']['PeriodLS']}")
        ind_brk_dict[f"ind_{count}_param"].add(ind_brk_dict[f"ind_{count}_table"])

        ind_brk_dict[f"ind_{count}_comp"] = ind_brk_dict[f"ind_{count}"].add(f"Components")
        ind_brk_dict[f"ind_{count}_comp_table"] = Table(show_footer=False, box=SIMPLE_HEAD)

        ind_brk_dict[f"ind_{count}_comp_table"].add_column(f'[{ind_color}]Component')
        ind_brk_dict[f"ind_{count}_comp_table"].add_column(f'[{ind_color}]Index')
        ind_brk_dict[f"ind_{count}_comp_table"].add_column(f'[{ind_color}]Value')
        ind_brk_dict[f"ind_{count}_comp_table"].add_column(f'[{ind_color}]Part #')
        for index, inductor in enumerate(ind['Inductors']):
            ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Inductor ", f"{inductor['Index']}", f"{inductor['Value']}", f"{inductor['PartNumber']}")
        for index, capacitor in enumerate(ind['Capacitors']):
            ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Capacitor {index} ", f"{capacitor['Index']}", f"{capacitor['Value']}", f"{capacitor['PartNumber']}")
        ind_brk_dict[f"ind_{count}_comp"].add(ind_brk_dict[f"ind_{count}_comp_table"])

        ind_brk_dict[f"ind_{count}_perf"] = ind_brk_dict[f"ind_{count}"].add(f"Performance")
        ind_brk_dict[f"ind_{count}_perf_table"] = Table(show_footer=False, box=SIMPLE_HEAD)
        ind_brk_dict[f"ind_{count}_perf_table"].add_column(f'[{ind_color}]AdjustedEfficiency')
        ind_brk_dict[f"ind_{count}_perf_table"].add_column(f'[{ind_color}]Minimum')
        ind_brk_dict[f"ind_{count}_perf_table"].add_column(f'[{ind_color}]Maximum')
        ind_brk_dict[f"ind_{count}_perf_table"].add_column(f'[{ind_color}]Peak to Peak')
        ind_brk_dict[f"ind_{count}_perf_table"].add_row(f"{ind['AdjustedEfficiency'] * 100:05.2f} %", f"{ind['VOutMin']}", f"{ind['VOutMax']}", f"{ind['VOutPtP']}")
        ind_brk_dict[f"ind_{count}_perf"].add(ind_brk_dict[f"ind_{count}_perf_table"])

    cons.print(tree)

    # for i in range(0, 101):
    #     eff_color = min(max(i * 0.88 / 255, 0), 1)
    #     r, g, b = hsv2rgb(eff_color, 0.8, 0.8)
    #     ind_color = rgb_to_hex(r, g, b)
    #     console.log(f'[{ind_color}]Test text here!')
