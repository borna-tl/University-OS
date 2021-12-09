#include "../includes/defines.hpp"
using namespace std;

int main(int argc, char* argv[])
{
    vector <pair<string, int>> vec;
    string file_name = "./testcases/";
    file_name = file_name + argv[1];
    file_name = file_name + ".csv";
    ifstream inp(file_name);
    string line;
    getline(inp,line);
    stringstream lineStream(line);
    string cell;

    while(getline(lineStream, cell, ',')) {
        bool is_rep = false;
        for (unsigned long int i = 0; i < vec.size(); i++){
            if (vec[i].first == cell){
                vec[i].second ++;
                is_rep = true;
                break;
            }
        }
        if (!is_rep){
            vec.push_back(make_pair(cell, 1));
        }
    }
    string ans = "";
    for (unsigned long int i = 0; i < vec.size(); i++){
        ans += vec[i].first;
        ans += ':';
        ans += to_string(vec[i].second);
        ans += ',';
    }
    
    string fifo_string = "";
    fifo_string += *argv[1];
    const char* my_fifo = fifo_string.c_str();
    mkfifo(my_fifo, 0666);
    
    char inbuf[PIPE_LEN];
    for (unsigned long int i = 0; i < ans.length(); i++) inbuf[i] = ans[i];
    
    int new_fd = open(my_fifo, O_WRONLY);
    write(new_fd, inbuf, PIPE_LEN);
    close(new_fd);
    return 0;
}