
import sys

def create_guard_file(input_file, output_file, namespace, variable):
    with open(input_file, 'r') as f:
        content = f.read()

    guard_name = output_file.upper().replace('.', '_').replace('/', '_')
    with open(output_file, 'w') as f:
        f.write('\n')
        f.write('#ifndef {}\n'.format(guard_name))
        f.write('#define {}\n\n'.format(guard_name))
        f.write('namespace {} {{\n'.format(namespace))
        f.write('    inline {} = R"({})";\n'.format(variable, content))
        f.write('}\n')
        f.write('#endif\n')

if __name__ == '__main__':
    create_guard_file(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
