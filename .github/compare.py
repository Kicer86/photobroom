
import sys

expected = open('.github/expected_file_list','rt')

expected_files = set()
for line in expected:
    expected_files.add(line.strip())

package_files = set()
for line in sys.stdin:
    package_files.add(line.strip())

missing_expected = expected_files - package_files
package_additional = package_files - expected_files

if missing_expected:
    print(f"There are missing files in package: {missing_expected}")

if package_additional:
    print(f"There are additional files in package: {package_additional}")

if missing_expected or package_additional:
    print()
    print(f"Expected list of files: {expected_files}")
    print(f"list of files in package: {package_files}")
    sys.exit(1)
