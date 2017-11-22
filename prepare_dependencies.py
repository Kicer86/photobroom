
from sys import argv
from sys import exit
import getopt
import os.path
import shutil


def usage():

    name = os.path.basename(argv[0])

    print("Usage:")
    print(name, "[options]", "destination dir")
    print("")
    print("Possible options:")
    print("-p <name>      Download nad install package.")
    print("               Possible packages:")
    print("               exiv2, jsoncpp, openlibrary")
    print("               Option can be reapeted.")
    print("")
    print("-g <generator> CMake generator. See cmake --help for possible options.")
    print("")
    print("-c <cmake>     Path to cmake. Useful when 'cmake' is not in PATH.")

def is_exe(path):
    return os.path.isfile(path) and os.access(path, os.X_OK)


def main(argv):

    cmake = shutil.which('cmake')
    generator = ""
    libraries = []

    # no arguments? Show usage
    if len(argv) == 1:
        usage()
        exit()

    # parse arguments
    try:
        opts, args = getopt.getopt(argv[1:], "c:g:hp:", ["help"])
    except getopt.GetoptError:
        print("Invalid arguments provided")
        usage()
        exit(2)

    for opt, arg in opts:
        if opt in ['-h', '--help']:
            usage()
            exit()
        elif opt == "-g":
            generator = arg
        elif opt == "-p":
            if (arg in ['exiv2', 'jsoncpp', 'openlibrary']):
                libraries.append(arg)
            else:
                print("'" + arg + "' is not valid package name.")
                exit(2)
        elif opt == "-c":
            cmake = arg

    if cmake is None or is_exe(cmake) == False:
        print("No valid path to 'cmake' was provided.")
        exit(2)

    # we expect one argument (desitnation dir) in args
    if len(args) == 0:
        print("No destination dir was provided.")
        usage()
        exit(2)

    if len(args) > 1:
        print("Too many desination dirs. Only one is expected.")
        usage()
        exit(2)

if __name__ == "__main__":
   main(argv[0:])
