
import certifi
import re
import sys
import urllib3
from html.parser import HTMLParser


class IndexParser(HTMLParser):

    def __init__(self):
        HTMLParser.__init__(self)
        self.__in_a_href = False
        self.__entries = []

    def handle_starttag(self, tag, attrs):
        if tag == "a":
            assert self.__in_a_href is False, "Nested link is not a valid construction"
            self.__in_a_href = True

    def handle_endtag(self, tag):
        if tag == "a":
            assert self.__in_a_href is True, "Missing star tag for </a>"
            self.__in_a_href = False

    def handle_data(self, data):
        if self.__in_a_href is True:
            self.__entries.append(data)

    def get_data(self):
        for entry in self.__entries:
            yield entry


def list_content(url: str):
    connection_pool = urllib3.PoolManager(cert_reqs='CERT_REQUIRED', ca_certs=certifi.where())
    resp = connection_pool.request('GET', url)
    html = resp.data

    parser = IndexParser()
    parser.feed(str(html))

    entries = [link for link in parser.get_data()]

    return entries


def find_versions(entries):
    versions_list = []

    for entry in entries:
        if entry[-1] == "/" and re.match("\d+\.\d+.*/", entry):
            versions_list.append(entry[:-1])                        # drop trailing "/"

    return versions_list


def find_matching_version(versions, major, minor):
    matching_versions = []

    for version in versions:
        # major.minor+not_a_number
        is_wanted_version = re.match(version_major + '\.' + version_minor + '\D+.*', version)

        if is_wanted_version:
            matching_versions.append(version)

    return matching_versions[-1]


if __name__ == "__main__":

    assert len(sys.argv) == 4, "Expecting 3 arguments"   # script name + 3 additional args

    version_major = sys.argv[1]
    version_minor = sys.argv[2]
    arch          = sys.argv[3]   # win32 or amd64

    assert re.match("\d+", version_major), "First argument should be a major version number"
    assert re.match("\d+", version_minor), "Second argument should be a minor version number"
    assert re.match("win32|amd64", arch),  "Third argument should be win32 or amd64"

    url = 'http://www.python.org/ftp/python/'
    all_entries = list_content(url)
    all_versions = find_versions(all_entries)
    version = find_matching_version(all_versions, version_major, version_minor)

    url += version
    files = list_content(url)

    desired_file_name_pattern = "python-" + version + "-embed-" + arch + ".zip"
    pkg_file_url = ""

    for file in files:
        if file == desired_file_name_pattern:
            pkg_file_url = url + "/" + file
            break

    print(pkg_file_url)
