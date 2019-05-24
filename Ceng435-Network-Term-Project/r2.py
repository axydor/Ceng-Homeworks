from socket import *

# We use same port number at every host for ease of use
port = 12000
PACKET_SIZE=8

# route_table holds the inet addresses of the followings
route_table = {"destination": "", "router": "", "broker": ""}

# Sockets for router
router_socket = socket(AF_INET, SOCK_DGRAM)

################################################################################

def set_addresses(table):
    route_table = open("routeTable.txt","r")
    for line in route_table:
        linewords = line.split("|")[0].split(":")
        address = linewords[0]
        if address == "Broker" :
            table["broker"] = linewords[1].strip()
        elif address == "Destination":
            table["destination"] = linewords[1].strip()
        elif address == "R2":
            table["router"] = linewords[1].strip()

################################################################################

def close_sockets():
    try:
        router_socket.close()
        print("All sockets closed successfully")
    except Exception as e:
        print("Error happened when closing sockets")
        print(e)

################################################################################

# TODO add sock 
def forward(message, coming_address, route_table):
    destination = route_table["destination"]
    broker = route_table["broker"]
    if coming_address[0] == destination:
        router_socket.sendto(message, (broker, port))
    else:
        router_socket.sendto(message, (destination, port))
        
################################################################################

def serve(route_table):
    try:
        while 1:
            message, coming_address = router_socket.recvfrom(PACKET_SIZE)
            if(message):
                forward(message, coming_address, route_table)
    except KeyboardInterrupt:
        print("Exiting from the program")

################################################################################

# Bind router_socket to address of the router
def set_router_socket(route_table):
    router_socket.bind((route_table["router"], port))

################################################################################

if __name__ == "__main__":
    try:
        set_addresses(route_table)
        set_router_socket(route_table)
        serve(route_table)
    except Exception as e:
        print(e)
    close_sockets()
