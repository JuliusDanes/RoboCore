#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <pthread.h>
#include <mutex>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <vector>
#include <regex>

using namespace std;

mutex m;
// thread th_Write, th_Read;

string dbData[2][2] = {{ "/media/detox/Drive/KRSBI/Verocia-Project/RobotCore/RobotCore/db1.txt", "0" }, { "/media/detox/Drive/KRSBI/Verocia-Project/RobotCore/RobotCore/db2.txt", "0" }};
string posXYZ = "";

inline bool isBlank(const std::string &s)
{
    return std::all_of(s.cbegin(), s.cend(), [](char c) { return std::isspace(c); });
}

string trim_left(const string &str)
{
    const string pattern = " \f\n\r\t\v";
    return str.substr(str.find_first_not_of(pattern));
}

string trim_right(const string &str)
{
    const string pattern = " \f\n\r\t\v";
    return str.substr(0, str.find_last_not_of(pattern) + 1);
}

string trim(const string &str)
{
    return trim_left(trim_right(str));
}

string toLowers(string s)
{
    for (int i =0; i< s.size(); i++)
        s[i] = tolower(s[i], locale());
    return s;
}

int kbhit(void)
{
    struct termios oldt, newt;
    int ch, oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1; }
    return 0;
}

void resetDB(int who)
{
    dbData[who][1] = "0";
    system(("printf '' > " + dbData[who][0]).c_str()); 
}

void reader1()
{
    string s = "";
    int i = 0;
    printf("# Read Mode db1 \n\n");
    while(true) {
        // system("clear");
        // printf("# Read Mode db1 \n\n");
        string line;
        int _temp = 1;
        for (ifstream infile(dbData[0][0]); getline(infile, line); _temp++) {  /// Display data
            if (_temp != stoi(dbData[0][1])) {
                dbData[0][1] = to_string(_temp);
                cout << line << endl; } }

        if (kbhit())
            s += getchar();
        if ((s.rfind("^") != (s.size() - 1)) && (s.rfind("[") != (s.size() - 1))) {
            if (s == "") {
                if (i == 1)
                    break;
                i++; }
            else {
                i = 0; }

            s = ""; }
        // usleep (100000); // time per limit (microsecond)
    }
    printf("# Write Mode \n");
}

void reader2()
{
    loop:
    string s = "";
    int i = 0, nRead = 0;
    system("clear");
    printf("# Read Mode db2 \n\n");
    while(true) {
        // system("clear");
        // printf("# Read Mode db2 \n\n");
        string line;
        int _temp = 1;
        if (nRead > stoi(dbData[1][1]))
            goto loop;
        for (ifstream infile(dbData[1][0]); getline(infile, line); _temp++) {  /// Display data
            if (_temp > nRead) {
                nRead = _temp;
                cout << line << endl; } }

        if (kbhit())
            s += getchar();
        if ((s.rfind("^") != (s.size() - 1)) && (s.rfind("[") != (s.size() - 1))) {
            if (s == "") {
                if (i == 1)
                    break;
                i++; }
            else {
                i = 0; }

            s = ""; }
        // usleep (100000); // time per limit (microsecond)
    }
    printf("# Write Mode \n");
}

void writer(string _new)
{
    string line;
    int _temp = 0;

    for (ifstream infile(dbData[1][0]); getline(infile, line); _temp++);   /// Get Old Data
    if (_temp > 1000)
        resetDB(1);
    else 
        dbData[1][1] = to_string(_temp+1);

    std::ofstream file;
    file.open(dbData[1][0], std::ios::out | std::ios::app);
    // if (file.fail())
    //     throw std::ios_base::failure(std::strerror(errno));
    ///make sure write fails with exception if something is wrong
    file.exceptions(file.exceptions() | std::ios::failbit | std::ifstream::badbit);
    file << _new << endl;
}


void setCommand()
{
    string Command;
    for (m.lock(); (true) && ((Command) != "quit"); m.unlock()) {
        getline(cin, Command);

        if (Command == ("OP")) {          //F1
            thread th_Read(reader1);
            th_Read.join(); }
        else if (Command == ("OQ")) {     //F2
            thread th_Read(reader2);
            th_Read.join(); }
        else if (Command == ("OR")) {     //F3
            resetDB(0); }
        else if (Command == ("OS")) {     //F4
            resetDB(1); }
        else
            writer(Command);
    }
}

void dtArduino()
{
    // for (m.lock(); true; m.unlock())
    while (true)
    {
        loop:
        // usleep(50000);
        int fd1;
        char buff[100];
        int rd,nbytes,tries;

        vector<string> dataList, dataList2;
        string item, line;


		fd1=open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
		fcntl(fd1, F_SETFL,0);
		rd=read(fd1,buff,128);

        string message = string(buff);
        if (!isBlank(message)) {
            for (stringstream ss(message); (getline(ss, item, 'E')); (dataList.push_back(item)));
            // item = line = "";
            if (regex_match(dataList[0].begin(), dataList[0].end(), regex("^[-]{0,1}[0-9]{1,4},[-]{0,1}[0-9]{1,4},[-]{0,1}[0-9]{1,4}$")))  {
                // for (stringstream ss(dataList[0]); (getline(ss, item, ',')); (dataList2.push_back(item))) 
                //     if (!(regex_match(item.begin(), item.end(), regex("^[-]{0,1}[0-9]{1,4}$"))))
                //         goto loop;

                // string posXYZ = "X"+dataList2[0]+", "+"Y"+ dataList2[1]+", "+ "Z"+dataList2[2];
                // cout << posXYZ << endl;
                // cout << dataList[0] << endl << endl;
                if (posXYZ != dataList[0]) {
                    posXYZ = (dataList[0]);
                    writer(dataList[0]); 
                    } } }
		close(fd1);
	}
}

int main()
{
    thread th_SetCommand(setCommand);
    thread th_Arduino(dtArduino);
    th_SetCommand.join();
    th_Arduino.join();

  return 0;
}
