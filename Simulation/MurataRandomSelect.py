import random

import numpy as np
import pandas as pd

capacitors = pd.read_csv('MurataCapacitors.csv')  # Get all the capacitors from the file


def indexed_murata_capacitor(name: str, index: int):
    selected_capacitor = capacitors.iloc[index]  # Select the capacitor at that index
    # First commonize the part number to Inductor to make it easy for the computer to put in the simulation
    # Then remove teh extra _imp or _sat which is appended to the subckt names
    # Then undo the \n→; conversion done when encoding to csv
    subckt = selected_capacitor['SubCkt'] \
        .replace(selected_capacitor['PartNumber'], name) \
        .replace(';', '\n')
    return {'PartNumber': selected_capacitor['PartNumber'], 'Capacitance': selected_capacitor['Capacitance'], 'SubCkt': subckt}


def find_nearest(array, value: float):  # Thanks: https://stackoverflow.com/a/2566508
    array = np.asarray(array)
    idx = (np.abs(array - value)).argmin()
    return array[idx]


def closest_murata_capacitor(value: float):
    required_capacitor = find_nearest(capacitors.Capacitance, value)
    return capacitors.loc[capacitors.Capacitance == required_capacitor]  # https://stackoverflow.com/a/17071908


def random_murata_capacitor(name: str):
    indexed_murata_capacitor(name, random.randint(0, len(capacitors)))
