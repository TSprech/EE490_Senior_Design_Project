import colorsys
import time
import json
from pathlib import Path

import numpy as np
import rich
from deap import tools
from deap.algorithms import varAnd
from rich.box import *
from rich.console import Console
from rich.progress_bar import ProgressBar
from rich.table import Table
from rich.tree import Tree

console = Console()


def my_ea_simple(population, toolbox, cxpb, mutpb, ngen, stats=None, halloffame=None, verbose=__debug__):
    """This algorithm reproduce the simplest evolutionary algorithm as
    presented in chapter 7 of [Back2000]_.

    :param population: A list of individuals.
    :param toolbox: A :class:`~deap.base.Toolbox` that contains the evolution
                    operators.
    :param cxpb: The probability of mating two individuals.
    :param mutpb: The probability of mutating an individual.
    :param ngen: The number of generation.
    :param stats: A :class:`~deap.tools.Statistics` object that is updated
                  inplace, optional.
    :param halloffame: A :class:`~deap.tools.HallOfFame` object that will
                       contain the best individuals, optional.
    :param verbose: Whether or not to log the statistics.
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
        logbook = tools.Logbook()
        logbook.header = ['gen', 'nevals'] + (stats.fields if stats else [])

        all_fitness_values = {}

        # Evaluate the individuals with an invalid fitness
        invalid_ind = [ind for ind in population if not ind.fitness.valid]
        all_fitness_values = toolbox.map(toolbox.evaluate, invalid_ind)
        gen_console.rule(f"Generation {0}")
        PrintGeneration(0, all_fitness_values, gen_console)
        fitnesses = [[fit["AdjustedEfficiency"], ] for fit in all_fitness_values]
        for ind, fit in zip(invalid_ind, fitnesses):
            ind.fitness.values = fit

        if halloffame is not None:
            halloffame.update(population)

        record = stats.compile(population) if stats else {}
        logbook.record(gen=0, nevals=len(invalid_ind), **record)
        if verbose:
            print(logbook.stream)

        # Begin the generational process
        for gen in range(1, ngen + 1):
            # Select the next generation individuals
            offspring = toolbox.select(population, len(population))

            # Vary the pool of individuals
            offspring = varAnd(offspring, toolbox, cxpb, mutpb)

            # Evaluate the individuals with an invalid fitness
            invalid_ind = [ind for ind in offspring if not ind.fitness.valid]
            with gen_console.status(f"[blue]Starting Generation {gen} Simulations", spinner="dots12", spinner_style='bold blue'):
                time.sleep(1)
                all_fitness_values = toolbox.map(toolbox.evaluate, invalid_ind)  # Make evaluate return a much more detailed summary object
                f.write(json.dumps(all_fitness_values))
                gen_console.rule(f"Generation {gen}")
                gen_console.print(f"[bold green]Finished Simulating Generation {gen} :white_check_mark:")
            PrintGeneration(gen, all_fitness_values, gen_console)

            fitnesses = [[fit["AdjustedEfficiency"], ] for fit in all_fitness_values]
            for ind, fit in zip(invalid_ind, fitnesses):  # Then just use what is needed in this part
                ind.fitness.values = fit

            # Update the hall of fame with the generated individuals
            if halloffame is not None:
                halloffame.update(offspring)

            # Replace the current population by the offspring
            population[:] = offspring

            # Append the current generation statistics to the logbook
            record = stats.compile(population) if stats else {}
            logbook.record(gen=gen, nevals=len(invalid_ind), **record)
            if verbose:
                print(logbook.stream)

        gen_console.save_html('HTMLOUT.html')

    return population, logbook


def hsv2rgb(h,s,v):
    return tuple(round(i * 255) for i in colorsys.hsv_to_rgb(h,s,v))

def rgb_to_hex(r, g, b):
    return '#{:02x}{:02x}{:02x}'.format(r, g, b).upper()

def PrintGeneration(gen: int, data: dict, cons: rich.console):
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
        table.add_row(f"Individual {count}:", bar, f"{ind['AdjustedEfficiency']:05.2f} %")
        summary.add(table)

    ind_brk = tree.add("Individual Breakdown")
    ind_brk_dict = {}
    for count, ind in enumerate(data):
        eff_color = min(max(ind["AdjustedEfficiency"] * 100 * 0.88 / 255, 0), 1)
        r, g, b = hsv2rgb(eff_color, 0.8, 0.8)
        ind_color = rgb_to_hex(r, g, b)

        ind_brk_dict[f"ind_{count}"] = ind_brk.add(f"Individual {count}:")
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
        ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Inductor ", f"{ind['Inductors'][0]['Index']}", f"{ind['Inductors'][0]['Value']}", f"{ind['Inductors'][0]['PartNumber']}")
        ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Capacitor 1 ", f"{ind['Capacitors'][0]['Index']}", f"{ind['Capacitors'][0]['Value']}", f"{ind['Capacitors'][0]['PartNumber']}")
        ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Capacitor 2 ", f"{ind['Capacitors'][1]['Index']}", f"{ind['Capacitors'][1]['Value']}", f"{ind['Capacitors'][1]['PartNumber']}")
        ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Capacitor 3 ", f"{ind['Capacitors'][2]['Index']}", f"{ind['Capacitors'][2]['Value']}", f"{ind['Capacitors'][2]['PartNumber']}")
        ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Capacitor 4 ", f"{ind['Capacitors'][3]['Index']}", f"{ind['Capacitors'][3]['Value']}", f"{ind['Capacitors'][3]['PartNumber']}")
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
