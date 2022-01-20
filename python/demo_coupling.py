#######################################################################################
###                                                                                 ###
###                c  o  p  a  n  _  d  e  m  o  .  p  y                            ###
###                                                                                 ###
###     Demonstration program for coupling LPJmL to COPAN                           ###
###                                                                                 ###
### (C) Potsdam Institute for Climate Impact Research (PIK), see COPYRIGHT file     ###
### authors, and contributors see AUTHORS file                                      ###
### This file is part of LPJmL and licensed under GNU AGPL Version 3                ###
### or later. See LICENSE file or go to http://www.gnu.org/licenses/                ###
### Contact: https://github.com/PIK-LPJmL/LPJmL                                     ###
###                                                                                 ###
#######################################################################################

import socket
import struct

COPAN_COUPLER_VERSION=2 # Protocol version
DEFAULT_PORT=2224       # default port for in- and outgoing data

# List of tokens

GET_DATA=0       # Receiving data from COPAN
PUT_DATA=1       # Sending data to COPAN
GET_DATA_SIZE=2  # Receiving data size from COPAN
PUT_DATA_SIZE=3  # Sending data size to COPAN
END_DATA=4       # Ending communication

N_OUT=346       # Number of available output data streams
N_IN=24         # Number of available input data streams

CLOUD_DATA=0
TEMP_DATA=1
PREC_DATA=2
SWDOWN_DATA=3
LWNET_DATA=4
CO2_DATA=5
LANDUSE_DATA=6
TILLAGE_DATA=7
RESIDUE_DATA=8
TMIN_DATA=9
TMAX_DATA=10
TAMP_DATA=11
WET_DATA=12
BURNTAREA_DATA=13
HUMID_DATA=14
WIND_DATA=15
NH4_DATA=16
NO3_DATA=17
FERTILIZER_DATA=18
MANURE_DATA=19
WATERUSE_DATA=20
POPDENS_DATA=21
HUMAN_IGNITION_DATA=22
LIGHTNING_DATA=23

GRID=0
COUNTRY=1
REGION=2
GLOBALFLUX=3

# LPJ datatypes

LPJ_BYTE=0
LPJ_SHORT=1
LPJ_INT=2
LPJ_FLOAT=3
LPJ_DOUBLE=4

LANDUSE_NBANDS=64 # number of bands in landuse data

def recvall (channel, size):
  str   = ""
  bytes = 0
  while bytes < size:
    str += channel.recv (size-bytes)
    bytes += len (str)
  return str

def write_int (channel, val):
  channel.sendall (struct.pack ('i', val))

def read_int (channel):
  intstr = recvall (channel,struct.calcsize ('i'))
  inttup = struct.unpack ('i', intstr)
  return inttup[0]

def read_short (channel):
  intstr = recvall (channel,struct.calcsize ('h'))
  inttup = struct.unpack ('h', intstr)
  return inttup[0]

def write_char (channel, val):
  channel.sendall (val)

def read_char (channel):
# The size of a char is always 1, and there is nothing to unpack
  c = recvall (channel,1)
  return c

def write_float (channel, val):
  channel.sendall (struct.pack ('f', val))

def read_float (channel):
  floatstr = recvall (channel,struct.calcsize ('f'))
  floattup = struct.unpack ('f', floatstr)
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
  known_int=read_int(channel)
  num=read_int(channel)
  num=1
  write_int(channel,num)
  return channel

print("Waiting for LPJmL model...")
# Establish the connection
channel=opentdt(DEFAULT_PORT)
# Get protocol version
version=read_int(channel)
if version!=COPAN_COUPLER_VERSION:
  print("Invalid coupler version "+str(version)+", must be "+str(COPAN_COUPLER_VERSION))
  channel.close()
  quit()
ncell=read_int(channel)
print('Number of cells: '+str(ncell))
n_in=read_int(channel)
n_out=read_int(channel)
print("Number of input streams:  "+str(n_in))
print("Number of output streams: "+str(n_out))

# Send number of items per cell for each input data stream
types_in=[-1]*N_IN

for i in range(0,n_in):
  token=read_int(channel)
  if token!=GET_DATA_SIZE:
    print("Token "+str(token)+" is not GET_DATA_SIZE")
    channel.close()
    quit()
  index=read_int(channel)
  types_in[index]=read_int(channel)
  if index==LANDUSE_DATA:
    landuse=[0.001]*LANDUSE_NBANDS*ncell
    index=LANDUSE_NBANDS
  elif index==CO2_DATA:
    index=1
  else:
    print("Unsupported input "+str(index))
# Send number of bands
  write_int(channel,index)

# Get number of items per cell for each output data stream

count=[-1]*N_OUT
nstep=[-1]*N_OUT
types=[-1]*N_OUT
data=[0.0]*ncell
n_out_1=0
for i in range(0,n_out):
  token=read_int(channel)
  if token!=PUT_DATA_SIZE:
    print("Token "+str(token)+" is not PUT_DATA_SIZE")
    channel.close()
    quit()
  index=read_int(channel)
# Get number of time steps per year  for output
  nstep[index]=read_int(channel)
# Get number of bands for output
  count[index]=read_int(channel)
# Get datatype for output
  types[index]=read_int(channel)
# Check for static output
  if index==GLOBALFLUX:
    flux=[0]*count[index]
  elif index==GRID or index==COUNTRY or index==REGION:
    n_out_1=n_out_1+1

# Read all static non time dependent outputs

for i in range(0,n_out_1):
  token=read_int(channel)
  if token!=PUT_DATA:
    print("Token "+str(token)+" is not PUT_DATA")
    channel.close()
    quit()
  index=read_int(channel)
  if index==GRID:
    for i in range(0,ncell):
      lon=read_short(channel)*0.01
      lat=read_short(channel)*0.01
      print(lon,lat)
  else:
    print("Unsupported output "+str(index))
# Reduce the number of output streams by the number of static streams
n_out=n_out-n_out_1

# Main simulation loop

while True:
# Send input to LPJmL
  for i in range(0,n_in):
    token=read_int(channel)
    if(token==END_DATA): # Did we receive end token?
      break
    if token!=GET_DATA:
      print("Token "+str(token)+" is not GET_DATA")
      channel.close()
      quit()
    index=read_int(channel)
    year=read_int(channel)
    if index==LANDUSE_DATA:
      for j in range(0,ncell*LANDUSE_NBANDS):
        write_float(channel,landuse[j])
    elif index==CO2_DATA:
      write_float(channel,288.)
    else:
      print("Unsupported input "+str(index))
  if(token==END_DATA):
    break
# Get output from LPJmL
  for i in range(0,n_out):
    token=read_int(channel)
    if token!=PUT_DATA:
      print("Token "+str(token)+" is not PUT_DATA")
      channel.close()
      quit()
    index=read_int(channel)
    print(index)
    print(count[index])
    year=read_int(channel)
    print(year)
    if index==GLOBALFLUX:
      for j in range(0,count[index]):
        flux[j]=read_float(channel)
      print("Flux:"+str(flux))
    else:
      for j in range(0,count[index]):
        for k in range(0,ncell):
          data[k]=read_float(channel)
        print(data)
print("End of communication")
channel.close()
