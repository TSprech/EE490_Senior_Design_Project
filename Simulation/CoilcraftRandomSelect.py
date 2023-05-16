import random

import pandas as pd

inductors = pd.read_csv('CoilcraftInductors.csv')  # Get all the inductors from the file


def random_coilcraft_inductor():
    index = random.randint(0, len(inductors))  # Pick a random index
    selected_inductor = inductors.iloc[index]  # Select the inductor at that index
    # First commonize the part number to Inductor to make it easy for the computer to put in the simulation
    # Then remove teh extra _imp or _sat which is appended to the subckt names
    # Then undo the \n→; conversion done when encoding to csv
    subckt = selected_inductor['SubCkt'] \
        .replace(selected_inductor['PartNumber'], 'Inductor') \
        .replace('_imp', '') \
        .replace('_sat', '') \
        .replace(';', '\n')
    return {'PartNumber': selected_inductor['PartNumber'], 'SubCkt': subckt}