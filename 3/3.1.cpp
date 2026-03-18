#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_INPUT 100
using namespace std;

vector<string> tokenize(const string &input){
    vector<string> tokens;
    size_t start =0;
    size_t end;
    while((end=input.find(' ',start))!=string::npos){
        //szuka spacji
        if (end!=start){
            //pomijamy podwojne
            tokens.push_back(input.substr(start,end-start));
        }
        start=end+1;
    }
    if (start < input.length()){
        //ostatni fragment
        tokens.push_back(input.substr(start));
    }
    return tokens;
}

void help(){
    cout << "help - wyswietl wszystkie komendy" << endl;
    cout << "pwd - wyswietl biezacy katalog" << endl;
    cout << "run - run program <params>, uruchamia program" << endl;
    cout << "bg - bg program <params>, uruchamia program w tle"<<endl;
}
void pwd(){
    char path[1024];
    if (getcwd(path,sizeof(path))!=NULL){
        cout << "Biezacy katalog" << path << endl;
    }
    else cout << "blad" << endl;

}
void run(const vector<string> &args){
    if (args.size()<2){
        cout << "blad";
        return;
    }
    pid_t pid =fork(); //rozbija kod na 2 procesy
    if (pid == 0){
        vector<char*> exec_args; //execvp() oczekuje char*

        for (size_t i=1; args.size()>i;i++){
            const char* c_style_string = args[i].c_str();
            char* non_const_string = const_cast<char*>(c_style_string);
            exec_args.push_back(non_const_string);
        }
        exec_args.push_back(nullptr); //na koniec oczekuje null

        execvp(exec_args[0],exec_args.data()); //jak działa to nie wykonuje dalej
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid>0){
        int status;
        waitpid(pid,&status,0);
        cout << "Kod zakonczenia procesu: " << WEXITSTATUS(status) << endl;
    }
    else perror("fork");

}
void bg(const vector<string> &args){
    if (args.size()<2){
        cout << "blad";
        return;
    }
    pid_t pid =fork(); //rozbija kod na 2 procesy
    if (pid == 0){
        vector<char*> exec_args; //execvp() oczekuje char*

        for (size_t i=1; args.size()>i;i++){
            const char* c_style_string = args[i].c_str();
            char* non_const_string = const_cast<char*>(c_style_string);
            exec_args.push_back(non_const_string);
        }
        exec_args.push_back(nullptr); //na koniec oczekuje null

        execvp(exec_args[0],exec_args.data()); //jak działa to nie wykonuje dalej
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    else if (pid>0){
        //tak samo jak run, ale nie czeka na zakończenie
        cout << "Uruchomiono w tle PID: " << pid << endl;
    }
    else perror("fork");
}

void process_input(string input){
   vector<string> args = tokenize(input);
   if (args.empty())return;
   else if (args[0]=="help") help();
   else if (args[0]=="pwd") pwd();
   else if (args[0]=="run") run(args);
   else if (args[0]=="exit")exit(0);
   else if (args[0]=="bg")bg(args);
   else cout << "Nieznana komenda: "<<args[0] << endl;
}

int main () {
    string input;

    while (true){
        cout << ">>>";
        if (!getline(cin,input)){
            cout << input << endl;
            break;
        }
        process_input(input);
    }
}