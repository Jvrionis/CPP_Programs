// $Id: commands.cpp,v 1.16 2016-01-14 16:10:40-08 - - $
#include <iostream>
#include <stdio.h>
#include <string>
#include <cstddef>

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}
//---------------------------------------------------------------------
void fn_cat (inode_state& state, const wordvec& words){
    // Because if cat needs an argument in this prog
    if (words.size() == 1){
       throw command_error("fn_cat: no ags specified");
    }
    else if (words.size() > 1) {
        shared_ptr <directory> content = 
    dynamic_pointer_cast <directory> (state.get_cwd()->get_contents());
    for(unsigned int i = 1; i < words.size(); i++) {
        inode_ptr file_content = content->get_file(words[i]);
        if( file_content == inode_ptr()) {
            cout << "cat: " 
                 << words[i] << " no such file exists.\n";
      }
      else {
      shared_ptr <plain_file> file = 
      dynamic_pointer_cast<plain_file>(file_content->get_contents());
            cout << file->readfile() << endl;
      }
   }
   }
   else {
        cout << "cat: expected a file to cat.\n";
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}
//---------------------------------------------------------------------
void navigate(bool& success, shared_ptr <directory> current_dir, 
                                             const wordvec& dirnames){
    inode_ptr next_file = dynamic_pointer_cast <inode> (current_dir);
    success = true;
    for(unsigned int i = 0; i < dirnames.size(); i++){
      next_file =
      dynamic_pointer_cast<inode>(current_dir->get_file(dirnames[i]));
        if(next_file == inode_ptr()){
            success = false;
            break;
        }
        if(next_file->get_type()) { // file specified is a directory
        current_dir = 
        dynamic_pointer_cast <directory>(current_dir->get_file
                                      (dirnames[i])->get_contents());
        }
        else{
            success = false;
            break;
        }
    }
}
//---------------------------------------------------------------------
void fn_cd (inode_state& state, const wordvec& words){
    if(words.size() == 1){
        state.set_cwd(state.get_root());
    }
    else if(words.size() == 2){
        shared_ptr <directory> current_dir
              = dynamic_pointer_cast<directory>
                    (state.get_cwd()->get_contents());
        inode_ptr next_file = 
            dynamic_pointer_cast <inode> (current_dir);
        wordvec dirnames = split (words[1], "/");
        bool successful = true;
        for(unsigned int i = 0; i < dirnames.size(); i++){
        next_file = 
           dynamic_pointer_cast <inode>
                    (current_dir->get_file(dirnames[i]));
            if(next_file == inode_ptr()){
                successful = false;
                break;
            } // file specified is a directory
            if(next_file->get_type()){ 
              current_dir = dynamic_pointer_cast <directory>
                  (current_dir->get_file(dirnames[i])->get_contents());
            }
            else {
                successful = false;
                break;
            }
        }
        if(successful){
            state.set_cwd(next_file);
        }
        else{
            cout << "cd: cannot navigate to specified dir...\n";
        }
        if(words[1] == "/")
        {
            state.set_cwd(state.get_root());
        }
    }
    else{
        cout << "cd: invalid argumenets given\n";
    }
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

//---------------------------------------------------------------------
void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}

//---------------------------------------------------------------------
//TODO: remove all inodes...
void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   shared_ptr <directory> content = 
        dynamic_pointer_cast <directory> 
                (state.get_cwd()->get_contents());
    content->clear_files();
   throw ysh_exit();
}

//---------------------------------------------------------------------
void fn_ls (inode_state& state, const wordvec& words){
    if(words.size() <= 2){
    shared_ptr <directory> content = 
        dynamic_pointer_cast <directory> 
                (state.get_cwd()->get_contents());
    cout << content->printentry();
    }
    else throw command_error("fn_ls:invalid number of args");
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

//---------------------------------------------------------------------
void fn_lsr (inode_state& state, const wordvec& words){
    shared_ptr <directory> content = 
            dynamic_pointer_cast<directory>(
                state.get_cwd()->get_contents());
    for(unsigned int i = 1; i < words.size(); i++)
    {
        wordvec parsed = split(words[i], "/");
        wordvec::iterator it = parsed.begin();
        content = 
            dynamic_pointer_cast<directory>(
                state.get_cwd()->get_contents());
        unsigned int size = 0;
        inode_ptr file;
        if(words[i] == "/")
        {
            shared_ptr <directory> content = 
            dynamic_pointer_cast<directory>
                (state.get_root()->get_contents());
            vector <int> *visited = new vector <int> (0);
            string output = content->printentryrec("", visited);
            cout << output.substr(output.rfind("/:"), 
                                            output.length() - 1);
            delete(visited);
        }
        else
        {
            try {
                while(size < parsed.size())
                {
                    if(content->file_exists(*it))
                    {
                        file = content->get_file(*it);
                    }
                    else
                    {
      throw command_error("fn_lsr: directory does not exist " + *it);
                    }
                    if(!file->get_type())
                    {
   throw command_error("fn_lsr: expected a directory and not a file");
                    }
                    else
                    {
                        it++;
                        size++;
                        content = dynamic_pointer_cast<directory>
                                              (file->get_contents());
                    }
                }
                content = 
                dynamic_pointer_cast<directory>(file->get_contents());
                vector <int> *visited = new vector<int>
                                               (file->get_inode_nr());
                string output = content->printentryrec("", visited);
                cout << output.substr(output.rfind(
                        content->getfilename() + ":"),
                                                 output.length() - 1);
                delete(visited);
            }
            catch(std::out_of_range) {
                cout << "fn_lsr: directory does not exist " + *it 
                     << endl;
            }
        }
        size = 0;
    }
    if( words.size() == 0 ){
        throw command_error("fn_lsr:invalid number of args");
    }
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

//---------------------------------------------------------------------
void fn_make (inode_state& state, const wordvec& words){
    if(words.size() > 2) {
        wordvec parsed = split(words[1], "/");
        wordvec::iterator it = parsed.begin();
        shared_ptr <directory> content = 
            dynamic_pointer_cast<directory>(
                state.get_cwd()->get_contents());
        unsigned int size = 0;
        if(parsed.size() < 2 )
        {
            if(content->get_file(words[1])  == inode_ptr()){
                content->createfile(words[1], 
                    wordvec (words.begin() + 2, words.end()));
            }
            else {
                cout << "make: " << words[1] 
                    << " file already exists.\n";
            }
        }
        else
        {
            try {
                while(size < (parsed.size() - 1 ))
                {
                    inode_ptr file;
                    if(content->file_exists(*it))
                    {
                        file = content->get_file(*it);
                    }
                    else
                    {
           throw command_error("fn_make: directory does not exist");
                    }
                    if(!file->get_type())
                    {
throw command_error("fn_make: expected a directory and not a file");
                    }
                    else
                    {
                        it++;
                        size++;
                        content = dynamic_pointer_cast<directory>
                                               (file->get_contents());
                    }
                }
                if(content->get_file(*it)  == inode_ptr()) {
                    content->createfile(*it, 
                        wordvec (words.begin() + 2, words.end()));
                }
                else {
                    cout << "make: " << words[1] 
                         << " file already exists.\n";
                }
            }
            catch(std::out_of_range) {
                throw command_error("fn_make:cannot find directory");
            }
        }
    }
    else {
        cout << "make: expected more than one argument\n";
    }
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

//---------------------------------------------------------------------
void fn_mkdir (inode_state& state, const wordvec& words){
   if(words.size() == 1) throw command_error("fn_mkdir: no args");
   wordvec parsed = split(words[1], "/");
        wordvec::iterator it = parsed.begin();
        shared_ptr <directory> content = 
            dynamic_pointer_cast<directory>(
                state.get_cwd()->get_contents());
    unsigned int size = 0;
    if(words.size() == 2) {
        if(parsed.size() < 2 )
        {
            if(!content->file_exists(words[1])) {
                content->createdir(words[1]);
            }
            else {
                 throw command_error( "fn_mkdir: " + words[1] 
                        + " file already exists.");
            }
        }
        else
        {
            try {
                while(size < parsed.size() - 1)
                {
                    inode_ptr file;
                    if(content->file_exists(*it))
                    {
                        file = content->get_file(*it);
                    }
                    else
                    {
   throw command_error("fn_mkdir: directory does not exist " + *it);
                    }
                    if(!file->get_type())
                    {
  throw command_error("fn_mkdir: expected a directory and not a file");
                    }
                    else
                    {
                        it++;
                        size++;
                        content = dynamic_pointer_cast<directory>
                                            (file->get_contents());
                    }
                }
                if(!content->file_exists(*it)) {
                    content->createdir(*it);
                }
                else {
                    throw command_error( "fn_mkdir: " + *it
                            + " file already exists.");
                }
            }
            catch(std::out_of_range)
            {
                throw command_error("fn_mkdir: "
                                   "cannot find directory");
            }
        }
    }
    else throw command_error("fn_mkdir: invalid numbr of args");
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

//---------------------------------------------------------------------
void fn_prompt (inode_state& state, const wordvec& words){
    string prompt = "";
    if(words.size() >= 2){
        for(unsigned int i = 0; i < words.size(); i++)
        {
            prompt += words[i];
            if(i < (words.size() - 1))
            {
                prompt += " ";
            }
        }
        state.set_prompt(prompt);
    }
    else{
        cout << "prompt: wrong number of arguments passed\n";
    }
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

//---------------------------------------------------------------------
string get_name(shared_ptr <directory> current){
    string output_string = current->getfilename();
    if(output_string.size() == 0){
       output_string = "/";
    }
    return output_string;
}

//---------------------------------------------------------------------
void fn_pwd (inode_state& state, const wordvec& words){
    if(words.size()==1){ 
        shared_ptr <directory> current 
                 = dynamic_pointer_cast<directory> 
                    (state.get_cwd()->get_contents());
        string output_string = get_name(current);
        cout << output_string << endl;
    }
    else throw command_error("fn_pwd:invalid num of args"); 
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

//---------------------------------------------------------------------
void fn_rm (inode_state& state, const wordvec& words){
    /*state.remove(state.get_cwd(), words);*/
    shared_ptr <directory> current 
        = dynamic_pointer_cast<directory>
            (state.get_cwd()->get_contents());
    if(words.size() == 2){
        auto element = current->get_file(words[1]);
        auto dir 
          = dynamic_pointer_cast<directory> (element->get_contents());
        if(!element->get_type() ||  dir->num_files() == 0){
            if(element->get_type())
            {
                dir->clear_files();
            }
            current->remove(words[1]);
        }
        else {
            cout << "rm: " << words[1] 
                 << ": is a non-empty directory.\n";
        }
    }
    DEBUGF ('c', state);
    DEBUGF ('c', words);
}

//---------------------------------------------------------------------
void fn_rmr (inode_state& state, const wordvec& words){
    DEBUGF ('c', state);
    DEBUGF ('c', words);
    shared_ptr <directory> content = 
            dynamic_pointer_cast<directory>(
                state.get_cwd()->get_contents());
    for(unsigned int i = 1; i < words.size(); i++)
    {
        wordvec parsed = split(words[i], "/");
        wordvec::iterator it = parsed.begin();
        content = 
            dynamic_pointer_cast<directory>(
                state.get_cwd()->get_contents());
        unsigned int size = 0;
        inode_ptr file;
        if(parsed.size() <= 2 )
        {
            if(content->file_exists(*parsed.begin())) {
                shared_ptr<directory> oldDir = content;
                content = dynamic_pointer_cast<directory>
                    (content->get_file(words[i])->get_contents());
                content->clear_files();
                oldDir->remove(*parsed.begin());
            }
            else {
                 cout << "fn_rmr: " + words[i] 
                        + " does not exists.\n";
            }
        }
        else
        {
            try {
                while(size < parsed.size() - 1)
                {
                    if(content->file_exists(*it))
                    {
                        file = content->get_file(*it);
                    }
                    else
                    {
      throw command_error("fn_rmr: directory does not exist " + *it);
                    }
                    if(!file->get_type())
                    {
  throw command_error("fn_rmr: expected a directory and not a file");
                    }
                    else
                    {
                        it++;
                        size++;
                        content = dynamic_pointer_cast<directory> 
                                              (file->get_contents());
                    }
                }
                if(content->file_exists(*it)) {
                    shared_ptr<directory> oldDir = content;
                    content = dynamic_pointer_cast<directory>
                        (content->get_file(words[i])->get_contents());
                    content->clear_files();
                    oldDir->remove(*it);
                }
                else {
                    cout << "fn_rmr: " + *it 
                            + " does not exists.\n";
                }
            }
            catch(std::out_of_range) {
                cout << "fn_rmr: " + words[i] 
                        + " cannot find directory";
            }
        }
    }
}

