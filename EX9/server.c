#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define PORT 5170

int add(int a, int b)
{
    return a + b;
}

int subtract(int a, int b)
{
    return a - b;
}

int multiply(int a, int b)
{
    return a * b;
}

int divide(int a, int b)
{
    return a / b;
}


/* Function to handle one client */
void handle_client(struct sockaddr_in clientAddr)
{
    int sockfd;

    char buffer[100];
    char reply[100];

    char operator;
    int num1, num2;
    int n;

    struct sockaddr_in localAddr;
    socklen_t addrLen;

    /*
     * Create a new UDP socket
     * for this client
     */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("Child socket creation failed");
        exit(1);
    }

    /*
     * Bind child socket to
     * any available port
     */
    memset(&localAddr, 0, sizeof(localAddr));

    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons(0);

    if (bind(sockfd,
             (struct sockaddr *)&localAddr,
             sizeof(localAddr)) < 0)
    {
        perror("Child bind failed");
        close(sockfd);
        exit(1);
    }

    /*
     * Get the port assigned to child
     */
    addrLen = sizeof(localAddr);

    getsockname(sockfd,
                (struct sockaddr *)&localAddr,
                &addrLen);

    int childPort = ntohs(localAddr.sin_port);

    /*
     * Display client information
     */
    printf("\n[Child PID %d] Chat session started "
           "with client %s:%d\n",
           getpid(),
           inet_ntoa(clientAddr.sin_addr),
           ntohs(clientAddr.sin_port));

    fflush(stdout);

    /*
     * Tell client that connection is initialized
     */
    sprintf(reply,
            "CONNECTED %d",
            childPort);

    sendto(sockfd,
           reply,
           strlen(reply) + 1,
           0,
           (struct sockaddr *)&clientAddr,
           sizeof(clientAddr));


    /*
     * Keep serving this client
     */
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        n = recvfrom(sockfd,
                     buffer,
                     sizeof(buffer) - 1,
                     0,
                     (struct sockaddr *)&clientAddr,
                     &addrLen);

        if (n < 0)
        {
            continue;
        }

        buffer[n] = '\0';


        /*
         * EXIT
         */
        if (strcmp(buffer, "exit") == 0)
        {
            strcpy(reply, "Connection ended");

            sendto(sockfd,
                   reply,
                   strlen(reply) + 1,
                   0,
                   (struct sockaddr *)&clientAddr,
                   addrLen);

            printf("[Child PID %d] Client disconnected.\n",
                   getpid());

            fflush(stdout);

            close(sockfd);

            exit(0);
        }


        /*
         * Read operator and numbers
         */
        if (sscanf(buffer,
                   " %c %d %d",
                   &operator,
                   &num1,
                   &num2) != 3)
        {
            strcpy(reply, "Invalid input");

            sendto(sockfd,
                   reply,
                   strlen(reply) + 1,
                   0,
                   (struct sockaddr *)&clientAddr,
                   addrLen);

            continue;
        }


        /*
         * Display request on server
         */
        printf("[Child PID %d] Client: %d %c %d\n",
               getpid(),
               num1,
               operator,
               num2);

        fflush(stdout);


        /*
         * Perform operation
         */
        switch (operator)
        {
            case '+':

                sprintf(reply,
                        "Result: %d",
                        add(num1, num2));

                break;


            case '-':

                sprintf(reply,
                        "Result: %d",
                        subtract(num1, num2));

                break;


            case '*':

                sprintf(reply,
                        "Result: %d",
                        multiply(num1, num2));

                break;


            case '/':

                if (num2 == 0)
                {
                    strcpy(reply,
                           "Cannot divide by zero");
                }
                else
                {
                    sprintf(reply,
                            "Result: %d",
                            divide(num1, num2));
                }

                break;


            default:

                strcpy(reply,
                       "Invalid operator");

                break;
        }


        /*
         * Display result on SERVER
         */
        printf("[Child PID %d] Server reply: %s\n",
               getpid(),
               reply);

        fflush(stdout);


        /*
         * Send result to CLIENT
         */
        sendto(sockfd,
               reply,
               strlen(reply) + 1,
               0,
               (struct sockaddr *)&clientAddr,
               addrLen);

        /*
         * IMPORTANT:
         *
         * Do NOT disconnect here.
         *
         * Client remains connected
         * for another operation.
         */
    }
}


int main()
{
    int sockfd;

    char buffer[100];

    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;

    socklen_t addrLen;

    int n;
    int pid;


    /*
     * Make printf appear immediately
     */
    setbuf(stdout, NULL);


    /*
     * Prevent zombie processes
     */
    signal(SIGCHLD, SIG_IGN);


    /*
     * Create main UDP socket
     */
    sockfd = socket(AF_INET,
                    SOCK_DGRAM,
                    0);

    if (sockfd < 0)
    {
        printf("Socket creation failed\n");
        return 1;
    }


    /*
     * Server address
     */
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;


    /*
     * Bind main server
     */
    if (bind(sockfd,
             (struct sockaddr *)&serverAddr,
             sizeof(serverAddr)) < 0)
    {
        printf("Bind failed\n");

        close(sockfd);

        return 1;
    }


    printf("Concurrent UDP Chat Server running "
           "on port %d...\n",
           PORT);


    /*
     * Wait for new clients
     */
    while (1)
    {
        addrLen = sizeof(clientAddr);

        n = recvfrom(sockfd,
                     buffer,
                     sizeof(buffer) - 1,
                     0,
                     (struct sockaddr *)&clientAddr,
                     &addrLen);

        if (n < 0)
        {
            continue;
        }

        buffer[n] = '\0';


        /*
         * New client
         */
        if (strcmp(buffer, "START") == 0)
        {
            /*
             * Create child
             */
            pid = fork();


            if (pid < 0)
            {
                printf("Fork failed\n");
                continue;
            }


            /*
             * CHILD
             */
            if (pid == 0)
            {
                /*
                 * Close parent's listening socket
                 */
                close(sockfd);

                /*
                 * Handle this client
                 */
                handle_client(clientAddr);

                exit(0);
            }


            /*
             * PARENT
             *
             * Parent continues waiting
             * for new clients.
             */
            printf("\n[Parent] New client accepted "
                   "with Child PID %d\n",
                   pid);

            fflush(stdout);
        }
    }


    close(sockfd);

    return 0;
}