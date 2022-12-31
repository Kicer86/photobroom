
import sys

if len(sys.argv) < 3:
    raise ValueError('Not enought input parameters')

input_file_name = sys.argv[1]
output_file_name = sys.argv[2]

with open(input_file_name, 'r') as input_file:
    file_names = input_file.read().splitlines()

with open(output_file_name, 'w') as output_file:
    for file_name in file_names:
        output_file.write(f'#include "{file_name}"\n')
