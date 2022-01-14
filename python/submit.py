from os import path
from datetime import datetime
from subprocess import run, CalledProcessError


def submit_lpjml(config_file, model_path, output_path=None, group="copan",
                 sclass="short", ntasks=256, wtime=None, dependency=None,
                 blocking=None):
    """Submit lpjml (spinup) run to Slurm using `lpjsubmit` and a generated
    (class LpjmlConfig) config file. Provide arguments for Slurm sbatch
    depending on the run. Similar to R function `lpjmlKit::submit_lpjml`.
    :param config_file: file name including path if not current to config_file
    :type config_file: str
    :param model_path: path to `LPJmL_internal` (lpjml repository)
    :type model_path: str
    :param output_path: path the output is written to, if None (default)
        model_path is used.
    :type output_path: str
    :param group: PIK group name to be used for Slurm. Defaults to "copan".
    :type output_path: str
    :param sclass: define the job classification,
        for more information have a look
        [here](https://www.pik-potsdam.de/en/institute/about/it-services/hpc/user-guides/slurm#section-5).
        Defaults to "short".
    :type sclass: str
    :param ntasks: define the number of tasks/threads,
        for more information have a look
        [here](https://www.pik-potsdam.de/en/institute/about/it-services/hpc/user-guides/slurm#section-18).
        Defaults to 256.
    :type ntasks: int/str
    :param wtime: define the time limit which can be an advantage to get faster
        to the top of the (s)queue. For more information have a look
        [here](https://www.pik-potsdam.de/en/institute/about/it-services/hpc/user-guides/slurm#section-18).
    :type wtime: str
    :param dependency: if there is a job that should be processed first (e.g.
        spinup) then pass its job id here.
    :type depdendency: int/str
    :param blocking: cores to be blocked. For more information have a look
        [here](https://www.pik-potsdam.de/en/institute/about/it-services/hpc/user-guides/slurm#section-18).
    :return: return the submitted jobs id if submitted successfully.
    :rtype: str
    """
    if not path.isdir(model_path):
        raise ValueError(
            f"Folder of model_path '{model_path}' does not exist!"
        )
    if not output_path:
        output_path = model_path
    else:
        if not path.isdir(output_path):
            raise ValueError(
                f"Folder of output_path '{output_path}' does not exist!"
            )
    # set timestamp with stdout and stderr files to write by batch process
    timestamp = datetime.now().strftime("%Y%m%d_%H%M")
    stdout = f"{output_path}/outfile_{timestamp}.out"
    stderr = f"{output_path}/errfile_{timestamp}.err"
    # prepare lpjsubmit command to be called via subprocess
    cmd = [f"{model_path}/bin/lpjsubmit"]
    # specify sbatch arguments required by lpjsubmit internally
    cmd.extend([
        "-group", group, "-class", sclass, "-o", stdout, "-e", stderr
    ])
    # if dependency (jobid) defined, submit is queued by slurm with nocheck
    if dependency:
        cmd.extend(["-nocheck", "-dependency", str(dependency)])
    # processing time to get a better position in slurm queue
    if wtime:
        cmd.extend(["-wtime", str(wtime)])
    # if cores to be blocked
    if blocking:
        cmd.extend(["-blocking", blocking])
    cmd.extend([str(ntasks), config_file])
    # call lpjsubmit via subprocess and return status if successfull
    submit_status = run(cmd, capture_output=True)
    # print stdout and stderr if not successful
    if submit_status.returncode == 0:
        print(submit_status.stdout.decode('utf-8'))
    else:
        print(submit_status.stdout.decode('utf-8'))
        print(submit_status.stderr.decode('utf-8'))
        raise CalledProcessError(submit_status.returncode, submit_status.args)
    # return job id
    return submit_status.stdout.decode('utf-8').split(
        "Submitted batch job "
    )[1].split("\n")[0]


def submit_couple():
    """Start coupled runs of copan:core and LPJmL
    """
    pass
