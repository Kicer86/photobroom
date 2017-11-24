
from sys import argv
from sys import exit
import getopt
import os.path
import shutil


def read_packages(location):
    from os import listdir
    from os.path import isfile, join

    available_packages = {}

    # load packages
    entries = listdir(location)
    for entry in entries:
        full_path = join(location, entry)
        if entry[-3:] == ".py" and isfile(full_path):
            file_content = open(full_path).read()
            exec(file_content)

    return available_packages


def usage(avail_packages):

    name = os.path.basename(argv[0])

    print("Usage:")
    print(name, "[options]", "destination dir")
    print("")
    print("Possible options:")
    print("-p <name>      Download nad install package.")
    print("               Possible packages:")
    print("               " + ", ".join(avail_packages))
    print("               Option can be reapeted.")


def is_exe(path):
    return os.path.isfile(path) and os.access(path, os.X_OK)


def main(argv):

    # read available packages
    packages = read_packages("./dependencies")
    packages_names = packages.keys()

    # no arguments? Show usage
    if len(argv) == 1:
        usage(packages_names)
        exit()

    cmake = shutil.which('cmake')
    libraries = []
    destination_dir = ""

    # parse arguments
    try:
        opts, args = getopt.getopt(argv[1:], "hp:", ["help"])
    except getopt.GetoptError:
        print("Invalid arguments provided")
        print("See -h for help.")
        exit(2)

    # collect data
    for opt, arg in opts:
        if opt in ['-h', '--help']:
            usage(packages_names)
            exit()
        elif opt == "-p":
            if (arg in packages_names):
                libraries.append(arg)
            else:
                print("'" + arg + "' is not valid package name.")
                print("See -h for help.")
                exit(2)

    # we expect one argument (desitnation dir) in args
    if len(args) == 0:
        print("No destination dir was provided.")
        print("See -h for help.")
        exit(2)

    if len(args) > 1:
        print("Too many desination dirs. Only one is expected.")
        print("See -h for help.")
        exit(2)

    destination_dir = args[0]

    # verify
    if cmake is None or is_exe(cmake) == False:
        print("No valid path to 'cmake' was provided.")
        print("See -h for help.")
        exit(2)


if __name__ == "__main__":
   main(argv[0:])
