
from sys import stdin
from sys import exit

expected = open('expected_file_list','rt')

for line in stdin:
    line = line.strip()
    expected_line = next(expected).strip()

    if line != expected_line:
        exit(f"Expected file: {expected_line}, got {line} instead")
