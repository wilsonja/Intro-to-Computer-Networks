#!/usr/bin/python
"""
   Name: Jacob Wilson
   Course: CS372_400
   Assignment: Program 1
   Description: The program serves as the server. It creates
    a socket with which to communicate to a client. It waits
    on this socket until a client connects. It then creates
    a new socket to communicate with the client. The server
    can disconnect from the client using '\quit' and will
    then wait for a new client.

   See References in README file
"""
import socket, sys

def main():
    if len(sys.argv) != 2:
        print "USAGE: $ ./chatserve [port number]"
    else:
        serverPort = int(sys.argv[1])
        host = ''

        s = startServer(serverPort,host)

        # this loop maintains a socket available for connection
        while 1:
            clientSocket, addr = s.accept()
            chatting = True
            print(receiveMessage(clientSocket))
            # this loop mantains a chat sequence while connected
            while chatting:
                clientMessage = receiveMessage(clientSocket)
                if clientMessage.endswith('\quit'):
                    chatting = clientQuit(clientSocket)
                else:
                    print clientMessage

                if chatting:
                    serverMessage = raw_input('server> ')

                if serverMessage == '\quit':
                    chatting = serverQuit(clientSocket)
                elif chatting:
                    sendMessage(clientSocket, serverMessage)
                serverMessage = ''

        clientSocket.close()
        s.close()

def startServer(serverPort, host):
    """
       The function starts the server using
       the provided port and host.

       Args:
          param1 (int): the server port
          param2 (string): the host name
       Returns:
          the newly formed socket
    """
    s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    s.bind((host,serverPort))
    s.listen(1)
    print "The server is now running on " + socket.gethostname() + ":" + str(serverPort)
    return s;

def receiveMessage(clientSocket):
    """
       The function receives a message from
       a connected client process.

       Args:
          param1 (int): the client socket
       Returns:
          None
    """
    message = clientSocket.recv(511)
    return message;

def sendMessage(clientSocket, serverMessage):
    """
       The functions sends a message from the
       server to a connected client.

       Args:
          param1 (int): the client socket
          param2 (string): the message to send
       Returns:
          None
    """
    clientSocket.send(serverMessage)
    return;

def serverQuit(clientSocket):
    """
       The function shuts down the connection
       to the client after the server initiates
       a quit process.

       Args:
          param1 (int): the client socket
       Returns:
          a bool to stop the chatting loop
    """
    print "Disconnecting from client..."
    clientSocket.send("quit")
    clientSocket.close()
    print "Ready for a new connection."
    chatting = False
    return chatting;

def clientQuit(clientSocket):
    """
       The functions shuts down the connection
       to the client after the client initiates
       a quit process.

       Args:
          param1 (int): the client socket
       Returns:
          a bool to stop the chatting loop
    """
    print "Quit message received, disconnecting from client..."
    print "Ready for a new connection."
    clientSocket.close()
    chatting = False
    return chatting;

if __name__ == "__main__":
    try:
        main()
    #to exit the program safely after ^C is received
    except KeyboardInterrupt:
        print "\n^C command received, shutting down server"
        sys.exit(0)
