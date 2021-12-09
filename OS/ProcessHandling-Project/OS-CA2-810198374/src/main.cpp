#include "../includes/defines.hpp"

using namespace std;

vector<string> get_files(){
    vector<string> file_names;
    int MAIN_PROCESS_ID = getpid();
    DIR *d;
    struct dirent *dir;
    d = opendir("./testcases");
    if (d) {
        while ((dir = readdir(d)) != NULL) {//iterating on all files
            char type[5];
            char* file_name = dir->d_name;
            int position = strlen(dir->d_name) - 5;
            if (position < 0)
                continue;
            for (int i = 0; i < 5; i++)
                type[i] = file_name[position + i];
            if (type[1] == '.' && type[2] == 'c' && type[3] == 's' && type[4] == 'v'){
                if (getpid() != MAIN_PROCESS_ID)
                    continue;
                file_names.push_back(dir->d_name);
            }
        }
        closedir(d);
    }
    return file_names;
}

int main() {
    vector<string> csv_files = get_files();
    string REDUCE_FILE = "reduce.out";
    string MAPS_FILE = "maps.out";

    int p[2], fd[MAX_FILE_NO][2], pid[MAX_FILE_NO];
    if (pipe(p) < 0)
        exit(1);
    pid[0] = fork();
    
    if (pid[0] > 0){ //map processes
        close(p[1]); //close the write descriptor
        for (unsigned long int i = 0; i < csv_files.size(); i++){
            int file_index = csv_files[i][0] - '0';
            pipe(fd[file_index]);
            pid[file_index] = fork();
            if (pid[file_index] > 0){ //main
                close(fd[file_index][0]); //close read descriptor
                char str[PIPE_LEN];
                sprintf(str, "%d", file_index);
                write(fd[file_index][1], str, PIPE_LEN);
                
                close(fd[file_index][1]); //close write descriptor
            }
            else{ //child
                close(fd[file_index][1]); //close write descriptor
                char id[PIPE_LEN];
                read(fd[file_index][0], id, PIPE_LEN);
                
                close(fd[file_index][0]); //close the read-descriptor

                
                string new_id = id;
                char* argv_list[] = {(char*)MAPS_FILE.c_str(), (char*)new_id.c_str(), NULL};    
                execv(argv_list[0], argv_list); 
                
            }
        }
        char inbuf[PIPE_LEN];
        read(p[0], inbuf, PIPE_LEN);
        ofstream outp;
        outp.open ("output.csv");
        outp << inbuf;
        outp.close();
        close(p[0]); //close the read descriptor
    }    
    else { //reduce process
        close(p[0]); //close the read descriptor
        sleep(10);
        
        string argv1 = to_string(csv_files.size());
        string argv2 = to_string(p[1]);
        char* argv_list[] = {(char*)REDUCE_FILE.c_str(),
                            (char*)argv1.c_str(), (char*)argv2.c_str(), NULL};    
        execv(argv_list[0], argv_list); 
        
        close(p[1]); //close the write descriptor
        
    }
    return 0;
}