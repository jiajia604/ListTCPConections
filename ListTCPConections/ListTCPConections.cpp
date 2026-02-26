// ListTCPConections.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <iphlpapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

void ListTCPConections(DWORD processId) {
    MIB_TCPTABLE_OWNER_PID *pTcpTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    // Get the size of TCP table
    dwRetVal = GetExtendedTcpTable(NULL, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
    if (dwRetVal != ERROR_INSUFFICIENT_BUFFER) {
        std::cerr << "Error getting TCP table size" << std::endl;
        return;
    }

    // Allocate memory for TCP table
    pTcpTable = (MIB_TCPTABLE_OWNER_PID *)malloc(dwSize);
    if (pTcpTable == NULL) {
        std::cerr << "Memory allocation failed" << std::endl;
        return;
    }

    // Get the TCP table
    dwRetVal = GetExtendedTcpTable(pTcpTable, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
    if (dwRetVal != NO_ERROR) {
        std::cerr << "Error getting TCP table" << std::endl;
        free(pTcpTable);
        return;
    }

    // Print TCP connections for the specified process ID
    for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
        if (pTcpTable->table[i].dwOwningPid == processId) {
            sockaddr_in localAddr, remoteAddr;
            localAddr.sin_addr.s_addr = pTcpTable->table[i].dwLocalAddr;
            localAddr.sin_port = htons((u_short)pTcpTable->table[i].dwLocalPort);
            remoteAddr.sin_addr.s_addr = pTcpTable->table[i].dwRemoteAddr;
            remoteAddr.sin_port = htons((u_short)pTcpTable->table[i].dwRemotePort);

            char localIP[INET_ADDRSTRLEN];
            char remoteIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(localAddr.sin_addr), localIP, INET_ADDRSTRLEN);
            inet_ntop(AF_INET, &(remoteAddr.sin_addr), remoteIP, INET_ADDRSTRLEN);

            std::cout << "Local Address: " << localIP << ":" << ntohs(localAddr.sin_port) << std::endl;
            std::cout << "Remote Address: " << remoteIP << ":" << ntohs(remoteAddr.sin_port) << std::endl;
        }
    }

    free(pTcpTable);
}

int main() {
    DWORD processId = 1234; // 你要查询的进程的 ID
    ListTCPConections(processId);

    return 0;
}


