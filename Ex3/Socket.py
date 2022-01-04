# import socket module
from socket import *
import sys  # In order to terminate the program

serverSocket = socket(AF_INET, SOCK_STREAM) # first of all, we create our server socket using SOCK_STREAM
serverSocket.bind(("192.118.132.135", 80))    # which related to TCP. Than we are binding between our IP address
serverSocket.listen(1)                      # and the port which the message sent in. and finally open our serverSocket
                                            # for listening until somthing will happen.

while True:
    print('Ready to serve...')
    connectionSocket, addr = serverSocket.accept() # The accept method of Python's socket class,
                                                   # accepts an incoming connection request from a TCP server
    try:
        message = connectionSocket.recv(1024)   #First of all, we are giving a range og 1024 bits for
        filename = message.split()[1]           # the message we would like to send.
        f = open(filename[1:])                  # we are spliting our message and than take the exact
        outputdata = f.read()                   # we want in our outputdata
        connectionSocket.send("\nHTTP/1.1 200 OK\n\n".encode())   # sending the ok message if everything went OK
        for i in range(0, len(outputdata)):                       # like OK message for a regular HTTP request
            connectionSocket.send(outputdata[i].encode())         # in wireshark. and finally send our message content
        connectionSocket.send("\r\n".encode())                    # and close our client server

        connectionSocket.close()
    except IOError:                                               # if something went wrong in our try part
        err="404 NOT FOUND"                                       # we are sending a "404 not found" message
        connectionSocket.send(err.encode())                       # as required and than closing our client server
        connectionSocket.close()
serverSocket.close()
sys.exit()  # Terminate the program after sending the corresponding data
