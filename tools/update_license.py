
import os
import sys


def parse_file(path):
    with open(path) as source_file:
        line = source_file.readline()

        if line and not line.strip():
            line = source_file.readline()

        if line and line.strip() != "/*":
            return

        line = source_file.readline()

        if line.strip().startswith("* Copyright (C)"):
            content = line + source_file.read()
        else:
            content = source_file.read()

        new_file_name = path + ".new"
        with open(new_file_name, "w") as new_file:
            new_file.write("/*\n")
            new_file.write(" * Photo Broom - photos management tool.\n")
            new_file.write(content)

            os.rename(new_file_name, path)

if len(sys.argv) < 2:
    print("pass working dir as parameter")
    sys.exit(1)

directory = sys.argv[1]

for root, dirs, files in os.walk(directory):
    for file in files:
        if file.endswith('.hpp') or file.endswith('.cpp'):
            full_path = root + "/" + file
            print(full_path)
            parse_file(full_path)
