
import certifi
import re
import sys
import urllib3
from distutils.version import StrictVersion
from PagesProcessor import list_content, find_versions, find_matching_version, find_files


class FileWithVersion(StrictVersion):
    def __init__(self, version):
        match = re.match("dlib-(\d+).(\d+).*", version)
        major = match.group(1)
        minor = match.group(2)
        StrictVersion.__init__(self, major + "." + minor)


if __name__ == "__main__":

    assert len(sys.argv) == 2, "Expecting 1 arguments"   # script name + download path

    output_file = sys.argv[1]

    url = 'http://dlib.net/files/'
    all_entries = list_content(url)
    files = find_files(all_entries, "dlib-.*zip")

    files.sort(key=FileWithVersion)

    pkg_file_url = url + files[-1]

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
