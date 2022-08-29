
from sys import stdin

def isDelimiter(line: str):
    return line.strip() == "------------------- ----- ------------ ------------  ------------------------"

for line in stdin:
    if isDelimiter(line):
        break

for line in stdin:
    if isDelimiter(line):
        break

    print(line[53:].rstrip())
