import random

import numpy as np
import pandas as pd

inductors = pd.read_csv('CoilcraftInductors.csv')  # Get all the inductors from the file


def number_of_models():
    return len(inductors) - 1  # -1 to account for the zero indexing


def indexed_coilcraft_inductor(index: int):
    selected_inductor = inductors.iloc[index]  # Select the inductor at that index
    # First commonize the part number to Inductor to make it easy for the computer to put in the simulation
    # Then remove teh extra _imp or _sat which is appended to the subckt names
    # Then undo the \n→; conversion done when encoding to csv
    subckt = selected_inductor['SubCkt'] \
        .replace(selected_inductor['PartNumber'], 'Inductor') \
        .replace('_imp', '') \
        .replace('_sat', '') \
        .replace(';', '\n')

    return {'PartNumber': selected_inductor['PartNumber'], 'Inductance': selected_inductor['Inductance'], 'SubCkt': subckt}


def find_nearest(array, value: float):  # Thanks: https://stackoverflow.com/a/2566508
    array = np.asarray(array)
    idx = (np.abs(array - value)).argmin()
    return array[idx]


def closest_coilcraft_inductor(value: float):
    required_inductor = find_nearest(inductors.Inductance, value)
    return inductors.loc[inductors.Inductance == required_inductor]  # https://stackoverflow.com/a/17071908


def random_coilcraft_inductor():
    indexed_coilcraft_inductor(random.randint(0, len(inductors)))
