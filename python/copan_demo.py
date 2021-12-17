#######################################################################################
###                                                                                 ###
###                c  o  p  a  n  _  d  e  m  o  .  c                               ###
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

GET_DATA=0      # Receiving data from COPAN
PUT_DATA=1      # Sending data to COPAN
GET_DATA_SIZE=2 # Receiving data size from COPAN
PUT_DATA_SIZE=3 # Sending data size to COPAN
END_DATA=4      # Ending communication

N_OUT=346
N_IN=23         # Number of available input data streams
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

GRID=0
COUNTRY=1
REGION=2
GLOBALFLUX=3

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

# create an INET, STREAMing socket
print("Waiting for LPJmL model...")
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# bind the socket to a public host, and a well-known port
serversocket.bind(("", 2224))
# become a server socket
serversocket.listen(5)
# accept connections from outside
(channel, address) = serversocket.accept()
channel.send('1')
known_int=read_int(channel)
num=read_int(channel)
num=1
write_int(channel,num)
version=read_int(channel)
ncell=read_int(channel)
print('Number of cells: '+str(ncell))
n_in=read_int(channel)
n_out=read_int(channel)
print("Number of input streams:  "+str(n_in))
print("Number of output streams: "+str(n_out))
for i in range(0,n_in):
  token=read_int(channel)
  index=read_int(channel)
  if index==LANDUSE_DATA:
    landuse=[0.001]*64*ncell
    index=64
  elif index==CO2_DATA:
    index=1
  else:
    print("Unsupported input "+str(index))
  write_int(channel,index)
count=[-1]*N_OUT
data=[0.0]*ncell
n_out_1=0
for i in range(0,n_out):
  token=read_int(channel)
  index=read_int(channel)
  count[index]=read_int(channel)
  if index==GLOBALFLUX:
    flux=[0]*count[index]
  elif index==GRID or index==COUNTRY or index==REGION:
    n_out_1=n_out_1+1
for i in range(0,n_out_1):
  token=read_int(channel)
  index=read_int(channel)
  if index==GRID:
    for i in range(0,ncell):
      lon=read_short(channel)*0.01
      lat=read_short(channel)*0.01
      print(lon,lat)
  else:
    print("Unsupported output "+str(index))
n_out=n_out-n_out_1
while True:
  for i in range(0,n_in):
    token=read_int(channel)
    if(token==END_DATA):
      break 
    index=read_int(channel)
    year=read_int(channel)
    if index==LANDUSE_DATA:
      for j in range(0,ncell*64):
        write_float(channel,landuse[j])
    elif index==CO2_DATA:
      write_float(channel,288.)
    else:
      print("Unsupported input "+str(index))
  if(token==END_DATA):
    break 
  for i in range(0,n_out):
    token=read_int(channel)
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
