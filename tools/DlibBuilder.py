
from pathlib import Path
import subprocess
import sys

if __name__ == "__main__":

    assert len(sys.argv) == 2, "Expecting 1 arguments"   # script name + dlib path

    wd = sys.argv[1]

    working_dir = Path(wd)
    subdirs = [d for d in working_dir.iterdir() if d.is_dir()]

    assert len(subdirs) == 1, "Expecting 1 subdir in dlib dir"

    dlib_dir = subdirs[0]
    build_script = "setup.py"

    result = subprocess.run([sys.executable, build_script, "bdist_wheel", "--no", "USE_AVX_INSTRUCTIONS"],
                            cwd=dlib_dir,
                            stdout=subprocess.PIPE,
                            stderr=subprocess.PIPE)

    if result.returncode == 0:
        dist_dir = dlib_dir / "dist"
        whl_files = list(dist_dir.glob("*.whl"))

        assert len(whl_files) == 1, "Expecting one whl file"
        whl = whl_files[0]

        print(str(whl))
    else:
        exit(1)
