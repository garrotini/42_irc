#ifndef SERVER_HPP
#define SERVER_HPP

#include <asm-generic/socket.h>
#include <cstdio>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <sstream>

#define BUFF_SIZE 4096

int server_init(int port, std::string passwd);


#endif

