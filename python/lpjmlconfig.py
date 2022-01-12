import subprocess
import json


class LpjmlConfig:
    """This serves as an LPJmL config class that can be easily accessed,
    converted to dict or written as a json file. It also provides methods to
    set sockets for model coupling.

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
        return(list(self.__dict__.keys()))

    def get_outputs_avail(self):
        """Get available output (outputvars) ids as list
        """
        pass

    def set_outputs(self, path, outputs, temporal_resolution):
        """Set outputs to be written by LPJmL, define temporal resolution
        """
        pass

    def set_restart(self, file):
        """Set restart file from which LPJmL starts the transient run
        """

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
            sock_input.__dict__ = {"fmt": "sock"}
        for out in self.output:
            if out.id in outputs:
                out.file.__dict__ = {"fmt": "sock"}

    def to_dict(self):
        """Convert class object to dictionary
        """
        def obj_to_dict(obj):
            if not hasattr(obj, "__dict__"):
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
        return(f"<{self.__class__.__name__} object>")


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
        cmd.append("-DFROM_RESTART")
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
    return(tmp_json)
