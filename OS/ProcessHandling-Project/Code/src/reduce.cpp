#include "../includes/defines.hpp"

using namespace std;

void parse_and_add(char inbuf[PIPE_LEN], vector<pair<string, int>>& vec){
    int i = 0;
    while (i < PIPE_LEN && inbuf[i] != '\0'){
        string s = "";
        int occ = 0;
        while (i < PIPE_LEN && inbuf[i] != ':'){
            s += inbuf[i];
            i++;
        }
        i++;
        while (i < PIPE_LEN && inbuf[i] != ','){
            occ = 10 * occ + (inbuf[i] - '0');
            i++;
        }
        bool is_rep = false;
        for (unsigned long int j = 0; j < vec.size(); j++){
            if (vec[j].first == s){
                vec[j].second += occ;
                is_rep = true;
                break;
            }
        }
        if (!is_rep){
            vec.push_back(make_pair(s, occ));
        }
        i++;
    }
}

int main(int argc, char* argv[])
{
    vector<pair<string, int>> vec;   
    int CSV_FILES_NO = *argv[1] - '0';
    int p = stoi(argv[2]);

    for (int i = 1; i <= CSV_FILES_NO; i++){
        string fifo_string = "";
        fifo_string += i + '0';
        const char* my_fifo = fifo_string.c_str();
        mkfifo(my_fifo, 0666);
        int new_fd = open(my_fifo, O_RDONLY);
        char inbuf[PIPE_LEN];
        read(new_fd, inbuf, PIPE_LEN);
        parse_and_add(inbuf, vec);
        close(new_fd);
    }

    string ans = "";
    for (unsigned long int i = 0; i < vec.size(); i++){
        ans += vec[i].first;
        ans += ':';
        ans += to_string(vec[i].second);
        ans += ',';
    }
    char inbuf[PIPE_LEN];
    for (unsigned long int i = 0; i < ans.length(); i++) inbuf[i] = ans[i];
    
    write(p, inbuf, PIPE_LEN);
    return 0;
}