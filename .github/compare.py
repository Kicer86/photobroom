
import sys

expected = open('.github/expected_file_list','rt')

expected_files = set()
for line in expected:
    expected_files.add(line.strip())

package_files = set()
for line in sys.stdin:
    package_files.add(line.strip())

missing_expected = list(expected_files - package_files)
package_additional = list(package_files - expected_files)

if missing_expected:
    sys.exit(f"There are missing files in package: {missing_expected}")

if package_additional:
    sys.exit(f"There are additional files in package: {package_additional}")
