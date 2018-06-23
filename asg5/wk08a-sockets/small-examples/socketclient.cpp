// $Id: socketclient.cpp,v 1.5 2016-07-29 14:17:42-07 - - $

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
using namespace std;

#include "../simple-sockets/sockets.h"

template <typename Type>
Type from_string (const string &str) { 
   stringstream stream; 
   stream << str; 
   Type result; 
   if (not (stream >> result and stream.eof())) {
      throw domain_error (string (typeid (Type).name())
            + " from_string (" + str + ")"); 
   }
   return result; 
}

void echo_server (string hostname, in_port_t port) {
   client_socket serv (hostname, port);
   for (;;) {
      string buffer; 
      int nbytes = serv.recv (buffer);
      if (nbytes == 0) break;
      cout << buffer; 
   }
}

int main (int argc, char** argv) {
   if (argc != 3) exit (1);
   echo_server (argv[1], from_string<in_port_t> (argv[2]));
}

#include "../simple-sockets/sockets.cpp"
