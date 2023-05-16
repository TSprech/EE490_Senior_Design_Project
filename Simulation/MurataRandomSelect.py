import random

import pandas as pd

capacitors = pd.read_csv('MurataCapacitors.csv')  # Get all the capacitors from the file


def random_murata_capacitor(name: str):
    index = random.randint(0, len(capacitors))  # Pick a random index
    selected_capacitor = capacitors.iloc[index]  # Select the capacitor at that index
    # First commonize the part number to Inductor to make it easy for the computer to put in the simulation
    # Then remove teh extra _imp or _sat which is appended to the subckt names
    # Then undo the \n→; conversion done when encoding to csv
    subckt = selected_capacitor['SubCkt'] \
        .replace(selected_capacitor['PartNumber'], name) \
        .replace(';', '\n')
    return {'PartNumber': selected_capacitor['PartNumber'], 'SubCkt': subckt}
