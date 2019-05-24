import pickle
from socket import *
from random import randint

PACKET_SIZE=8

route_table = {"broker": "", "r1": "", "r2": "", "broker_source": ""}
port = 12000

# Socket for communicating with the routers
router_socket = socket(AF_INET, SOCK_DGRAM)

# Socket for initiating the communication with source
source_socket = socket(AF_INET, SOCK_STREAM)
#source_socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

################################################################################

def set_addresses(table):
    route_table = open("routeTable.txt","r")
    for line in route_table:
        linewords = line.split("|")[0].split(":")
        address = linewords[0]
        if address == "R1":
            table["r1"] = linewords[1].strip()
        elif address == "R2":
            table["r2"] = linewords[1].strip()
        elif address == "Broker":
            table["broker"] = linewords[1].strip()
        elif address == "Broker_Source":
            table["broker_source"] = linewords[1].strip()

################################################################################

def close_sockets():
    try:
        router_socket.close()
        source_socket.close()
        print("All sockets closed successfully")
    except Exception as e:
        print("Error happened when closing sockets")
        print(e)

################################################################################

def send_to_source(message, connection_socket):
    connection_socket.send(message)

################################################################################

# We send data by choosing random router
def send_to_router(message, i):
	router = "r1"
	#router += str(i+1)
	router_socket.sendto(message, (route_table[router], port))

################################################################################

# One socket for communicating with source and other for routers
def set_sockets():
    router_socket.bind((route_table["broker"], port))
    source_socket.bind((route_table["broker_source"], port))

################################################################################

def receive_from_source(connection_socket):
    message = connection_socket.recv(PACKET_SIZE)
    return message

################################################################################

def receive_from_router():
    message, coming_address = router_socket.recvfrom(PACKET_SIZE)
    return message

################################################################################
# TODO check if the connection comes from the source or another malicious source
# Accept connection from the source
def accept_source():
    source_socket.listen()
    connection_socket, source_address = source_socket.accept()
    return connection_socket

################################################################################

def serve():
    i = 0
    try:
        while 1:
            connection_socket = accept_source()
            while 1:
                message = receive_from_source(connection_socket)
                if not message:
                    connection_socket.close()
                    #print("Connection closed by the Client")
                    break
                i = i ^ 1
                send_to_router(message, i)
                message = receive_from_router()
                send_to_source(message, connection_socket)
    except KeyboardInterrupt:
            print("Exiting from program...")

################################################################################

if __name__ == "__main__":
    try:
        set_addresses(route_table)
        set_sockets()
        serve()
    except Exception as e:
        print(e)
    close_sockets()
