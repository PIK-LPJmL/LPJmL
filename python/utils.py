from os import path, environ
from subprocess import run, Popen, PIPE, CalledProcessError


def clone_lpjml(model_location=".", branch="lpjml53_copan"):
    git_url = environ.get("GIT_LPJML_URL")
    git_token = environ.get("GIT_READ_TOKEN")
    cmd = ["git", "clone", f"https://oauth2:{git_token}@{git_url}"]
    with Popen(
        cmd, stdout=PIPE, bufsize=1, universal_newlines=True,
        cwd=model_location
    ) as p:
        for line in p.stdout:
            print(line, end='')
    # raise error if returncode does not reflect successfull call
    if p.returncode != 0:
        raise CalledProcessError(p.returncode, p.args)
    # check if branch required
    if branch:
        with Popen(
            ["git", "checkout", branch],
            stdout=PIPE, bufsize=1, universal_newlines=True,
            cwd=f"{model_location}/LPJmL_internal"
        ) as p:
            for line in p.stdout:
                print(line, end='')


def compile_lpjml(model_path=".", make_fast=False, make_clean=False):
    """ Compile or make lpjml after model clone/changes. make_fast for small
    changes, make_clean to delete previous compiled version (clean way)
    :param model_path: path to `LPJmL_internal` (lpjml repository)
    :type model_path: str
    :param make_fast: make with arg -j8. Defaults to False.
    :type make_fast: bool
    :param make_clean: delete previous compiled model version. Defaults to
        False.
    :type make_clean: bool
    """
    if not path.isdir(model_path):
        raise ValueError(
            f"Folder of model_path '{model_path}' does not exist!"
        )
    if path.isfile(f"{model_path}/bin/lpjml"):
        proc_status = run(
            "./configure.sh", capture_output=True, cwd=model_path
        )
        print(proc_status.stdout.decode('utf-8'))
    # make clean first
    if make_clean:
        run(["make", "clean"], capture_output=True, cwd=model_path)
    # make all call with possibility to make fast via -j8 arg
    cmd = ['make']
    if make_fast:
        cmd.append('-j8')
    cmd.append('all')
    # open process to be iteratively printed to the console
    with Popen(
        cmd, stdout=PIPE, bufsize=1, universal_newlines=True, cwd=model_path
    ) as p:
        for line in p.stdout:
            print(line, end='')
    # raise error if returncode does not reflect successfull call
    if p.returncode != 0:
        raise CalledProcessError(p.returncode, p.args)


def check_lpjml(config_file, model_path):
    """Check if config file is set correctly.
    :param config_file: file_name (including path) to generated config json 
        file.
    :type model_path: str
    :param model_path: path to `LPJmL_internal` (lpjml repository)
    :type model_path: str
    """
    if not path.isdir(model_path):
        raise ValueError(
            f"Folder of model_path '{model_path}' does not exist!"
        )
    if path.isfile(f"{model_path}/bin/lpjcheck"):
        proc_status = run(
            ["./bin/lpjcheck", config_file], capture_output=True,
            cwd=model_path
        )
    if proc_status.returncode == 0:
        print(proc_status.stdout.decode('utf-8'))
    else:
        print(proc_status.stdout.decode('utf-8'))
        print(proc_status.stderr.decode('utf-8'))
        raise CalledProcessError(proc_status.returncode, proc_status.args)
