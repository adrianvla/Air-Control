#include "framework.h"
#include "Air Control.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <stdlib.h>
#include <functional>
#include <cstdio>
#include <future>
#include <time.h>
#include <wchar.h>
#include <string.h> 
#include <sys/types.h> 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
char* ERROR_MSG;
#include "Network.h"
#include<thread>


#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) //sec error

using namespace std;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
VOID __stdcall mouseSend(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
int Cleanup();
void GetDesktopResolution(int* horizontal, int* vertical);
void connectToNew();
void RecieveUDP();

inline std::tm* localtime_xp(std::time_t* timer)
{
    std::tm bt{};
#if defined(__unix__)
    localtime_r(timer, &bt);
#elif defined(_MSC_VER)
    localtime_s(&bt, timer);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    bt = *std::localtime(timer);
#endif
    return &bt;
}




class wnd {
public:
    MSG  msg;
    HINSTANCE* hi;
    const wchar_t* nm;
    HWND hwnd;
    WNDCLASSW wc = { 0 };
    wnd (const wchar_t* name,HINSTANCE* hInstance) {
        ZeroMemory(&wc, sizeof(WNDCLASS));
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpszClassName = (LPCWSTR) name;
        wc.hInstance = *hInstance;
        hi = hInstance;
        nm = name;
        wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
        wc.lpfnWndProc = (WNDPROC)WndProc;
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        RegisterClassW(&wc);
    }
    HWND Display(int x, int y, int w, int h, int nCmdShow) {
        hwnd = CreateWindowW(wc.lpszClassName, (LPCWSTR)(nm),
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            x, y, w, h, NULL, NULL, *hi, NULL);
        ShowWindow(hwnd, nCmdShow);
        UpdateWindow(hwnd);
        while (GetMessage(&msg, NULL, 0, 0)) {

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return hwnd;
    }
};




#define DEFAULT_BUFLEN 512
#define MAXLINE 1024 

void debug(char kk[100]) {}

#define M_UDP_PORT 23232

#define IDT_MOUSESEND 100


#define RECIEVE_BUFFER_MAX_LENGTH 19
#define IDLE_AFTER_MS 500



POINT* oldp;
POINT* startingPos;
POINT* virtualMousePos;
POINT* spoofedPosition;
POINT* toPut;
POINT* oldSpoofedPosition;
BOOL wasOldP_USED = false;
//#define DISTANCE_TO_BORDER 2
#define DISTANCE_TO_BORDER 2
#define MAX_COMPUTER_NUM 2
#define IP_LENGTH 16
char NextPacket[DEFAULT_BUFLEN];
const char* FalseOrTrue;
int* w;
int* h;
bool isOnOtherComputer = false;
int* WhichAmI;
int* WhichIsMouseOn;
int* WhichControls;
char computerList [MAX_COMPUTER_NUM][IP_LENGTH] = {
    "192.168.1.22","192.168.1.23"
};
int widthList[MAX_COMPUTER_NUM] = {
    0,0
};
int heightList[MAX_COMPUTER_NUM] = {
    0,0
};
bool whoSetRes[MAX_COMPUTER_NUM] = {
    false,false
};
WSASession* Session;
UDPSocket* Socket;
UDPSocket* SocketToSend;
char A[RECIEVE_BUFFER_MAX_LENGTH];

int time_to_idle_check = 0;

//bool WasIdling = false;

wchar_t* wString;


QuadraticFunction tempFunction;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow) {
    const wchar_t* windowName1 = L"Pixels";
    //const wchar_t* windowName2 = L"Pixels2";
    ZeroMemory(A, sizeof(char[RECIEVE_BUFFER_MAX_LENGTH]));
    tempFunction.a = tempFunction.b = tempFunction.c = 0;
    //INIT
    /*
    char q[DEFAULT_BUFLEN];
    NextPacket = &q[0];*/
    char F[128] = "";
    ERROR_MSG = F;
    char G[6];
    FalseOrTrue = &G[0];
    wchar_t J[1024];
    wString = J;
    //UDP dgram;
    //datagram = &dgram;

    POINT p,s,o,S,O,I;
    toPut = &I;
    oldp = &p;
    spoofedPosition = &S;
    startingPos = &s;
    oldSpoofedPosition = &O;
    virtualMousePos = &o;
    ZeroMemory(oldp, sizeof(POINT));
    ZeroMemory(startingPos, sizeof(POINT));
    ZeroMemory(virtualMousePos, sizeof(POINT));
    ZeroMemory(spoofedPosition, sizeof(POINT));
    ZeroMemory(oldSpoofedPosition, sizeof(POINT));
    ZeroMemory(toPut,sizeof(POINT));
    int a, b,c=-1,d=-1,e;
    w = &a;
    h = &b;
    e = 0;  // MY INDEX
    WhichAmI = &e;
    WhichIsMouseOn = &c;
    WhichControls = &d;
    GetDesktopResolution(w,h);
    //debug(w,h);
    widthList[*WhichAmI] = *w;
    heightList[*WhichAmI] = *h;
    whoSetRes[*WhichAmI] = true;



    //PCWSTR addr = L"192.168.1.1";

    //datagram->init(addr, 23232);
    //datagram->Send("Hello World!");
    //int Q = 0;
    //Q = datagram->Recieve();
    //printf("%d : %s",Q,datagram->buf);

    WSASession g;
    UDPSocket hh,kk;
    Session = &g;
    Socket = &hh;
    SocketToSend = &kk;

    Socket->Bind(23232);
    //char K[DEFAULT_BUFLEN];
    //snprintf(K,DEFAULT_BUFLEN,"hiello world %d",1);
    //SocketToSend->SendTo("192.168.1.23",23232,K,sizeof(K));
    //RecieveUDP();
    std::thread thread_obj(RecieveUDP);
    u_short l = 0;
    for (; l < MAX_COMPUTER_NUM; l++) {
        if (l == *WhichAmI)continue;
        snprintf(A, RECIEVE_BUFFER_MAX_LENGTH, "R%d;%d;%d", *w, *h, *WhichAmI);
        SocketToSend->SendTo(computerList[l], M_UDP_PORT, A, sizeof(A));
    }

    wnd j(&windowName1[0], &hInstance);
    j.Display(rand() % 1000, rand() % 1000, 500 + (rand() % 500), 500 + (rand() % 500), nCmdShow);



    srand(time(NULL));
    return (int)j.msg.wParam;
}

//bool returnedFromOther = false;


#define CORE_MOUSE_MS 100
#define MOUSE_IDLE_CHECK_INTERV 10


bool WasIdle = false;

bool IsWaitingAReply = false;

VOID __stdcall mouseSend(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
    
    POINT p;
    if (GetCursorPos(&p)) {
        /*if ((((spoofedPosition->x != p.x) || (spoofedPosition->y != p.y)) && ((*WhichIsMouseOn == *WhichAmI) && ((*WhichControls != *WhichAmI) && (*WhichControls != -1)))) && ((oldp2->x != p.x) || (oldp2->y != p.y))) {
            snprintf(A, DEFAULT_BUFLEN, "J%d;%d", p.x, p.y);
            SocketToSend->SendTo(computerList[*WhichControls], M_UDP_PORT, A, sizeof(A));
            oldp2->x = p.x;
            oldp2->y = p.y;
        }*/
        if (*WhichControls == *WhichAmI) {
            if (!wasOldP_USED) {
                *oldp = p;
                //*startingPos = p;
                startingPos->x = *w / 2;
                startingPos->y = *h / 2;

                //*virtualMousePos = p;
                //virtualMousePos->x = *w / 2;
                //virtualMousePos->y = *h / 2;
                if (*WhichIsMouseOn == *WhichAmI + 1)
                    virtualMousePos->x = DISTANCE_TO_BORDER + 1;
                else if (*WhichIsMouseOn == *WhichAmI - 1)
                    virtualMousePos->x = widthList[*WhichIsMouseOn] - 1 - DISTANCE_TO_BORDER;
                virtualMousePos->y = p.y;
                SetCursorPos(*w / 2, *h / 2);
                wasOldP_USED = true;
            }
            if ((oldp->x == p.x) && (oldp->y == p.y)) return; //check for mouse idling
            //SetCursor(NULL);
            virtualMousePos->x += p.x - startingPos->x;
            virtualMousePos->y += p.y - startingPos->y;

            if (virtualMousePos->x < 0) virtualMousePos->x = 0; // do not allow overscan
            else if (virtualMousePos->x > widthList[*WhichIsMouseOn]) virtualMousePos->x = widthList[*WhichIsMouseOn];
            if (virtualMousePos->y < 0) virtualMousePos->y = 0;
            else if (virtualMousePos->y > heightList[*WhichIsMouseOn]) virtualMousePos->y = heightList[*WhichIsMouseOn];

            if (*WhichIsMouseOn == *WhichAmI && *WhichControls == *WhichAmI) { //I'm controling my mouse
                *WhichControls = -1;
                SetCursorPos(DISTANCE_TO_BORDER+1, virtualMousePos->y);
                wasOldP_USED = false;
                return;
            }
            if (virtualMousePos->x > (widthList[*WhichIsMouseOn] - DISTANCE_TO_BORDER)) { //right
                //isOnOtherComputer = true;
                if (*WhichIsMouseOn != MAX_COMPUTER_NUM-1) {
                    *WhichIsMouseOn += 1;
                    *WhichControls = *WhichAmI;
                    virtualMousePos->x = DISTANCE_TO_BORDER + 1;
                }
            }
            if (virtualMousePos->x < DISTANCE_TO_BORDER && *WhichIsMouseOn >=0) { //left
                if (*WhichIsMouseOn - 1 == *WhichAmI) { //if mouse is on my left computer
                    *WhichIsMouseOn = *WhichIsMouseOn - 1;
                    *WhichControls = -1;
                    wasOldP_USED = false;
                    SetCursorPos(-DISTANCE_TO_BORDER + widthList[*WhichIsMouseOn] - 1, virtualMousePos->y);
                    virtualMousePos->x = -DISTANCE_TO_BORDER + widthList[*WhichIsMouseOn] - 1;
                    return;
                }
                //isOnOtherComputer = false;
                if (*WhichIsMouseOn > 0) { //check if computer is somewhere in middle
                    *WhichIsMouseOn = *WhichIsMouseOn - 1;
                    virtualMousePos->x = -DISTANCE_TO_BORDER + widthList[*WhichIsMouseOn] - 1;
                    return;
                }
                else if (*WhichIsMouseOn == 0) { //check for left boundary
                    //return; 
                }
            }
            SetCursorPos(startingPos->x, startingPos->y);
            //SetWindowPos(hWnd, nullptr, virtualMousePos->x, virtualMousePos->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

            if ((virtualMousePos->x == oldp->x) && (virtualMousePos->y == oldp->y)) return; //check for mouse idling

            if (time_to_idle_check > IDLE_AFTER_MS) {
                //WasIdle = true; not used
                time_to_idle_check = 0;
                snprintf(NextPacket, DEFAULT_BUFLEN, "A");
                SocketToSend->SendTo(computerList[*WhichIsMouseOn], M_UDP_PORT, NextPacket, sizeof(NextPacket));
                IsWaitingAReply = true;
            }

            time_to_idle_check = 0;

            if (IsWaitingAReply) return;
            snprintf(NextPacket, DEFAULT_BUFLEN, "M%d;%d", virtualMousePos->x, virtualMousePos->y);
            //datagram->Send(NextPacket);
            SocketToSend->SendTo(computerList[*WhichIsMouseOn], M_UDP_PORT, NextPacket, sizeof(NextPacket));

            *oldp = *virtualMousePos;
        }
        else {
            if ((p.x>( * w - DISTANCE_TO_BORDER)) && ( * WhichAmI != MAX_COMPUTER_NUM - 1)) { //right
                //isOnOtherComputer = true;
                *WhichIsMouseOn = *WhichAmI + 1;
                *WhichControls = *WhichAmI;
                connectToNew();
            }
            if ((p.x < DISTANCE_TO_BORDER) && (*WhichAmI != 0)) { //left
                *WhichIsMouseOn = *WhichAmI - 1;
                *WhichControls = *WhichAmI;
                connectToNew();
            }
        }
        /*
        if (returnedFromOther) {
            SetCursorPos(-DISTANCE_TO_BORDER + *w - 1, virtualMousePos->y);
            returnedFromOther = false;
        }*/
    }
}

int Cleanup() {
    int ERNUM = 0;
    //ERNUM += datagram->Shutdown();
    return ERNUM;
}
char RECVbuffer[100];
void connectToNew() {
    //computerList[*WhichIsMouseOn];
}

bool isInit = false;
char tempAddress[16] = "256.256.256.256";
POINT CursorPos;
void RecieveUDP() {
    //"R32767;32767;32767"
    while (1) {
        sockaddr_in add = Socket->RecvFrom(RECVbuffer, sizeof(RECVbuffer));

        //std::string input(RECVbuffer);
        //std::reverse(std::begin(input), std::end(input));
        //debug(RECVbuffer);
        if (RECVbuffer[0] == 'A') {
            if (GetCursorPos(&CursorPos)) {
                snprintf(tempAddress, 16, "%d.%d.%d.%d", add.sin_addr.S_un.S_un_b.s_b1, add.sin_addr.S_un.S_un_b.s_b2, add.sin_addr.S_un.S_un_b.s_b3, add.sin_addr.S_un.S_un_b.s_b4);
                snprintf(A, DEFAULT_BUFLEN, "J%d;%d", CursorPos.x, CursorPos.y);
                SocketToSend->SendTo(tempAddress, M_UDP_PORT, A, sizeof(A));
                //SocketToSend->SendTo(computerList[atoi(packet3)], M_UDP_PORT, A, sizeof(A));
            }
        }
        else if (RECVbuffer[0] == 'J') {
            u_short index = 1;
            u_short index2 = 0;
            bool isY = false;
            char packet[5] = "";  //32767
            char packet2[5] = ""; //32767
            while (1) {
                if (RECVbuffer[index] == ';') {
                    isY = true;
                    index++;
                    index2 = 0;
                }
                if (RECVbuffer[index] == '\0') { //NULL TERMINATED
                    break;
                }
                if (isY) {
                    packet2[index2] = RECVbuffer[index];

                }
                else {
                    packet[index2] = RECVbuffer[index];

                }
                index++;
                index2++;
            }
            //if ((virtualMousePos->x != spoofedPosition->x) || (virtualMousePos->y != spoofedPosition->y))

            virtualMousePos->x = atoi(packet);
            virtualMousePos->y = atoi(packet2);
            IsWaitingAReply = false;
        }
        else if (RECVbuffer[0] == 'S') {
            u_short index = 1;
            u_short index2 = 0;
            bool isY = false;
            char packet[5] = "";  //32767
            char packet2[5] = ""; //32767
            while (1) {
                if (RECVbuffer[index] == ';') {
                    isY = true;
                    index++;
                    index2 = 0;
                }
                if (RECVbuffer[index] == '\0') { //NULL TERMINATED
                    break;
                }
                if (isY) {
                    packet2[index2] = RECVbuffer[index];

                }
                else {
                    packet[index2] = RECVbuffer[index];

                }
                index++;
                index2++;
            }
            *WhichIsMouseOn = atoi(packet);
            *WhichControls = atoi(packet2);
        }
        else if (RECVbuffer[0] == 'M') { // mouse packet
            u_short index = 1;
            u_short index2 = 0;
            bool isY = false;
            char packet[5] = "";  //32767
            char packet2[5] = ""; //32767
            while (1) {
                if (RECVbuffer[index] == ';') {
                    isY = true;
                    index++;
                    index2 = 0;
                }
                if (RECVbuffer[index] == '\0') { //NULL TERMINATED
                    break;
                }
                if (isY) {
                    packet2[index2] = RECVbuffer[index];

                }
                else {
                    packet[index2] = RECVbuffer[index];

                }
                index++;
                index2++;
            }
            spoofedPosition->x = atoi(packet);
            spoofedPosition->y = atoi(packet2);
            if((oldSpoofedPosition->x != spoofedPosition->x)||(oldSpoofedPosition->y != spoofedPosition->y))
                SetCursorPos(atoi(packet), atoi(packet2));

            oldSpoofedPosition->x = spoofedPosition->x;
            oldSpoofedPosition->y = spoofedPosition->y;
        }
        else if (RECVbuffer[0] == 'R') {
            u_short index = 1;
            u_short index2 = 0;
            u_short isY = 0;
            char packet[5] = "";  //32767
            char packet2[5] = ""; //32767
            char packet3[5] = ""; //32767
            while (1) {
                if (RECVbuffer[index] == ';') {
                    isY++;
                    index++;
                    index2 = 0;
                }
                if (RECVbuffer[index] == '\0') { //NULL TERMINATED
                    break;
                }
                if (isY==1) {
                    packet2[index2] = RECVbuffer[index];

                }
                else if (isY==0) {
                    packet[index2] = RECVbuffer[index];

                }
                else if (isY == 2) {
                    packet3[index2] = RECVbuffer[index];

                }
                index++;
                index2++;
            }
            // Rwidth;height;computer_index
            widthList[atoi(packet3)] = atoi(packet);
            heightList[atoi(packet3)] = atoi(packet2);
            if (whoSetRes[atoi(packet3)]) continue;
            snprintf(A, RECIEVE_BUFFER_MAX_LENGTH, "R%d;%d;%d", *w, *h, *WhichAmI);
            SocketToSend->SendTo(computerList[atoi(packet3)], M_UDP_PORT, A, sizeof(A));
            whoSetRes[atoi(packet3)] = true;
        }
    }
    //Socket->SendTo(add, input.c_str(), input.size());
    //return;
}
wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 1024);
    return wString;
}
char B[RECIEVE_BUFFER_MAX_LENGTH];
void OnPaint(HWND hWnd) {
    RECT  rect;
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    GetClientRect(hWnd, &rect);
    SetTextColor(hdc, RGB(0x00, 0x00, 0x00));
    SetBkMode(hdc, 0);
    //SetBkColor(hdc, RGB(0xD0, 0xD0, 0xFF));
    rect.left = 40;
    rect.top = 10;
    
    u_short l = 0;

    for (; l < MAX_COMPUTER_NUM; l++) {
        if (whoSetRes[l]) {
            FalseOrTrue = "TRUE";
        }
        else {
            FalseOrTrue = "FALSE";
        }
        snprintf(B, RECIEVE_BUFFER_MAX_LENGTH+32, "%d %s: %d;%d %s",l, computerList[l], widthList[l], heightList[l], FalseOrTrue);
        DrawText(hdc, convertCharArrayToLPCWSTR(B), -1, &rect, DT_SINGLELINE | DT_NOCLIP);
        rect.top += 20;
    }
    rect.top += 30;
    snprintf(B, RECIEVE_BUFFER_MAX_LENGTH + 32, "COMPUTER INDEX: %d", *WhichAmI);
    DrawText(hdc, convertCharArrayToLPCWSTR(B), -1, &rect, DT_SINGLELINE | DT_NOCLIP);
    rect.top += 20;
    snprintf(B, RECIEVE_BUFFER_MAX_LENGTH + 32, "COMPUTER CONTROLS MOUSE: %d", *WhichControls);
    DrawText(hdc, convertCharArrayToLPCWSTR(B), -1, &rect, DT_SINGLELINE | DT_NOCLIP);
    rect.top += 20;
    snprintf(B, RECIEVE_BUFFER_MAX_LENGTH + 32, "WHICH COMPUTER IS MOUSE ON: %d", *WhichIsMouseOn);
    DrawText(hdc, convertCharArrayToLPCWSTR(B), -1, &rect, DT_SINGLELINE | DT_NOCLIP);
    rect.top = 10;
    rect.left = 300;
    SetTextColor(hdc, RGB(0xFF, 0x00, 0x00));
    DrawText(hdc, convertCharArrayToLPCWSTR(ERROR_MSG), -1, &rect, DT_SINGLELINE | DT_NOCLIP);

    //SelectObject(hdc, oldPen);
    //DeleteObject(hPen);
    EndPaint(hWnd, &ps);
}
VOID __stdcall RWINDOW(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
    RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    if (*WhichControls == *WhichAmI) {
        snprintf(A, DEFAULT_BUFLEN, "S%d;%d", *WhichIsMouseOn, *WhichControls);
        for (int l = 0; l < MAX_COMPUTER_NUM; l++) {
            SocketToSend->SendTo(computerList[l], M_UDP_PORT, A, sizeof(A));
        }
    }
}
VOID __stdcall mouse_idle_check(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
    if ((virtualMousePos->x != oldp->x) || (virtualMousePos->y != oldp->y)) return; //if mouse is idle continue
    time_to_idle_check += MOUSE_IDLE_CHECK_INTERV;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (!isInit) {
        SetTimer(hWnd, IDT_MOUSESEND, CORE_MOUSE_MS, (TIMERPROC)mouseSend);
        SetTimer(hWnd, 102, MOUSE_IDLE_CHECK_INTERV, (TIMERPROC)mouse_idle_check);
        SetTimer(hWnd, 101, 1000, (TIMERPROC)RWINDOW);
        isInit = true;
    }
    switch (message)
    {
    case WM_LBUTTONDOWN:
        //SocketToSend->SendTo("127.0.0.1", M_UDP_PORT, d.c_str(),d.size());
        //SetTimer(hWnd, IDT_MOUSESEND, 100, (TIMERPROC)mouseSend);
        break;
    case WM_RBUTTONDOWN:
        //KillTimer(hWnd, IDT_MOUSESEND);
        break;
    case WM_DESTROY:
        Cleanup();
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        OnPaint(hWnd);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


void GetDesktopResolution(int* horizontal, int* vertical)
{
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    *horizontal = desktop.right;
    *vertical = desktop.bottom;
}





void findFunction(QuadraticFunction* pTo, QuadraticPoints3* p) {
    #define d ((p->Yb - p->Ya) / (p->Xb - p->Xa))
    pTo->a = ((p->Xc * d) + p->Ya - (p->Xa * d) - p->Yc) / ((p->Xc * (p->Xb + p->Xa - p->Xc)) + (p->Xa * p->Xb));
    pTo->b = d - (pTo->a * (p->Xb + p->Xa));
    pTo->c = p->Ya - (p->Xa * p->Xa * pTo->a) - (p->Xa * pTo->b);
}