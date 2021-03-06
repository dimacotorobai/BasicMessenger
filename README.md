# BasicMessenger
Very basic messenger which works by having a server and clients.
The clients will connect to the server and then the server will forward the message
send from a certain client to all the rest.
Closing the clients will cause a the server to send an empty string to the rest of the clients
due to not closing the socket before terminating the clients connection.
