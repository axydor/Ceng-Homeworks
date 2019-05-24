from socket import *
import struct
import binascii
import time

PACKET_SIZE=8

# We use same port number at every host for ease of use
port = 12000

# route_table holds the inet addresses of the followings
route_table = {"destination": ""}

# Sockets for router
destination_socket = socket(AF_INET, SOCK_DGRAM)

################################################################################

def set_addresses(table):
    route_table = open("routeTable.txt","r")
    for line in route_table:
        linewords = line.split("|")[0].split(":")
        address = linewords[0]
        if address == "Destination":
            table["destination"] = linewords[1].strip()
            break

################################################################################

def close_sockets():
    try:
        destination_socket.close()
        print("All sockets closed successfully")
    except Exception as e:
        print("Error happened when closing sockets")
        print(e)

################################################################################

def acknowledge(coming_address):
    message = b'ACK'
    destination_socket.sendto(message, (coming_address, port))

################################################################################

# We get message from any router and then send an ACK message to the router
def serve(route_table):
    #time_offset = 0.6219597
    #s = struct.Struct('8s d')
    try:
        with open("output", "w") as output:
            while 1:
                message, coming_address = destination_socket.recvfrom(PACKET_SIZE)
                #if message:
                    #unpacked = s.unpack(message)
                    # Calculating end-to-end delay
                    #output.write( str( (time.time()-unpacked[1] - time_offset )*1000) + '\n' ) 
                acknowledge(coming_address[0])
    except KeyboardInterrupt:
        print("Exiting from program...")

################################################################################

# Bind socket so that we can receive and send messages from this socket
def set_socket(route_table):
    destination_socket.bind((route_table["destination"], port))

################################################################################

if __name__ == "__main__":
    try:
        set_addresses(route_table)
        set_socket(route_table)
        serve(route_table)
    except Exception as e:  
        print(e)
    close_sockets()
