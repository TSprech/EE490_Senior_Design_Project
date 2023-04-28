from pprint import pprint
from pathlib import Path
import re
import pandas as pd

# with open('LTFiles/all_inductors.lib') as inductors:
#     re.findall(r"(?s)(?=\*)(.*?)(?=\.subckt)(.*?)(?<=\.ends)(.*?)(?=\n)", inductors.rea, flags=re.S)

# with open('LTFiles/subset_inductors.lib') as inductors:

def process_coilcraft_inductor_lib(filename: str):
    data_str = Path(filename).read_text()
    data = re.findall(r"(?s)(?=\*)(.*?)(?=\.subckt)(.*?)(?<=\.ends)(.*?)(?=\n)", data_str, flags=re.S)  # This expression extracts each subckt 'clump'
    data = [''.join(x) for x in data]  # This takes all the groups that are generated in the above step and merges them into one string

    data = [x for x in data if '_freq' not in x]  # Remove any of the frequency simulations since those are for frequency domain analysis and we are doing time

    # pprint(data)
    ret_data = []
    inductance_regex = r"(?<=Inductance = )(.*?)(?=\n)"
    part_number_regex = r"(?<=Coilcraft Part Number : )(.*?)(?=\n)"
    for inductor in data:
        try:
            ret_data.append({"PartNumber": re.findall(part_number_regex, inductor)[0], "Inductance": re.findall(inductance_regex, inductor)[0], "SubCkt": inductor.replace('\n', ';')})
            # ret_data.append({"PartNumber": re.findall(part_number_regex, inductor)[0], "Inductance": re.findall(inductance_regex, inductor)[0], "SubCkt": inductor})
        except:
            try:
                ret_data.append({"PartNumber": "MODEL",
                                 "Inductance": 0,
                                 "SubCkt": inductor.replace('\n', ';')})
            except:
                continue

    return ret_data

coilcraft_inductors = process_coilcraft_inductor_lib('LTFiles/all_inductors.lib')
pprint(coilcraft_inductors)
df = pd.DataFrame(coilcraft_inductors).drop_duplicates()
df.to_csv('ProcessedInductors.csv')