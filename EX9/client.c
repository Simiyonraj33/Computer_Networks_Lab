#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5170

int main()
{
    int sockfd;

    char buffer[100];
    char reply[100];

    char operator;

    int num1;
    int num2;

    struct sockaddr_in serverAddr;
    struct sockaddr_in replyAddr;

    socklen_t addrLen;


    /*
     * Create UDP socket
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
     * Main server address
     */
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr =
        inet_addr("127.0.0.1");


    /*
     * Tell main server that
     * a new client is starting
     */
    strcpy(buffer, "START");

    sendto(sockfd,
           buffer,
           strlen(buffer) + 1,
           0,
           (struct sockaddr *)&serverAddr,
           sizeof(serverAddr));


    /*
     * Receive connection information
     * from child server
     */
    addrLen = sizeof(replyAddr);

    recvfrom(sockfd,
             reply,
             sizeof(reply) - 1,
             0,
             (struct sockaddr *)&replyAddr,
             &addrLen);

    reply[sizeof(reply) - 1] = '\0';


    /*
     * IMPORTANT:
     *
     * The reply comes from the
     * child server's port.
     *
     * Save that address.
     */
    serverAddr = replyAddr;


    /*
     * Connection initialized
     */
    printf("\nUDP client connection initialized.\n");

    printf("Type exit to end the connection.\n");

    printf("Connected to server.\n");


    /*
     * Client remains connected
     */
    while (1)
    {
        printf("\nEnter operator (+, -, *, /) or exit: ");

        scanf(" %s", buffer);


        /*
         * EXIT
         */
        if (strcmp(buffer, "exit") == 0)
        {
            sendto(sockfd,
                   buffer,
                   strlen(buffer) + 1,
                   0,
                   (struct sockaddr *)&serverAddr,
                   sizeof(serverAddr));


            /*
             * Receive exit confirmation
             */
            addrLen = sizeof(serverAddr);

            recvfrom(sockfd,
                     reply,
                     sizeof(reply) - 1,
                     0,
                     (struct sockaddr *)&serverAddr,
                     &addrLen);

            reply[sizeof(reply) - 1] = '\0';


            break;
        }


        /*
         * Operator
         */
        operator = buffer[0];


        /*
         * Get numbers
         */
        printf("Enter first number: ");
        scanf("%d", &num1);

        printf("Enter second number: ");
        scanf("%d", &num2);


        /*
         * Prepare request
         */
        sprintf(buffer,
                "%c %d %d",
                operator,
                num1,
                num2);


        /*
         * Send request
         */
        sendto(sockfd,
               buffer,
               strlen(buffer) + 1,
               0,
               (struct sockaddr *)&serverAddr,
               sizeof(serverAddr));


        /*
         * Receive result
         */
        addrLen = sizeof(serverAddr);

        recvfrom(sockfd,
                 reply,
                 sizeof(reply) - 1,
                 0,
                 (struct sockaddr *)&serverAddr,
                 &addrLen);

        reply[sizeof(reply) - 1] = '\0';

    }


    close(sockfd);

    return 0;
}