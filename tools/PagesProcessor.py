
import certifi
import re
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


def find_files(entries, pattern=".*"):
    files = []
    for entry in entries:
        if entry[-1] != "/" and re.match(pattern, entry):
            files.append(entry)

    return files


def find_matching_version(versions, major, minor):
    matching_versions = []

    for version in versions:
        # major.minor+not_a_number
        is_wanted_version = re.match(major + '\.' + minor + '\D+.*', version)

        if is_wanted_version:
            matching_versions.append(version)

    return matching_versions[-1]
