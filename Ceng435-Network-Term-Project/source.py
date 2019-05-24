from socket import *
import sys, time, struct, binascii

PACKET_SIZE=8

# route_table holds the inet addresses of the followings
route_table = {"source": "", "broker": ""}

# TCP Socket for communicating with broker
source_socket = socket(AF_INET, SOCK_STREAM)

# We use same port number at every host for ease of use
port = 12000

################################################################################

def set_addresses(table):
    route_table = open("routeTable.txt","r")
    for line in route_table:
        linewords = line.split("|")[0].split(":")
        address = linewords[0]
        if address == "Broker_Source" :
            table["broker"] = linewords[1].strip()
        elif address == "Source":
            table["source"] = linewords[1].strip()

################################################################################

def close_sockets():
    try:
        source_socket.close()
        #print("All sockets closed successfully")
    except Exception as e:
        print("Error happened when closing sockets")
        print(e)

################################################################################

# Send packet to broker
def send_to_broker(message):
    broker = route_table["broker"]
    source_socket.send(message)
        
################################################################################

# Receive ACK from broker
def receive_from_broker():
    message = source_socket.recv(PACKET_SIZE)
    return message

################################################################################

# If you want to calculate the end-to-end delay comment out the # lines
# And change the packet size into 16 in each script
# Send a file to destination through broker and routes
def send_file(filename):
    # We will read binary data from a file
    with open(filename, "rb") as filee:
        # We are reading 8 bytes from a file
        data = filee.read(8)
        # Size of this struct is 16
        #s = struct.Struct('8s d') 
        # We save the end-to-end delay into a file
        with open("output", "w") as result:
            while(data):
                start = time.time()
                #values = (data, start)
                #pack = s.pack(*values)
                #send_to_broker(pack)
                send_to_broker(data)
                message = receive_from_broker()
                end_to_end = str((time.time()-start)*1000/2)
                result.write(end_to_end + '\n')
                #print(end_to_end)
                data = filee.read(8)
                #print(message) This prints the ACK message for each packet

################################################################################

# Connect to Broker
def set_source_socket():
    source_socket.connect((route_table["broker"], port))

################################################################################

if __name__ == "__main__":
    try:
        set_addresses(route_table)
        set_source_socket()
        send_file(sys.argv[1])
    except Exception as e:
        print(e)
    close_sockets()
