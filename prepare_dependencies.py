
from sys import argv
from sys import exit
import getopt
import os.path
import shutil


def usage(name):
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


def get_script_name(script_path):
    script_path = argv[0]
    script_path_splited = script_path.split(os.path.sep)
    script_name = script_path_splited[-1]

    return script_name


def is_exe(path):
    return os.path.isfile(path) and os.access(path, os.X_OK)


def main(argv):

    cmake = shutil.which('cmake')
    name = get_script_name(argv[0])

    # no arguments? Show usage
    if len(argv) == 1:
        usage(name)
        exit()

    #parse arguments
    try:
        opts, args = getopt.getopt(argv[1:], "p:g:h", ["help"])
    except getopt.GetoptError:
        print("Invalid arguments provided")
        usage(name)
        exit(2)

    for opt, arg in opts:
        if opt in ['-h', '--help']:
            usage(name)
            exit()
        elif opt == "-g":
            print("-g", arg)
        elif opt == "-p":
            print("-p", arg)
        elif opt == "-c":
            cmake = arg

    if cmake is None or is_exe(cmake) == False:
        print("No valid path to 'cmake' was provided.")
        exit(2)


if __name__ == "__main__":
   main(argv[0:])
