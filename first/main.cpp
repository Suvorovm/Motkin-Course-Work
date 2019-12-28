#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;
const string pidOwnFileName = "/Pips/Pid1.p";
const string pidFileSecondProcess = "/Pips/Pid2.p";
const string generalPipeName = "/Pips/GeneralPipe.p";
string generalPath;
int myPid = 0;
int anotherProcessPid = 0;
bool is_number(const std::string& s);
string ReadFromPipe(const string &path);
void InitSignals();
void SigUser1Handler();
bool fileExists(const char* file);

void WriteOwnPid(const string &pathPid);
void WriteToPipe(const string &path, const string &value);

void SendMessage(const string &tempResultOfUserCommand);

int main() {
chdir("..");
chdir("..");
InitSignals();

string pathOwnPid = get_current_dir_name();
pathOwnPid += pidOwnFileName;

string pathAnotherProcessPid = get_current_dir_name();
pathAnotherProcessPid += pidFileSecondProcess ;

generalPath = get_current_dir_name();
generalPath += generalPipeName;

myPid = getpid();
cout<<"My PID is "<< myPid<<endl;
WriteOwnPid(pathOwnPid);
string  tem = ReadFromPipe(pathAnotherProcessPid);
anotherProcessPid = stoi(tem);
cout<< "PID второго процесса = "<<anotherProcessPid<<endl;
string tempResultOfUserCommand ;
char word[256];
int resultCode =1;
while (true){
    int cod = 0;
    cout << "\nВведите 1 для отправки сообщения \nВведите 2 для завершения\n ";
    fgets(word, sizeof(word), stdin);
    tempResultOfUserCommand.clear();
    tempResultOfUserCommand = word;
    tempResultOfUserCommand.erase(std::remove(tempResultOfUserCommand.begin(), tempResultOfUserCommand.end(), '\n'), tempResultOfUserCommand.end());
    if(!is_number(tempResultOfUserCommand)){
        continue;
    }
    cod = stoi(tempResultOfUserCommand);
    switch(cod){
        case 1:
            cout << "\n Введите сообщение\n";
            fgets(word, sizeof(word), stdin);
            tempResultOfUserCommand = word;
            SendMessage(tempResultOfUserCommand);
            break;
        case 2:
            exit(0);
            break;
        default:
            cout<<"Не поддержтиваемы тип";
            break;
    }

}
return 0;
}

void SendMessage(const string &tempResultOfUserCommand) {
    kill(anotherProcessPid,SIGUSR1);
    if(!fileExists(generalPath.c_str())){
        mkfifo(generalPath.c_str(),S_IFIFO);
        chmod(generalPath.c_str(),0666);
    }
    WriteToPipe(generalPath,tempResultOfUserCommand);
}

bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
                                      s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}
string ReadFromPipe(const string &path){
    ifstream fifo{path};
    string resultLine;
    string line;
    while (std::getline(fifo, line))
    {
        resultLine +=line;
    }
    fifo.close();
    return  resultLine;
}

void WriteOwnPid(const string &pathPid) {
    if(!fileExists(pathPid.c_str())){
        mkfifo(pathPid.c_str(),S_IFIFO);
        chmod(pathPid.c_str(),0666);
    }
    WriteToPipe(pathPid,to_string(myPid));
}

void WriteToPipe(const string &path,const string &value) {
    int filePidDescriptor = open(path.c_str(), O_WRONLY );
    if(filePidDescriptor < 0){
        cout << "Error can't open file " << path << "\n";
    }
    int response = write(filePidDescriptor, value.c_str(),value.size());
    close(filePidDescriptor);
}

void PrepareToRead(int n){
    cout<< "Start reading.."<<endl;
    cout<<"Протченно : \n"<<ReadFromPipe(generalPath)<<endl;
    cout.flush();
    cout.clear();
    cin.clear();

}
void InitSignals() {
    struct sigaction act, outI;
    act.sa_handler = PrepareToRead;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NODEFER;
    sigaction(SIGUSR1, &act, &outI);
}
bool fileExists(const char* file) {
    struct stat buf;
    return (stat(file, &buf) == 0);
}