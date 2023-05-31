# import logging
# from rich.logging import RichHandler
#
# FORMAT = "%(message)s"
# logging.basicConfig(
#     level="ERROR", format=FORMAT, datefmt="[%X]", handlers=[RichHandler()]
# )
#
# logging.getLogger("richmodule").level = logging.DEBUG
#
# import RichModuleTest
# log = logging.getLogger("rich")
# log.info("Hello, World!")

# imperial_red = '#F94144'
# coral = '#F9844A'

from rich.box import *
from rich.console import Console
from rich.progress_bar import ProgressBar
from rich.table import Table
from rich.tree import Tree

tree = Tree(f"Generation {1}")
stats = tree.add("Statistics")
stats.add(f"Population: {20}")
stats.add(f"Minimum: {15.8}%")
stats.add(f"Maximum: {98.4}%")
stats.add(f"Average: {52.7}%")
stats.add(f"Standard Deviation: {52.7}%")
summary = tree.add("Summary")

bar = ProgressBar(total=100, completed=73.8, complete_style='#008888')
table = Table(show_header=False, pad_edge=False, show_edge=False, show_lines=False, show_footer=False, box=None)
table.add_row(f"Individual {1}:", bar, f"{73.8}%")
summary.add(table)

ind_brk = tree.add("Individual Breakdown")
ind_1 = ind_brk.add(f"Individual {1}:")
ind_1.add(f"File Name: {'Hello.txt'}")
ind_1_param = ind_1.add(f"Parameters")
table = Table(show_footer=False, box=SIMPLE_HEAD)
table.add_column('[red]Frequency')
table.add_column('[red]Deadtime')
table.add_column('[red]Period')
table.add_row(f"{500}kHz", f"{8205}ns", f"{8}ns")
ind_1_param.add(table)

ind_1_comp = ind_1.add(f"Components")
table = Table(show_footer=False, box=SIMPLE_HEAD)
table.add_column('[red]Component')
table.add_column('[red]Index')
table.add_column('[red]Value')
table.add_column('[red]Part #')
table.add_row(f"Inductor ", f"{145}", f"{865}µH", f"{'XAL4080-228'}")
table.add_row(f"Capacitor 1 ", f"{26348}", f"{10}nF", f"{'GRM1555C2AR10WA01'}")
table.add_row(f"Capacitor 2 ", f"{2634}", f"{865}nF", f"{'GRM1555C1H4R8WA01'}")
table.add_row(f"Capacitor 3 ", f"{263}", f"{2.2}µF", f"{'GRM32ED70J476ME20'}")
table.add_row(f"Capacitor 4 ", f"{26}", f"{10}µF", f"{'GRM188R11E223MA01'}")
ind_1_comp.add(table)

ind_1_perf = ind_1.add(f"Performance")
table = Table(show_footer=False, box=SIMPLE_HEAD)
table.add_column('[red]Efficiency')
table.add_column('[red]Minimum')
table.add_column('[red]Maximum')
table.add_column('[red]Peak to Peak')
table.add_row(f"{73.8}%", f"{3.84}V", f"{4.98}V", f"{2.05}V")
ind_1_perf.add(table)

console = Console()
console.print(tree)

# table = Table.grid()
# table.add_column()
# table.add_row(f"Individual {3}: ", bar, f"{53}%")  # For some reason this causes the bar to extend into the percentage

# with console.status("[blue]Starting Generation 1 Simulations", spinner="dots12", spinner_style='bold blue') as status:
#     sleep(4)
# console.print("[bold green]Finished Simulating Generation 1 :white_check_mark:")

# Example of making a custom pulse progress bar
# columns = (TextColumn("[progress.description]{task.description}"),
#             BarColumn(pulse_style='blue', complete_style='green'),
#             TaskProgressColumn(),
#             TimeRemainingColumn())
#
# with Progress(
#         SpinnerColumn(),
#         *columns,
#         TimeElapsedColumn(),
#         console=console,
#         transient=False,
# ) as progress:
#     n = 0
#     task3 = progress.add_task("[yellow]Thinking", total=None)
#     while n < 5:
#         # progress.update(task3, )
#         n += 1
#         time.sleep(1)
#     # progress.remove_task(task3)
#     progress.update(task3, completed=True)
