#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <thread>
#include <pthread.h>
#include <mutex>
#include <map>
#include <sstream>
#include <vector>
#include <regex>
#include <math.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <ifaddrs.h>

using namespace std;

string useAs, myIP = "0.0.0.0";
int listening, clientSocket = 0, bufSize = 4096;
int posXYZ[3] = {0, 0, 0};
map<string, thread> gotoDict;
thread th_Received, th_setCommand, th_keyPress;
sockaddr_in client;
mutex m;

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

string getMyIP(){
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);
    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) {
            continue;
        }
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if ((ifa->ifa_name) != "lo")
                myIP = addressBuffer;
            // printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        } else if (ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            // printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        } 
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return myIP;
}

void sendCallBack(string message)
{
    if ((!message.empty()) && (message != "quit"))
    {
        char buf[bufSize];

        //	Send to server
        int sendRes = send(clientSocket, trim(message).c_str(), message.size() + 1, 0);
        if (sendRes == -1)
        {
            cout << "Could not send to server! Whoops!\r\n";
            // continue;
            return;
        }
        else
        {
            cout << "@ " << "[BaseStation]" << " : " << trim(message) << endl;
        }

        // //		Wait for response
        // memset(buf, 0, bufSize);
        // int bytesReceived = recv(clientSocket, buf, bufSize, 0);
        // if (bytesReceived == -1) {
        //     cout << "There was an error getting response from server\r\n"; }
        // else {
        //     //		Display response
        //     cout << "SERVER> " << string(buf, bytesReceived) << "\r\n"; }
    }
}

void sendPosXYZ()
{
    string message = to_string(posXYZ[0]) + "," + to_string(posXYZ[1]) + "," + to_string(posXYZ[2]);
    sendCallBack(message);
}

void GotoLoc (string Robot, int endX, int endY, int endAngle, int shiftX, int shiftY, int shiftAngle) 
{
    try {
        cout << "# " << Robot << " : Goto >> " << "X:" << endX << " Y:" << endY << " ∠:" << endAngle << "°" << endl;
        bool chk[] = { true, true, true };
        while (chk[0] |= chk[1] |= chk[2]) {
            // cout << "wkwkw A " << chk[0] << chk[1] << chk[2] << endl;
            if (posXYZ[0] > 12000)
                posXYZ[0] = stoi (to_string(posXYZ[0]).substr (0, 4));
            if (posXYZ[1] > 9000)
                posXYZ[1] = stoi (to_string(posXYZ[1]).substr (0, 4));
            if (posXYZ[2] > 360)
                posXYZ[2] = stoi (to_string(posXYZ[2]).substr (0, 2));

            if ((posXYZ[0] > endX) && (shiftX > 0))
                shiftX *= -1;
            else if ((posXYZ[0] < endX) && (shiftX < 0))
                shiftX *= -1;
            if ((posXYZ[1] > endY) && (shiftY > 0))
                shiftY *= -1;
            else if ((posXYZ[1] < endY) && (shiftY < 0))
                shiftY *= -1;
            if ((posXYZ[2] > endAngle) && (shiftAngle > 0))
                shiftAngle *= -1;
            else if ((posXYZ[2] < endAngle) && (shiftAngle < 0))
                shiftAngle *= -1;

            if (posXYZ[0] != endX) {
                if (abs (endX - posXYZ[0]) < abs (shiftX)) // Shift not corresponding
                    shiftX = (endX - posXYZ[0]);
                posXYZ[0] += shiftX; // On process
            } else
                chk[0] = false; // Done
            if (posXYZ[1] != endY) {
                if (abs (endY - posXYZ[1]) < abs (shiftY)) // Shift not corresponding
                    shiftY = (endY - posXYZ[1]);
                posXYZ[1] += shiftY; // On process
            } else
                chk[1] = false; // Done
            if (posXYZ[2] != endAngle) {
                if (abs (endAngle - posXYZ[2]) < abs (shiftAngle)) // Shift not corresponding
                    shiftAngle = (endAngle - posXYZ[2]);
                posXYZ[2] += shiftAngle; // On process
            } else
                chk[2] = false; // Done

            sendPosXYZ();
            usleep (100000); // time per limit (microsecond)
        }
    } catch (exception e) 
    { 
        cout << "# Error GotoLoc \n\n" << e.what() << endl;
    }
}

void threadGoto (string keyName, string message) 
{
    string item;
    vector<int> dtXYZ;
    if (gotoDict.count(keyName)) {
        gotoDict[keyName].detach();
        gotoDict.erase (keyName);
    }
    for (stringstream ss(message.substr(2)); (getline(ss, item, ',')); (dtXYZ.push_back(stoi(item))));
    gotoDict[keyName] = thread( GotoLoc, useAs, dtXYZ[0], dtXYZ[1], dtXYZ[2], 20, 20, 1);
}

string ResponeCallback(string message)
{
    string respone = "", item;
    vector<string> _dtMessage, msgXYZ;
    for (stringstream ss(message); (getline(ss, item, '|')); (_dtMessage.push_back(item)));
    if ((_dtMessage[0].find("go") != 0) && (regex_match(_dtMessage[0].begin(), _dtMessage[0].end(), regex("[-]{0,1}[0-9]{1,4},[-]{0,1}[0-9]{1,4},[-]{0,1}[0-9]{1,4}"))))
    {
        // If message is data X & Y from encoder
        /// Scale is 1 : 20StartStart
        for (stringstream ss(_dtMessage[0]); (getline(ss, item, ',')); (msgXYZ.push_back(item)));
        _dtMessage[0].clear();
        ;
        if (msgXYZ.size() > 3) // If data receive multi value X & Y (error bug problem)
        {
            msgXYZ[0] = msgXYZ[msgXYZ.size() - 3].substr(msgXYZ[msgXYZ.size() - 1].size());
            msgXYZ[1] = msgXYZ[msgXYZ.size() - 2];
            msgXYZ[2] = msgXYZ[msgXYZ.size() - 1];
        }
        if ((!isBlank(msgXYZ[0])) && (stoll(msgXYZ[0]) > 12000))
            msgXYZ[0] = msgXYZ[0].substr(0, 4);
        if ((!isBlank(msgXYZ[1])) && (stoll(msgXYZ[1]) > 9000))
            msgXYZ[1] = msgXYZ[1].substr(0, 4);
        if ((!isBlank(msgXYZ[2])) && (stoll(msgXYZ[2]) > 360))
            msgXYZ[2] = msgXYZ[2].substr(0, 2);

        for (int i = 0; i < msgXYZ.size(); i++)
            posXYZ[i] = stoll(msgXYZ[i]);

        sendPosXYZ();
        // _dtMessage[0] = "X:" + to_string(posXYZ[0]) + " Y:" + to_string(posXYZ[1]) + " ∠:" + to_string(posXYZ[2]) + "°";
    }
    else if (regex_match(_dtMessage[0].begin(), _dtMessage[0].end(), regex("go[-]{0,1}[0-9]{1,4},[-]{0,1}[0-9]{1,4},[-]{0,1}[0-9]{1,4}")))
    {
        threadGoto(useAs, _dtMessage[0]);
    }
    // else if ((_socketDict.ContainsKey("BaseStation")) && (socket.Client.RemoteEndPoint.ToString().Contains(_socketDict["BaseStation"].Client.RemoteEndPoint.ToString())))
    else if (clientSocket != 0)
    // else if (true)
    {
        // If socket is Base Station socket
        /// INFORMATION ///
        if (_dtMessage[0] == "B")
        { //Get the ball
            respone = "Ball on " + useAs /*+ socketToName(socket)*/;
            goto broadcast;
        }

        /// OTHERS ///
        else if (_dtMessage[0] == "ip")
        {
            respone = getMyIP();
            goto multicast;
        }
        else if (_dtMessage[0] == "get_time")
        { //TIME NOW
            time_t ct = time(0);
            respone = ctime(&ct);
            goto multicast;
        }
        else;
            // printf("# Invalid Command :< \n");
    }
    goto end;

broadcast:
    sendCallBack(respone + "|" + "Robot1,Robot2,Robot3");
    // sendByHostList("BaseStation", respone + "|" + "Robot1,Robot2,Robot3");
    goto end;

multicast:
    if (_dtMessage.size() > 1)
        sendCallBack(respone + "|" + _dtMessage[1]);
    else
        sendCallBack(respone);
    // sendByHostList("BaseStation", respone + "|" + chkRobotCollect);
    goto end;

end:
    // if (!_dtMessage[0].empty())
    //     cout << "> " << "[" << useAs << "]" << " : " << _dtMessage[0];
    return respone;
}

void receivedCallBack()
{
    string message = "";
    for (m.lock(); (true) && (message != "quit"); message.clear(), m.unlock())
    {
        // While loop: accept and echo message back to clientuserInput
        char buf[bufSize];
        memset(buf, 0, bufSize);

        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, bufSize, 0);
        if (bytesReceived == -1) {
            cerr << "Error in receivedCallBack(). Quitting" << endl;
            break; }

        if (bytesReceived == 0) {
            cout << "Client disconnected " << endl;
            break; }
        message = trim(string(buf, 0, bytesReceived));
        cout << "> " << "[BaseStation]" << " : " << message << endl;
        ResponeCallback(message);
        
        // Echo message back to client
        // send(clientSocket, buf, bytesReceived + 1, 0);
    }
    // Close the socket
    close(clientSocket);
}

void listenClient(int listening)
{
    // Wait for a connection
    socklen_t clientSize = sizeof(client);
    clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

    char host[NI_MAXHOST];      // Client's remote name
    char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on

    memset(host, 0, NI_MAXHOST); // same as memset(host, 0, NI_MAXHOST);
    memset(service, 0, NI_MAXSERV);

    printf("IP address is: %s\n", inet_ntoa(client.sin_addr));
    // printf("port is: %d\n", (int) ntohs(client.sin_port));

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
        cout << host << " connected on port " << service << endl; }
    else {
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        cout << host << " connected on port " << ntohs(client.sin_port) << endl; }

    // Close listening socket
    close(listening);

    // Start received message
    th_Received = thread(receivedCallBack);
}

int setupServer(int port)
{
    cout << "Server Starting..." << endl;
    // Create a socket
    listening = socket(AF_INET, SOCK_STREAM, 0);
    // int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) {
        cerr << "Can't create a socket! Quitting" << endl;
        return -1; }

    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    bind(listening, (sockaddr*)&hint, sizeof(hint));

    // Tell Winsock the socket is for listening
    listen(listening, SOMAXCONN);
    cout  << "Listening to TCP clients at " << getMyIP() << " : " << port << endl;
    listenClient(listening);
}




int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;
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

void keyEvent(string key)
{
    int _temp[3];
    copy(begin(posXYZ), end(posXYZ), begin(_temp));
    if (key == "[C")
        posXYZ[0] += 1;
    else if (key == "[D")
        posXYZ[0] -= 1;
    else if (key == "[A")
        posXYZ[1] -= 1;
    else if (key == "[B")
        posXYZ[1] += 1;
    else if (key == "[5")
        posXYZ[2] += 1;
    else if (key == "[6")
        posXYZ[2] -= 1;

    for (int i = 0; i < 3; i++)
        if ((posXYZ[i] != _temp[i]) && (clientSocket != 0) /*&& (_socketDict.ContainsKey("BaseStation"))*/){
            sendPosXYZ();
            break; }
    // cout << "# X:" << posXYZ[0] << " Y:" << posXYZ[1] << " ∠:" << posXYZ[2] << "°" << endl;
}

void keyPress()
{
    string s = "";
    int i = 0;
    printf("# Set Location Mode \n");
    for (m.lock(); 1; m.unlock()){
        if (kbhit())
            s += getchar();
        if ((s.rfind("^") != (s.size() - 1)) && (s.rfind("[") != (s.size() - 1))) {
            if (s == "") {
                if (i == 1)
                    break;
                i++; }
            else {
                keyEvent(s);
                i = 0; }
            s = ""; } }
    printf("# Set Command Mode \n");
}

void setCommand()
{
    try {        
        string Command;
        for (m.lock(); (true) && (Command != "quit"); m.unlock()){
            getline(cin, Command);

            if (Command == "quit") {
                close(listening);
                close(clientSocket);
            }
            else if (Command == "key") {
                thread th_keyPress(keyPress);
                th_keyPress.join();
            }
            else if ((clientSocket != 0) && (Command.find("go") == 0))
                ResponeCallback(Command);
            else if (Command == "myip")            
                cout << "# MyIP: " << getMyIP() << endl;
            else if (Command == "as")            
                cout << "# UseAs: " << useAs << endl;
            else if (clientSocket != 0)
                sendCallBack(Command);
            else
                printf("!...not Connected...! \n"); }  

        close(listening);
        close(clientSocket);
        cout << "# Close App" << endl;
    }
    catch (exception e) {
        cout << "% setCommand error \n~\n" << e.what() << endl; }
}

int main()
{
    printf("~ Welcome to Robot Core ~ \n");
    printf("Use As: "); cin >> useAs;
    getMyIP();
    setupServer(8686);
    thread th_setCommand(setCommand);
    th_setCommand.join();
    return 0;
}
