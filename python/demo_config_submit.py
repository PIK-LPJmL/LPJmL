from copan_core.lpjml.LPJmL_internal.python.utils import check_lpjml
from lpjmlconfig import parse_config, LpjmlConfig
from submit import submit_lpjml
from utils import check_lpjml


# set paths
model_path = "/p/projects/open/Jannes/copan_core/lpjml/LPJmL_internal"
base_path = "/p/projects/open/Jannes/copan_core/coupling_tests"
output_path = f"{base_path}/output"
restart_path = f"{base_path}/restart"


# define and submit spinup run ---------------------------------------------- #

# set config for spinup run
config_spinup = parse_config(path=model_path, spin_up=True)
# set output directory
config_spinup.set_output_path(output_path)
# set restart directory to restart from in subsequent historic run
config_spinup.set_restart(path=restart_path)
# write config (LpjmlConfig object) as json file
config_spinup_fn = f"{base_path}/config_spinup.json"
config_spinup.to_json(file=config_spinup_fn)

# check if everything is set correct
check_lpjml(config_spinup_fn, model_path)
# submit spinup job and get corresponding id
spinup_jobid = submit_lpjml(config_spinup_fn, model_path, output_path)


# define and submit historic run -------------------------------------------- #

# set config for spinup run
config_historic = parse_config(path=model_path)
# set output directory
config_historic.set_outputs(output_path, outputs=[])
# set start from directory to start from spinup run
config_historic.set_startfrom(path=restart_path)
# set restart directory to restart from in subsequent transient run
config_historic.set_restart(path=restart_path)
# set time range for historic run
config_historic.set_timerange(start=1901, end=1980)
# write config (LpjmlConfig object) as json file
config_historic_fn = f"{base_path}/config_historic.json"
config_historic.to_json(file=config_historic_fn)

# check if everything is set correct
check_lpjml(config_historic_fn, model_path)
# submit spinup job and get corresponding id
historic_jobid = submit_lpjml(config_historic_fn, model_path, output_path)


# define coupled run -------------------------------------------------------- #

# set config for spinup run
config_coupled = parse_config(path=model_path)
# set start from directory to start from historic run
config_coupled.set_startfrom(path=restart_path)
# set output directory, outputs (relevant ones for pbs and agriculture)
config_coupled.set_outputs(
    output_path,
    outputs=["prec", "transp", "interc", "evap", "runoff", "discharge",
             "fpc", "vegc", "soilc", "litc", "cftfrac", "pft_harvestc",
             "pft_harvestn", "pet", "leaching"],
    temporal_resolution=["monthly", "monthly", "monthly", "monthly",
                         "monthly", "monthly", "annual", "annual", "annual",
                         "annual", "annual", "annual", "annual", "monthly",
                         "monthly"], file_format="cdf")
# set coupling parameters
config_coupled.set_coupled(
    inputs=["landuse", "fertilizer_nr", "with_tillage", "residue_on_field"],
    outputs=["cftfrac", "pft_harvestc", "pft_harvestn"])

# write config (LpjmlConfig object) as json file
config_coupled_fn = f"{base_path}/config_historic.json"
config_coupled.to_json(file=config_coupled_fn)
