
from sys import argv
from sys import exit
import getopt

argc = len(argv)

def usage():
    print("Usage:")
    print("prepare_dependencies.py [options] destination dir")
    print("")
    print("Possible options:")
    print("-p <name>      Download nad install package.")
    print("               Possible packages:")
    print("               exiv2, jsoncpp, openlibrary")
    print("               Option can be reapeted.")
    print("")
    print("-g <generator>")


def main(argv):
    if len(argv) == 0:
        usage()
        exit(2)
        
    try:
        opts, args = getopt.getopt(argv, "p:g:h", [])
    except getopt.GetoptError:
        usage()
        exit(2)
    for opt, arg in opts:
        if opt == '-h':
            usage()
            exit()
        elif opt == "-g":
            print("-g", arg)
        elif opt  == "-p":
            print("-p", arg)

    print("exit")

if __name__ == "__main__":
   main(argv[1:])
