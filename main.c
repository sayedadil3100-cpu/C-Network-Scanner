#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

#define START_PORT 1
#define END_PORT 1000
#define TIMEOUT_MS 100

void setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

const char* getServiceName(int port)
{
    switch (port)
    {
        case 20:
        case 21:
            return "FTP";

        case 22:
            return "SSH";

        case 23:
            return "TELNET";

        case 25:
            return "SMTP";

        case 53:
            return "DNS";

        case 80:
            return "HTTP";

        case 110:
            return "POP3";

        case 135:
            return "RPC";

        case 139:
            return "NETBIOS";

        case 143:
            return "IMAP";

        case 443:
            return "HTTPS";

        case 445:
            return "SMB";

        case 3306:
            return "MySQL";

        case 3389:
            return "RDP";

        case 8080:
            return "HTTP-ALT";

        default:
            return "Unknown";
    }
}

int main()
{
    WSADATA wsa;
    struct sockaddr_in target;

    int port;
    int openPorts = 0;

    printf("\n");
    setColor(11);

    printf("====================================================\n");
    printf("              C NETWORK SCANNER\n");
    printf("====================================================\n");

    setColor(7);

    printf("\nTarget      : 127.0.0.1");
    printf("\nPort Range  : %d - %d", START_PORT, END_PORT);
    printf("\nTimeout     : %d ms\n\n", TIMEOUT_MS);

    /* Initialize Winsock */
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        setColor(12);
        printf("[-] Winsock initialization failed.\n");
        setColor(7);

        return 1;
    }

    target.sin_family = AF_INET;
    target.sin_addr.s_addr = inet_addr("127.0.0.1");

    setColor(14);
    printf("Scanning...\n\n");
    setColor(7);

    for (port = START_PORT; port <= END_PORT; port++)
    {
        SOCKET sock;
        u_long mode = 1;

        fd_set writefds;
        struct timeval timeout;

        int result;

        /* Create socket */
        sock = socket(AF_INET, SOCK_STREAM, 0);

        if (sock == INVALID_SOCKET)
        {
            continue;
        }

        target.sin_port = htons(port);

        /* Non-blocking socket */
        ioctlsocket(sock, FIONBIO, &mode);

        /* Attempt connection */
        connect(
            sock,
            (struct sockaddr*)&target,
            sizeof(target)
        );

        /* Wait for connection */
        FD_ZERO(&writefds);
        FD_SET(sock, &writefds);

        timeout.tv_sec = 0;
        timeout.tv_usec = TIMEOUT_MS * 1000;

        result = select(
            0,
            NULL,
            &writefds,
            NULL,
            &timeout
        );

        if (result > 0 && FD_ISSET(sock, &writefds))
        {
            int error = 0;
            int errorSize = sizeof(error);

            getsockopt(
                sock,
                SOL_SOCKET,
                SO_ERROR,
                (char*)&error,
                &errorSize
            );

            if (error == 0)
            {
                setColor(10);

                printf(
                    "[+] Port %-5d OPEN    %-10s\n",
                    port,
                    getServiceName(port)
                );

                setColor(7);

                openPorts++;
            }
        }

        closesocket(sock);

        /* Progress */
        if (port % 50 == 0)
        {
            int progress;

            progress =
                ((port - START_PORT + 1) * 100)
                / (END_PORT - START_PORT + 1);

            setColor(8);

            printf(
                "    Progress: %3d%%\r",
                progress
            );

            setColor(7);
        }
    }

    printf("\n\n");

    setColor(11);

    printf("====================================================\n");

    setColor(10);

    printf("              SCAN COMPLETE\n");

    setColor(11);

    printf("====================================================\n");

    setColor(7);

    printf("\nOpen ports found: %d\n", openPorts);

    printf("\nTarget scanned: 127.0.0.1\n");

    setColor(8);

    printf("\nPress Enter to exit...");

    setColor(7);

    getchar();

    WSACleanup();

    return 0;
}
