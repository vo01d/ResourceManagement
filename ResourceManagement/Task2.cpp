//#include <iostream>
//#include <memory>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//
//void closeSocketFunction(SOCKET* ptr) {
//    if (*ptr != INVALID_SOCKET) {
//        closesocket(*ptr);
//    }
//    delete ptr;
//}
//
//struct SocketDeleterFunctor {
//    void operator()(SOCKET* ptr) const {
//        if (*ptr != INVALID_SOCKET) {
//            closesocket(*ptr);
//        }
//        delete ptr;
//    }
//};
//
//int main() {
//    WSADATA wsaData;
//    int wsaerr;
//    WORD wVersionRequested = MAKEWORD(2, 2);
//    wsaerr = WSAStartup(wVersionRequested, &wsaData);
//    if (wsaerr != 0) {
//        std::cout << "The Winsock dll not found" << std::endl;
//        return 0;
//    }
//    else {
//        std::cout << "The Winsock dll found!" << std::endl;
//        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
//    }
//
//    auto socketDeleterLambda = [](SOCKET* ptr) {
//        if (*ptr != INVALID_SOCKET) {
//            closesocket(*ptr);
//        }
//        delete ptr;
//    };
//
//    std::unique_ptr<SOCKET, decltype(socketDeleterLambda)> socket1(new SOCKET(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)), socketDeleterLambda);
//    std::unique_ptr<SOCKET, void(*)(SOCKET* ptr)> socket2(new SOCKET(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)), closeSocketFunction);
//    std::unique_ptr<SOCKET, SocketDeleterFunctor> socket3(new SOCKET(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)), SocketDeleterFunctor());
//    if (*socket1 == INVALID_SOCKET || *socket2 == INVALID_SOCKET || *socket3 == INVALID_SOCKET) {
//        std::cout << "Error with sockets" << WSAGetLastError() << std::endl;
//        return 0;
//    }
//    else {
//        std::cout << "sockets is OK!" << std::endl;
//    }
//
//    // other actions with sockets...
//
//    return 0;
//}