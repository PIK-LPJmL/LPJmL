import socket
import struct
from enum import Enum
from config import LpjmlConfig, read_config

COPAN_COUPLER_VERSION = 1  # Protocol version
DEFAULT_PORT = 2224        # default port for in- and outgoing data

# List of tokens

GET_DATA = 0       # Receiving data from COPAN
PUT_DATA = 1       # Sending data to COPAN
GET_DATA_SIZE = 2  # Receiving data size from COPAN
PUT_DATA_SIZE = 3  # Sending data size to COPAN
END_DATA = 4       # Ending communication

N_OUT = 346       # Number of available output data streams
N_IN = 23         # Number of available input data streams

CLOUD_DATA = 0
TEMP_DATA = 1
PREC_DATA = 2
SWDOWN_DATA = 3
LWNET_DATA = 4
CO2_DATA = 5
LANDUSE_DATA = 6
TILLAGE_DATA = 7
RESIDUE_DATA = 8
TMIN_DATA = 9
TMAX_DATA = 10
TAMP_DATA = 11
WET_DATA = 12
BURNTAREA_DATA = 13
HUMID_DATA = 14
WIND_DATA = 15
NH4_DATA = 16
NO3_DATA = 17
FERTILIZER_DATA = 18
MANURE_DATA = 19
WATERUSE_DATA = 20
POPDENS_DATA = 21
HUMAN_IGNITION_DATA = 22

GRID = 0
COUNTRY = 1
REGION = 2
GLOBALFLUX = 3

LANDUSE_NBANDS = 64  # number of bands in landuse data
FERTILIZER_NBANDS = 32  # number of bands in fertilizer data


def recvall(channel, size):
    string = ""
    nbytes = 0
    while bytes < size:
        string += channel.recv(size-nbytes)
        nbytes += len(string)
    return str


def write_int(channel, val):
    channel.sendall(struct.pack('i', val))


def read_int(channel):
    intstr = recvall(channel, struct.calcsize('i'))
    inttup = struct.unpack('i', intstr)
    return inttup[0]


def read_short(channel):
    intstr = recvall(channel, struct.calcsize('h'))
    inttup = struct.unpack('h', intstr)
    return inttup[0]


def write_char(channel, val):
    channel.sendall(val)


def read_char(channel):
    # The size of a char is always 1, and there is nothing to unpack
    c = recvall(channel, 1)
    return c


def write_float(channel, val):
    channel.sendall(struct.pack('f', val))


def read_float(channel):
    floatstr = recvall(channel, struct.calcsize('f'))
    floattup = struct.unpack('f', floatstr)
    return floattup[0]


def opentdt(port):
    # create an INET, STREAMing socket
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # bind the socket to a public host, and a well-known port
    serversocket.bind(("", port))
    # become a server socket
    serversocket.listen(5)
    # accept connections from outside
    (channel, address) = serversocket.accept()
    channel.send('1')
    known_int = read_int(channel)
    num = read_int(channel)
    num = 1
    write_int(channel, num)
    return channel


def read_token(channel):
    # create Token object
    return Token(read_int(channel))


class Token(Enum):
    """Available tokens"""
    GET_DATA: int = 0       # Receiving data from COPAN
    PUT_DATA: int = 1       # Sending data to COPAN
    GET_DATA_SIZE: int = 2  # Receiving data size from COPAN
    PUT_DATA_SIZE: int = 3  # Sending data size to COPAN
    END_DATA: int = 4       # Ending communication


class Inputs(Enum):
    """Available tokens"""
    landuse: int = 64  # number of bands in landuse data
    fertilizer_nr: int = 32  # number of bands in fertilizer data
    manure_nr: int = 32  # number of bands in manure data
    residue_on_field: int = 32  # number of bands in residue data
    with_tillage: int = 2  # number of bands in tillage data


class StaticOutputs(Enum):
    """Static LPJmL Outputs
    """
    GRID: int = 0
    COUNTRY: int = 1
    REGION: int = 2


class GlobalFluxOutput(Enum):
    """Which is global flux
    """
    GLOBALFLUX: int = 3


class Coupler:
    # constructor; set channel
    def __init__(self, config_file, version=1, port=2224):
        config = read_config(config_file)
        # open/initialize socket channel
        self.channel = opentdt(port)
        # Check coupler protocol version
        self.version = read_int(self.channel)
        if (self.version != version):
            self.channel.close()
            raise ValueError(
                f"Invalid coupler version {version}, must be {self.version}"
            )
        # read amount of LPJml cells
        self.ncell = read_int(self.channel)
        # read amount of input streams
        self.n_in = read_int(self.channel)
        # read amount of output streams
        self.n_out = read_int(self.channel)
        # Send number of bands per cell for each input data stream
        self.iterate_operation(
            length=self.n_in, fun=self.write_input_size,
            token=Token.GET_DATA_SIZE,
            args={"input_bands": self.read_config_sockets()}
        )
        # Get number of bands per cell for each output data stream
        self.out_bands = [-1] * self.n_out
        self.out_data = [0.0] * self.ncell
        self.out_static = 0
        self.iterate_operation(
            length=self.n_out, fun=self.read_output_details,
            token=Token.PUT_DATA_SIZE
        )
        # Read all static non time dependent outputs

    def iterate_operation(self, length, fun, token, args):
        token_check, received_token = self.check_token(token)
        if token_check:
            val = fun(self.channel, **args)
        else:
            raise ValueError(
                f"Token {received_token.name} is not {token.name}"
            )
        if length > 0:
            return self.iterate_operation(length-1, fun)
        else:
            return val

    def check_token(self, token):
        received_token = read_token(self.channel)
        if received_token is token:
            return True, token
        else:
            return False, token

    def write_input_size(self, input_bands):
        val = read_int(self.channel)
        if val in input_bands.keys():
            # Send number of bands
            write_int(self.channel, val=input_bands[val])
        else:
            write_int(self.channel, val=0)

    def read_config_sockets(self):
        sockets = self.config.get_input_sockets
        input_names = [inp.name for inp in Inputs]
        valid_inputs = {
            sock: getattr(
                Inputs, sock
            ).value for sock in sockets if sock in input_names
        }
        if len(sockets) != len(valid_inputs):
            raise ValueError(
                f"Configurated sockets {sockets.keys()} not defined in " +
                f"{input_names}!"
            )
        return valid_inputs

    def read_output_size(self):
        val = read_int(self.channel)
        # Get number of bands for output
        self.out_bands[val] = read_int(self.channel)
        # Check for static output
        outputs_avail = self.config.get_outputs_avail(id_only=False)
        globalflux = [out["id"] for out in outputs_avail if out[
            "name"] == "globalflux"]
        output_static = [out["id"] for out in outputs_avail if out[
            "name"] in ["grid", "country", "region"]]
        if val in globalflux:
            self.flux = [0] * count[val]
        elif val in output_static:
            self.out_static += 1

    def send_data(self):
        pass

    def receive_data(self):
        pass


# Read all static non time dependent outputs
for i in range(0, n_out_1):
    token = read_int(channel)
    if token != PUT_DATA:
        print("Token " + str(token) + " is not PUT_DATA")
        channel.close()
        quit()
    index = read_int(channel)
    if index == GRID:
        for i in range(0, ncell):
            lon = read_short(channel) * 0.01
            lat = read_short(channel) * 0.01
            print(lon, lat)
    else:
        print("Unsupported output " + str(index))
# Reduce the number of output streams by the number of static streams
n_out = n_out - n_out_1


# Main simulation loop
while True:
    # Send input to LPJmL
    for i in range(0, n_in):
        token = read_int(channel)
        # Did we receive end token?
        if(token == END_DATA):
            break
        if token != GET_DATA:
            print("Token " + str(token) + " is not GET_DATA")
            channel.close()
            quit()
        index = read_int(channel)
        year = read_int(channel)
        if index == LANDUSE_DATA:
            for j in range(0, ncell * LANDUSE_NBANDS):
                write_float(channel, landuse[j])
        elif index == CO2_DATA:
            write_float(channel, 288.0)
        else:
            print("Unsupported input " + str(index))
    if(token == END_DATA):
        break
# Get output from LPJmL
    for i in range(0, n_out):
        token = read_int(channel)
        if token != PUT_DATA:
            print("Token " + str(token) + " is not PUT_DATA")
            channel.close()
            quit()
        index = read_int(channel)
        print(index)
        print(count[index])
        year = read_int(channel)
        print(year)
        if index == GLOBALFLUX:
            for j in range(0, count[index]):
                flux[j] = read_float(channel)
            print("Flux:" + str(flux))
        else:
            for j in range(0, count[index]):
                for k in range(0, ncell):
                    data[k] = read_float(channel)
                print(data)
print("End of communication")
channel.close()
