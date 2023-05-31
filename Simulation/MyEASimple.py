import time

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
    logbook = tools.Logbook()
    logbook.header = ['gen', 'nevals'] + (stats.fields if stats else [])

    all_fitness_values = {}

    # Evaluate the individuals with an invalid fitness
    invalid_ind = [ind for ind in population if not ind.fitness.valid]
    all_fitness_values = toolbox.map(toolbox.evaluate, invalid_ind)
    gen_console.rule(f"Generation {0}")
    PrintGeneration(0, all_fitness_values, gen_console)
    fitnesses = [[fit["Efficiency"], ] for fit in all_fitness_values]
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
            gen_console.rule(f"Generation {gen}")
            gen_console.print(f"[bold green]Finished Simulating Generation {gen} :white_check_mark:")
        PrintGeneration(gen, all_fitness_values, gen_console)
        fitnesses = [[fit["Efficiency"], ] for fit in all_fitness_values]
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


def PrintGeneration(gen: int, data: dict, cons: rich.console):
    tree = Tree(f"Generation {gen}")
    stats = tree.add("Statistics")
    stats.add(f"Population: {len(data)}")
    stats.add(f"Minimum: {np.min([i['Efficiency'] for i in data]):05.2f}%")
    stats.add(f"Maximum: {np.max([i['Efficiency'] for i in data]):05.2f}%")
    stats.add(f"Average: {np.average([i['Efficiency'] for i in data]):05.2f}%")
    stats.add(f"Standard Deviation: {np.std([i['Efficiency'] for i in data]):05.2f}")
    summary = tree.add("Summary")

    for count, ind in enumerate(data):
        bar = ProgressBar(total=100, completed=ind["Efficiency"], complete_style='#008888')
        table = Table(show_header=False, pad_edge=False, show_edge=False, show_lines=False, show_footer=False, box=None)
        table.add_row(f"Individual {count}:", bar, f"{ind['Efficiency']:05.2f} %")
        summary.add(table)

    ind_brk = tree.add("Individual Breakdown")
    ind_brk_dict = {}
    for count, ind in enumerate(data):
        ind_brk_dict[f"ind_{count}"] = ind_brk.add(f"Individual {count}:")
        ind_brk_dict[f"ind_{count}"].add(f"File Name: {ind['Raw']}")
        ind_brk_dict[f"ind_{count}_param"] = ind_brk_dict[f"ind_{count}"].add(f"Parameters")
        ind_brk_dict[f"ind_{count}_table"] = Table(show_footer=False, box=SIMPLE_HEAD)
        ind_brk_dict[f"ind_{count}_table"].add_column('[red]Frequency')
        ind_brk_dict[f"ind_{count}_table"].add_column('[red]Deadtime')
        ind_brk_dict[f"ind_{count}_table"].add_column('[red]Period')
        ind_brk_dict[f"ind_{count}_table"].add_column('[red]Period HS')
        ind_brk_dict[f"ind_{count}_table"].add_column('[red]Period LS')
        ind_brk_dict[f"ind_{count}_table"].add_row(f"{ind['Freq']['Frequency']}", f"{ind['Freq']['Deadtime']}", f"{ind['Freq']['Period']}", f"{ind['Freq']['PeriodHS']}", f"{ind['Freq']['PeriodLS']}")
        ind_brk_dict[f"ind_{count}_param"].add(ind_brk_dict[f"ind_{count}_table"])

        ind_brk_dict[f"ind_{count}_comp"] = ind_brk_dict[f"ind_{count}"].add(f"Components")
        ind_brk_dict[f"ind_{count}_comp_table"] = Table(show_footer=False, box=SIMPLE_HEAD)
        ind_brk_dict[f"ind_{count}_comp_table"].add_column('[red]Component')
        ind_brk_dict[f"ind_{count}_comp_table"].add_column('[red]Index')
        ind_brk_dict[f"ind_{count}_comp_table"].add_column('[red]Value')
        ind_brk_dict[f"ind_{count}_comp_table"].add_column('[red]Part #')
        ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Inductor ", f"{ind['L']['Index']}", f"{ind['L']['Value']}", f"{ind['L']['PartNumber']}")
        ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Capacitor 1 ", f"{ind['C1']['Index']}", f"{ind['C1']['Value']}", f"{ind['C1']['PartNumber']}")
        ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Capacitor 2 ", f"{ind['C2']['Index']}", f"{ind['C2']['Value']}", f"{ind['C2']['PartNumber']}")
        ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Capacitor 3 ", f"{ind['C3']['Index']}", f"{ind['C3']['Value']}", f"{ind['C3']['PartNumber']}")
        ind_brk_dict[f"ind_{count}_comp_table"].add_row(f"Capacitor 4 ", f"{ind['C4']['Index']}", f"{ind['C4']['Value']}", f"{ind['C4']['PartNumber']}")
        ind_brk_dict[f"ind_{count}_comp"].add(ind_brk_dict[f"ind_{count}_comp_table"])

        ind_brk_dict[f"ind_{count}_perf"] = ind_brk_dict[f"ind_{count}"].add(f"Performance")
        ind_brk_dict[f"ind_{count}_perf_table"] = Table(show_footer=False, box=SIMPLE_HEAD)
        ind_brk_dict[f"ind_{count}_perf_table"].add_column('[red]Efficiency')
        ind_brk_dict[f"ind_{count}_perf_table"].add_column('[red]Minimum')
        ind_brk_dict[f"ind_{count}_perf_table"].add_column('[red]Maximum')
        ind_brk_dict[f"ind_{count}_perf_table"].add_column('[red]Peak to Peak')
        ind_brk_dict[f"ind_{count}_perf_table"].add_row(f"{ind['Efficiency']:05.2f} %", f"{ind['VOutMin']}", f"{ind['VOutMax']}", f"{ind['VOutPtP']}")
        ind_brk_dict[f"ind_{count}_perf"].add(ind_brk_dict[f"ind_{count}_perf_table"])

    cons.print(tree)
