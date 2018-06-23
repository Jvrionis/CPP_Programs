// $Id: cix.cpp,v 1.4 2016-05-09 16:01:56-07 - - $

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unordered_map>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream log (cout);
struct cix_exit: public exception {};

//get, put,rm 
unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS  },
   {"get" , cix_command::GET },
   {"put" , cix_command::PUT },
   {"rm"  , cix_command::RM  },
};

void cix_help() {
   static const vector<string> help = {
      "exit         - Exit the program.  Equivalent to EOF.",
      "get filename - Copy remote file to local host.",
      "help         - Print help summary.",
      "ls           - List names of files on remote server.",
      "put filename - Copy local file to remote host.",
      "rm filename  - Remove file from remote server.",
   };
   for (const auto& line: help) cout << line << endl;
}

// LS -> LSOUT
void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      log << "sent LS, server did not return LSOUT" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.nbytes + 1];
      recv_packet (server, buffer, header.nbytes);
      log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer;
   }
}

// GET -> FILE
void cix_get (client_socket& server, string filename) {
   cix_header header;
   header.command = cix_command::GET;
   //strcpy (header.filename, filename.c_str());
   // compiler complained about strcopy and said to you snprintf
   snprintf(header.filename, filename.length()+1, filename.c_str());
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != cix_command::FILE) {
      log << filename << " not on server" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.nbytes + 1];
      
      log << "Waiting for " << header.nbytes << " bytes" << endl;
      recv_packet (server, buffer, header.nbytes);
      log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer;
      
     // ofstream out(header.filename, ios::binary);
      std::ofstream out(header.filename, std::ofstream::binary);
      out.write(buffer, header.nbytes);
       out.close();
        cout << "get finished..." << endl;
   }
}

void cix_put (client_socket& server, string filename) {
    cix_header header;
   // header.command = cix_command::PUT;
    
    snprintf(header.filename, filename.length()+1, filename.c_str());
    
    // cd to open
    ifstream file(filename, ifstream::binary);

    if(file.is_open()) {
        // length of file
        file.seekg(0, file.end);
        int length = file.tellg();
        file.seekg(0, file.beg);
        char buffer[length];

        file.read(buffer,length);
        
        header.command = cix_command::PUT;
        header.nbytes = length;
        send_packet (server, &header, sizeof header);
        send_packet (server, buffer, length);
        recv_packet (server, &header, sizeof header);

    }else{
        log << filename <<  " is not on server"<< endl;
    }
    if (header.command == cix_command::NAK){
        log << "NAK: PUT failed " << endl;
    }    
    if (header.command == cix_command::ACK){
        log << "ACK received " << filename << " put on server" 
             << endl;
    }
    file.close();
}

// RM -> ACK
void cix_rm (client_socket& server, string filename) {
   cix_header header;
   //strcpy (header.filename, filename);
   snprintf( header.filename, filename.length()+1, filename.c_str());
   header.command = cix_command::RM;
   header.nbytes = 0;

   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;


  if(header.command == cix_command::NAK){
      log << "NAK received, " << filename << " not deleted" <<endl;
  }
  if(header.command == cix_command::ACK){
      log << "ACK received, " << filename << " deleted" <<endl;
  }
}

void usage() {
   cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
   throw cix_exit();
}


int main (int argc, char** argv) {
   log.execname (basename (argv[0]));
   log << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host;
   in_port_t port = 65535;
  // in_port_t port;
  // string host = "localhost";
   if (args.size() == 1) {    
       host = "localhost";
       port = get_cix_server_port (args, 0);
       log << to_string (hostinfo()) << endl; 
   }else{
      host = get_cix_server_host (args, 0);
      port = get_cix_server_port (args, 1);
      log << to_string (hostinfo()) << endl;
   }
  // string host = get_cix_server_host (args, 0);
  //  in_port_t port = get_cix_server_port (args, 1);
  // log << to_string (hostinfo()) << endl;
   try {
      log << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      log << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         vector<string> vec;
         std::istringstream sstring(line);
         string tok;
         while(getline (sstring,tok, ' ')) vec.push_back(tok);
         log << "command " << line << endl;
         const auto& itor = command_map.find (vec[0]);
         
         
         //const auto& itor = command_map.find (line);
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         switch (cmd) {
            case cix_command::EXIT:
               throw cix_exit();
               break;
            case cix_command::HELP:
               cix_help();
               break;
            case cix_command::LS:
               cix_ls (server);
               break;
            case cix_command::PUT:
               cix_put (server, vec[1]);
               vec.erase(vec.begin(),vec.end());
               break;  
            case cix_command::GET:
               cix_get (server, vec[1]);
               vec.erase(vec.begin(),vec.end());
               break;
            case cix_command::RM:
               cix_rm (server, vec[1]);
               vec.erase(vec.begin(),vec.end());
               break;   
            default:
               log << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      log << error.what() << endl;
   }catch (cix_exit& error) {
      log << "caught cix_exit" << endl;
   }
   log << "finishing" << endl;
   return 0;
}

