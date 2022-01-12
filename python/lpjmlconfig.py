import subprocess
import json
import warnings


class LpjmlConfig:
    """This serves as an LPJmL config class that can be easily accessed,
    converted to a dictionary or written as a json file. It also provides
    methods to get/set outputs, restarts and sockets for model coupling.

    :param config_dict: takes a dictionary (ideally LPJmL config dictionary)
        and builds up a nested LpjmLConfig class with corresponding fields
    :type config_dict: dict
    """
    def __init__(self, config_dict):
        """Constructor method
        """
        self.__dict__.update(config_dict)

    def fields(self):
        """Return object fields
        """
        return list(self.__dict__.keys())

    def get_inputs(self, only_ids=True):
        """
        Get defined inputs as list
        """
        if only_ids:
            return self.input.__dict__.keys()
        else:
            return self.input.to_dict()

    def get_outputs_avail(self):
        """Get available output (outputvar) names (== output ids) as list
        """
        return [out.name for out in self.outputvar]

    def get_outputs(self, only_ids=True):
        """Get defined output ids as list
        """
        if only_ids:
            return [out.id for out in self.output]
        else:
            return self.to_dict()['output']

    def set_outputs(self, output_path, outputs=[], file_format="raw",
                    temporal_resolution="annual", append_output=False):
        """Set outputs to be written by LPJmL, define temporal resolution
        :param output_path: define output_path the output is written to. If
            `append_output == True` output_path is only altered for appended
            `outputs`.
        :type output_path: str
        :param outputs: output ids of `outputs` to be written by LPJmL. Make
            sure to check if required output is available via
            `get_outputs_avail`
        :type outputs: list
        :param file_format: file format for `outputs` (not to be used for
            sockets!). "raw" (binary), "clm" (binary with header) and "cdf"
            (NetCDF) are availble. Defaults to "raw".
        :type file_format: str
        :param temporal_resolution: list of temporal resolutions corresponding
            to `outputs` or str to set the same resolution for all `outputs`.
            Defaults to "annual" (for all `outputs`).
        :type temporal_resolution: list/str
        :param append_output: if True defined output entries are appended by
            defined `outputs`. Please mind that the existing ones are not
            altered.
        :param append_output: bool
        """
        resolution_avail = ('annual', 'monthly', 'daily')
        formats_avail = {'raw': 'bin', 'clm': 'clm', 'netcdf': 'cdf'}
        if isinstance(temporal_resolution, list):
            if any(
                res not in resolution_avail for res in temporal_resolution
            ):
                raise ValueError("Temporal resolution not available for " +
                                 "LPJmL. Choose from 'annual', 'monthly' " +
                                 "and 'daily'.")
            if len(outputs) != len(temporal_resolution):
                raise ValueError("outputs and temporal_resolution have a " +
                                 "different length. Please adjust.")
        elif isinstance(temporal_resolution, str):
            if temporal_resolution not in resolution_avail:
                raise ValueError("Temporal resolution not available for " +
                                 "LPJmL. Choose from 'annual', 'monthly' " +
                                 "and 'daily'.")
        else:
            TypeError("Provide a list or a str to temporal_resolution")
        # create dict of outputvar names with indexes for iteration
        outputvar_names = {
            ov.name: idx for idx, ov in enumerate(self.outputvar)
        }
        # extract dict of outputvar for manipulation
        outputvars = self.to_dict()['outputvar']
        # empty list to collect if defined outputs are not in outputvar
        not_found = []
        if not append_output:
            # empty output to be filled with defined entries (outputs)
            self.output = []
        # handle each defined output
        for idx, out in enumerate(outputs):
            # check temporal_resolution instance list/str
            if isinstance(temporal_resolution, list):
                temp_res = temporal_resolution[idx]
            else:
                temp_res = temporal_resolution
            if out in outputvar_names:
                new_out = {
                    'id': outputvars[outputvar_names[out]]['name'],
                    'file': {
                        'fmt': file_format,
                        'time_step': temp_res,
                        'name': f"{output_path}/"
                                f"{outputvars[outputvar_names[out]]['name']}"
                                f".{formats_avail[file_format]}"
                    }
                }
                self.output.append(self.__class__(new_out))
            else:
                # collect if defined outputs are not in outputvar
                not_found.append(out)
        if not_found:
            # warning for defined outputs that are not listed in outputvar
            warnings.warn("The following outputs do not exist in the" +
                          f" current model version: {not_found}")

    def get_startfrom(self):
        """Set restart file from which LPJmL starts the transient run
        """
        return self.restart_filename

    def set_startfrom(self, path=None, file_name=None):
        """Set restart file from which LPJmL starts the transient run
        """
        if path is not None:
            file_name = self.restart_filename.split("/")
            file_name.reverse()
            self.restart_filename = f"{path}/{file_name[0]}"
        elif file_name is not None:
            file_check = file_name.split(".")
            file_check.reverse()
            if file_check[0] == 'lpj':
                self.restart_filename = file_name
        else:
            raise ValueError('Please provide either a path or a file_name.')

    def set_sockets(self, inputs=[], outputs=[]):
        """Set sockets for inputs (corresponding key) and outputs (via id)
        :param inputs: list of inputs to be used as socket for coupling.
            Provide dictionary/json key as identifier -> entry in list.
        :type inputs: list
        :param inputs: list of outputs to be used as socket for coupling.
            Provide output id as identifier -> entry in list.
        :type inputs: list
        """
        for inp in inputs:
            sock_input = getattr(self, inp)
            sock_input.__dict__ = {'fmt': 'sock'}
        for out in self.output:
            if out.id in outputs:
                out.file.__dict__ = {'fmt': 'sock'}

    def to_dict(self):
        """Convert class object to dictionary
        """
        def obj_to_dict(obj):
            if not hasattr(obj, '__dict__'):
                return obj
            result = {}
            for key, val in obj.__dict__.items():
                if key.startswith("_"):
                    continue
                element = []
                if isinstance(val, list):
                    for item in val:
                        element.append(obj_to_dict(item))
                else:
                    element = obj_to_dict(val)
                result[key] = element
            return result
        return obj_to_dict(self)

    def to_json(self, file="./config.json"):
        """Write json file
        :param file: file name (including relative/absolute path) to write json
            to
        :type: str
        """
        # convert class to dict
        config_dict = self.to_dict()
        # write json and prettify via indent
        with open(file, 'w') as con:
            json.dump(config_dict, con, indent=2)

    def __repr__(self):
        return f"<{self.__class__.__name__} object>"


def parse_config(path, js_filename="lpjml.js", spin_up=False,
                 macros=None, return_dict=False):
    """Precompile lpjml.js and return LpjmlConfig object or dict. Also
    evaluate macros. Analogous to R function `lpjmlKit::parse_config`.
    :param path: path to lpjml root
    :type path: str
    :param js_filename: js file filename, defaults to lpjml.js
    :type js_filename: str
    :param from_restart: convenience argument to set macro whether to start
        from restart file (`True`) or not (`False`). Defaults to `True`
    :type from_restart: bool
    :param macros: provide a macro in the form of "-DMACRO" or list of macros
    :type macros: str, list
    :param return_lpjmlconfig: if `True` an LpjmlConfig object is returned,
        else (`False`) a dictionary is returned
    :type return_lpjmlconfig: bool
    :return: if `return_lpjmlconfig == True` -> LpjmlConfig object, else a
        a dictionary
    :rtype: LpjmlConfig, dict
    """
    # precompile command
    cmd = ["cpp", "-P"]
    # add arguments
    if not spin_up:
        cmd.append('-DFROM_RESTART')
    if macros:
        if isinstance(macros, list):
            cmd.extend(macros)
        else:
            cmd.append(macros)
    cmd.append(f"./{js_filename}")
    if not return_dict:
        config = LpjmlConfig
    else:
        config = None
    # subprocess call of cmd - return stdout
    tmp_json_str = subprocess.run(cmd, cwd=path, capture_output=True)
    # convert to dict
    tmp_json = json.loads(tmp_json_str.stdout, object_hook=config)
    return tmp_json

