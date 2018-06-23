// $Id: main.cpp,v 1.9 2016-07-20 21:00:33-07 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

//=====================================================================

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;
//using str_str_itr = listmap<string,string>iterator;
//=====================================================================

//=====================================================================

string split(string line) {   
    size_t first = line.find_first_not_of(' ');
    size_t last = line.find_last_not_of(' ');
    if(first == string::npos ) return "";
    string res = line.substr(first, last - first + 1);
    return res; 
}
//=====================================================================


//=====================================================================

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
          case '@':
              traceflags::setflags (optarg);
              break;
          default:
              complain() << "-" << char (optopt) << ": invalid option"
                         << endl;
              break;
      }
   }
}
//=====================================================================


void parser(string line, str_str_map& map){
    line = split(line);
    string key = ""; 
    string value = "";
    
    if ((line[0] == '#' or line.size() == 0)) {
        return;
    }
    size_t pos = line.find_first_of('=');
    bool atFront = line.find_first_of('=') == 0;
    bool atEnd = line.find_first_of('=') == line.length()-1;
    str_str_map::iterator itor;

    if(pos == string::npos){
        key = line;
        itor = map.find(key);
        if(itor != map.end() and itor != str_str_map::iterator()){
            cout << itor->first << " = " << itor->second 
                             << endl;
        }
        else{
            cout << key <<": key not found." << endl; 
        }
    }
    // ' = '
    else if (line.size() == 1 and line[0] == '='){
        for(auto itor = map.begin(); itor != map.end(); ++itor){
            cout << itor->first << " = " << itor->second 
                             << endl;
        }
    }
    // case 'key =' 
    else if (!atFront and atEnd){
        key = split(line.substr(0,line.length()-1));
        for(auto itor = map.begin(); itor != map.end(); ++itor){
            if(key == itor->first){
                map.erase(itor);
                break;
            }
        }
    }
    // 'Key = value'
    else if(!atFront and !atEnd){
        key = split(line.substr(0,pos));
        value = split(line.substr(pos+1));
        str_str_pair newPair(key,value);

        map.insert(newPair);
        cout << key << " = " << value << endl;
    }
    //case '= value'
    else if (atFront and !atEnd){
        value = split(line.substr(1));
        for(auto itor = map.begin();itor != map.end(); ++itor){
            if(value == itor->second){
               cout << itor->first << " = " << itor->second 
                                << endl; 
            }
        }
    }
    else { 
        cerr <<  "Parser Error" << endl;
        /*catch (exception& error) {
            //cout << "Error in parser! " << endl;
         complain() << error.what() << endl;
    }*/
    }
    //}
}
//=====================================================================

//=====================================================================

int main (int argc, char** argv) {
    scan_options (argc, argv);
    str_str_map test;    
    string line = "";
    int count{0};

    if(argc == 1){
        while( getline(cin,line)){
            if(cin.eof()) break;
        }
        ++count;
        cout << count << ": " << line << endl;
        parser(line, test);
    }
    for(auto itor = 1; itor < argc; ++itor){
        if(argv[itor] == std::string("-")) {
            while(getline(cin,line)){
                if(cin.eof()) break;
                ++count;
                cout << argv[itor] << ": " << count 
                     << ": " << line << endl;
                parser(line, test);
            }
        }
        else {
            ifstream fstream (argv[itor]);
            if(fstream.fail()) cout << "no such file " << endl;
            while(getline(fstream, line)){
                ++count;
                cout << argv[itor] << ": " << count 
                     << ": " << line << endl;
                parser(line, test);    
            }
        }
    }
    /*//sys_info::set_execname (argv[0]);
    scan_options (argc, argv);

    str_str_map test;
    bool need_echo = true;
    bool has_equal_sign = false;
    string key = "";
    string value = "";

  //try {
    for (;;) {
    has_equal_sign = false;
    key = "";
    value = "";
     try {
        // Read a line, break at EOF, and echo print the prompt
        // if one is needed.
        string line;
        getline (cin, line);
        if (cin.eof()) {
            if (need_echo) cout << "^D";
            cout << endl;
            break;
        }
        if (need_echo) cout << line << endl;

        // Split the line into words and lookup the appropriate
        // function.  Complain or call it.
        if(line.size() > 0)
        {
              if(line[0] == '#')
              {
                    // Do nothing, a comment.
              }
              else
              {
                if(line[0] == '=')
                {
                  if(line.size() == 1)
{
// print all kv pairs
cout << "print all kv pairs\n";
for(str_str_map::iterator it = test.begin(); it != test.end(); 
        ++it)
{
cout << it->first << " " << it->second << endl; 
}
}
else
                  {
                    for(unsigned int i = 1; i < line.size(); i++)
                    {
                      value.append(to_string(line[i]));
                    }
// call function to get all kv pairs with same value
cout << "call function to get all kv pairs with same value\n";
for(str_str_map::iterator it = test.begin(); it != test.end(); 
        ++it)
{
  if(it->second == value)
  {
    cout << it->first << " " << it->second << endl; 
  }
}
}
}
else
{
for(unsigned int i = 0; i < line.size(); i++)
{
if(line[i] == '=')
{
  has_equal_sign = true;
  continue;
}
if(!has_equal_sign)
{
  key.append(to_string(line[i]));
}
else
{
  value.append(to_string(line[i]));
}
}
if(!has_equal_sign)
{
//call function to output kv pair with key
auto item = test.find(key);
if(item != str_str_map::iterator()) // key not found
{
    cout << key << " " << item->second << endl; 
}
else // key found
{
  cout << key << ": not found" << endl;
}                      
}
else
{
if(value != "" && value != " ")
{
  //call function to assign kv pair
  test.insert(xpair <const string, string> (key,value));
}
else
{
  //call function to delete kv pair with key
  test.erase(test.find(key));
}
}
                }
              }
        }
    }
    catch (exception& error) {
        // If there is a problem discovered in any function, an
        // exn is thrown and printed here.
        complain() << error.what() << endl;
    }*/
   // }

    for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
      str_str_pair pair (*argp, to_string<int> (argp - argv));
      cout << "Before insert: " << pair << endl;
      test.insert (pair);
    }
    return EXIT_SUCCESS;
}
//=====================================================================
