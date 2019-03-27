
# Purpose of this script is to buils zip file containing all necessary Python modules

import os
from pip._internal import main as pipmain
import shutil
import sys
import zipfile

import InterpreterDownloader
import DlibDownloader
import DlibBuilder


def list_files(dir: str):
    result = []
    def _list_files(dir, cutoff):
        for entry in os.listdir(dir):
            path = dir + "/" + entry
            if os.path.isfile(path):
                result.append(path[cutoff:])
            elif os.path.isdir(path):
                _list_files(path, cutoff)

    cutoff = len(dir) + 1
    _list_files(dir, cutoff)

    return result


def python_package(major_ver, minor_ver, arch, tmp_dir):
    # Download python embed package
    python_pkg = tmp_dir + "/python_embed.zip"
    InterpreterDownloader.download_python(major_ver, minor_ver, arch, python_pkg)

    # unzip package
    python_env = tmp_dir + "/python_embed"
    zip_ref = zipfile.ZipFile(python_pkg, 'r')
    zip_ref.extractall(python_env)
    zip_ref.close()

    return python_env


def dlib_package(tmp_dir):
    # Download dlib
    dlib_pkg = tmp_dir + "/dlib.zip"
    DlibDownloader.download_dlib(dlib_pkg)

    # unzip dlib package
    dlib = tmp_dir + "/dlib"
    zip_ref = zipfile.ZipFile(dlib_pkg, 'r')
    zip_ref.extractall(dlib)
    zip_ref.close()

    # build dlib
    dlib_whl = DlibBuilder.build_dlib(dlib)
    return dlib_whl


def install_pkgs(dir, pkgs):
    pip_args = ['install', '--compile', '--target', dir]
    pip_args.extend(pkgs)
    pipmain(pip_args)


def main():
    assert len(sys.argv) == 6, "Expecting 5 arguments"

    output_file = sys.argv[1]
    tmp_dir = sys.argv[2]       # temporary dir
    major_ver = sys.argv[3]
    minor_ver = sys.argv[4]
    arch = sys.argv[5]

    python_env = python_package(major_ver, minor_ver, arch, tmp_dir)
    dlib_whl = dlib_package(tmp_dir)
    install_pkgs(python_env, [dlib_whl, "setuptools", "face_recognition"])

    output_file_name, output_file_ext = os.path.splitext(output_file)
    shutil.make_archive(base_name=output_file_name,
                        format=output_file_ext[1:],
                        root_dir=tmp_dir + "/python_embed")


if __name__ == "__main__":
    main()
