
import certifi
import re
import sys
import urllib3
from PagesProcessor import list_content, find_versions, find_matching_version


def download_python(version_major, version_minor, version_patch, arch, output_file):
    url = 'http://www.python.org/ftp/python/'
    all_entries = list_content(url)
    all_versions = find_versions(all_entries)
    version = find_matching_version(all_versions, version_major, version_minor, version_patch)

    url += version
    files = list_content(url)

    desired_file_name = "python-" + version + "-embed-" + arch + ".zip"
    pkg_file_url = ""

    # check if desired file exists
    for file in files:
        if file == desired_file_name:
            pkg_file_url = url + "/" + file
            break

    # download file
    if pkg_file_url:
        connection_pool = urllib3.PoolManager(cert_reqs='CERT_REQUIRED', ca_certs=certifi.where())
        resp = connection_pool.request('GET', pkg_file_url, preload_content=False)
        with open(output_file, 'wb') as output:
            while True:
                data = resp.read(4096)
                if data:
                    output.write(data)
                else:
                    break


def main():
    assert len(sys.argv) == 4, "Expecting 3 arguments"   # script name + 3 additional args

    version       = sys.argv[1]
    arch          = sys.argv[2]   # win32 or amd64
    output_file   = sys.argv[3]

    assert re.match("(?:\d+\.)+\d", version), "First argument should be a major version number"
    assert re.match("win32|amd64", arch),  "Third argument should be win32 or amd64"

    version_splitted = version.split('.')

    assert len(version_splitted) == 2 or len(version_splitted) == 3, "Version should be in format x.y or x.y.z"

    major_ver = version_splitted[0]
    minor_ver = version_splitted[1]
    patch_ver = version_splitted[2] if len(version_splitted) == 3 else None

    download_python(major_ver, minor_ver, patch_ver, arch, output_file)


if __name__ == "__main__":
    main()
