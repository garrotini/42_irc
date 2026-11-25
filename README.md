
# ft_irc

## basic usage

server side:
`./irc_serv <port> <passwd>`

client side:
`telnet localhost <port>` or `nc -C localhost <port>`

for fast tests use:
`make t`, `make a` and `make b` (to compile the program and immediatly run it on 5555-5557 ports)
(you will notice you'll get out of available sockets pretty soon!)

to close the program on the client side, type `:q!` just like vim :)

## notes:

- very basic program (parsing is still a joke!)
- `make t` rule re-compiles and runs the program as `./ircserver 5555 pass`
- `make c` rule creates a standard client as `telnet localhost 5555`
- updated: multiple clients can connect to server, send messages to it and be broadcast to everyone in the chat
<<<<<<< HEAD
- server_init() is separated from main function (on server.cpp)
=======
- updated: every client can leave the server gracefully with `:q!` and server continues to serve to everyone else
>>>>>>> cgarrote

## next:

- start to organize classes: Server, Client (and Channel later on)
<<<<<<< HEAD
- separate the clients section from main function
- parser (that returns a int=port and string=passwd, that will be used in server_init() )
=======
    - read Server.cpp (there's cues/tips there!)
    - Client hpp and cpp has been created but still not in use

- first argument parser/check: `./ircserver 5555 <passwd>`
>>>>>>> cgarrote

## sources:

- man pages for socket, bind, listen, accept, write, send, recv
	- take in consideration the libraries! `man 2 socket` != `man 7 socket`
- [man 2 bind](https://man7.org/linux/man-pages/man2/bind.2.html)
	- here there's a complete code demo on how to socket(), bind(), listen(), accept(), close()
- [man 7 socket](https://man7.org/linux/man-pages/man7/socket.7.html)
	- it has a good resume on all syscalls related to networking
- [yt - Tony btw - Basics of a webserver](https://youtu.be/RwSv24LpRJ0)
	- basics of a webserver, but nice explanation and search through man pages
- [yt - Sloan Kelly - Creating a TCP Server in C++ Linux](https://youtu.be/cNdlrbZSkyQ)
	- creates a basic 1 client to server chat
- [yt - Sloan Kelly - Multiple Chat Clients: One Thread](https://youtu.be/dquxuXeZXgo)
	- probably next step on our program!
- [yt - Jacob Sorber - how one thread listens to many sockers with select()](https://youtu.be/Y6pFtgRdUts)
    - explanation on select(2)

- [yt - Jacob Sorber - getaddrinfo](https://youtu.be/yN6EGfv5Dew)
    - we might need it for later!

