/* =====================================================================
   SIMPLE NETWORK LAYER SIMULATOR (Beginner Friendly Version)
   ---------------------------------------------------------------------
   Changes in this version:
   1. The message is typed by the user, saved into message.txt,
      and then the program reads it back from that file.
   2. Every layer now prints its own STREAM (the bits at that layer)
      and TOTAL BITS (how many bits that stream has).
   3. The Tail (trailer) of every frame is always 8 zero bits.
   ===================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TABLE_SIZE 10        /* small table size, easy to understand */
#define MAX_BITS   6000       /* max size of the binary message       */

/* ---------- Hash table arrays (3 columns: URL, IP, MAC) ---------- */
char urlTable[TABLE_SIZE][50];
char ipTable[TABLE_SIZE][20];
char macTable[TABLE_SIZE][20];
int  used[TABLE_SIZE] = {0};   /* 1 if slot is filled, 0 if empty     */

/* ======================================================================
   STEP 0: SIMPLE HASH FUNCTION
   Add up the ASCII values of every character in the URL, then take
   the remainder (%) with the table size. That is the "home" position.
   ====================================================================== */
int hashFunction(char url[]) {
    int sum = 0;
    int i;
    for (i = 0; url[i] != '\0'; i++) {
        sum = sum + url[i];
    }
    return sum % TABLE_SIZE;
}

/* Insert a URL/IP/MAC into the table. If the slot is taken,
   move to the next slot (this is called LINEAR PROBING). */
void insertURL(char url[], char ip[], char mac[]) {
    int index = hashFunction(url);
    int count = 0;

    while (used[index] == 1 && count < TABLE_SIZE) {
        if (strcmp(urlTable[index], url) == 0) {
            return; /* already in the table, do nothing */
        }
        index = (index + 1) % TABLE_SIZE;  /* try next slot */
        count++;
    }

    strcpy(urlTable[index], url);
    strcpy(ipTable[index], ip);
    strcpy(macTable[index], mac);
    used[index] = 1;
}

/* Search for a URL. Returns the slot index, or -1 if not found. */
int searchURL(char url[]) {
    int index = hashFunction(url);
    int count = 0;

    while (count < TABLE_SIZE) {
        if (used[index] == 1 && strcmp(urlTable[index], url) == 0) {
            return index;
        }
        index = (index + 1) % TABLE_SIZE;
        count++;
    }
    return -1;
}

/* Print the whole hash table so we can see what is inside it. */
void printTable() {
    int i;
    printf("---------------------------------------------------\n");
    printf(" HASH TABLE (URL -> IP -> MAC)\n");
    printf("---------------------------------------------------\n");
    for (i = 0; i < TABLE_SIZE; i++) {
        if (used[i] == 1) {
            printf(" Slot %d : %-15s | %-15s | %s\n",
                   i, urlTable[i], ipTable[i], macTable[i]);
        }
    }
    printf("---------------------------------------------------\n\n");
}

/* ======================================================================
   BINARY CONVERSION FUNCTIONS
   ====================================================================== */

/* Convert one byte (0-255) into 8 characters of binary text, e.g. "01001000" */
void byteToBinary(int number, char result[]) {
    int i;
    for (i = 7; i >= 0; i--) {
        if ((number >> i) & 1) {
            result[7 - i] = '1';
        } else {
            result[7 - i] = '0';
        }
    }
    result[8] = '\0';
}

/* Convert a number into 16-bit binary text (used for port numbers) */
void numberToBinary16(int number, char result[]) {
    int i;
    for (i = 15; i >= 0; i--) {
        if ((number >> i) & 1) {
            result[15 - i] = '1';
        } else {
            result[15 - i] = '0';
        }
    }
    result[16] = '\0';
}

/* Convert "192.168.1.1" style IP address into 32-bit binary text */
void ipToBinary32(char ip[], char result[]) {
    int a, b, c, d;
    char piece[9];
    sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d);

    result[0] = '\0';
    byteToBinary(a, piece); strcat(result, piece);
    byteToBinary(b, piece); strcat(result, piece);
    byteToBinary(c, piece); strcat(result, piece);
    byteToBinary(d, piece); strcat(result, piece);
}

/* Convert "AA:BB:CC:DD:EE:FF" style MAC address into 48-bit binary text */
void macToBinary48(char mac[], char result[]) {
    int b0, b1, b2, b3, b4, b5;
    char piece[9];
    sscanf(mac, "%x:%x:%x:%x:%x:%x", &b0, &b1, &b2, &b3, &b4, &b5);

    result[0] = '\0';
    byteToBinary(b0, piece); strcat(result, piece);
    byteToBinary(b1, piece); strcat(result, piece);
    byteToBinary(b2, piece); strcat(result, piece);
    byteToBinary(b3, piece); strcat(result, piece);
    byteToBinary(b4, piece); strcat(result, piece);
    byteToBinary(b5, piece); strcat(result, piece);
}

/* Create a random IP address like "182.45.200.9" */
void makeRandomIP(char ip[]) {
    int a = 1 + rand() % 223;
    int b = rand() % 256;
    int c = rand() % 256;
    int d = 1 + rand() % 254;
    sprintf(ip, "%d.%d.%d.%d", a, b, c, d);
}

/* Create a random MAC address like "3F:AA:12:9B:00:E1" */
void makeRandomMAC(char mac[]) {
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
            rand() % 256, rand() % 256, rand() % 256,
            rand() % 256, rand() % 256, rand() % 256);
}

/* ======================================================================
   MAIN PROGRAM
   ====================================================================== */
int main() {
    srand(time(NULL));   /* seed the random number generator */

    printf("=====================================================\n");
    printf("     SIMPLE 4-LAYER NETWORK SIMULATOR (in binary)\n");
    printf("=====================================================\n\n");

    /* ---------- Preload 5 well-known websites into the table ---------- */
    insertURL("google.com",    "142.250.193.14",  "3C:5A:B4:1D:9F:02");
    insertURL("youtube.com",   "142.250.72.14",   "A4:5E:60:D3:2B:19");
    insertURL("facebook.com",  "157.240.22.35",   "F0:2F:74:6B:88:11");
    insertURL("amazon.com",    "205.251.242.103", "B8:27:EB:9A:3C:44");
    insertURL("wikipedia.org", "208.80.154.224",  "00:1A:2B:3C:4D:5E");

    printTable();

    /* ---------- Ask the user for source and destination URLs ---------- */
    char srcUrl[50], dstUrl[50];
    printf("Enter SOURCE URL (example: google.com): ");
    scanf("%s", srcUrl);
    printf("Enter DESTINATION URL (example: youtube.com): ");
    scanf("%s", dstUrl);
    getchar();   /* remove the leftover newline left behind by scanf */

    /* ---------- Look up source URL, add it if it is missing ---------- */
    int srcIndex = searchURL(srcUrl);
    if (srcIndex == -1) {
        char newIp[20], newMac[20];
        makeRandomIP(newIp);
        makeRandomMAC(newMac);
        insertURL(srcUrl, newIp, newMac);
        srcIndex = searchURL(srcUrl);
        printf("\n(\"%s\" was not in the table, so a new IP/MAC was created)\n", srcUrl);
    }

    /* ---------- Look up destination URL, add it if it is missing ---------- */
    int dstIndex = searchURL(dstUrl);
    if (dstIndex == -1) {
        char newIp[20], newMac[20];
        makeRandomIP(newIp);
        makeRandomMAC(newMac);
        insertURL(dstUrl, newIp, newMac);
        dstIndex = searchURL(dstUrl);
        printf("\n(\"%s\" was not in the table, so a new IP/MAC was created)\n", dstUrl);
    }

    /* Copy the resolved addresses into simple variables */
    char srcIP[20], dstIP[20], srcMAC[20], dstMAC[20];
    strcpy(srcIP,  ipTable[srcIndex]);
    strcpy(dstIP,  ipTable[dstIndex]);
    strcpy(srcMAC, macTable[srcIndex]);
    strcpy(dstMAC, macTable[dstIndex]);

    /* Convert them into binary form */
    char srcIPBin[40], dstIPBin[40], srcMACBin[60], dstMACBin[60];
    ipToBinary32(srcIP, srcIPBin);
    ipToBinary32(dstIP, dstIPBin);
    macToBinary48(srcMAC, srcMACBin);
    macToBinary48(dstMAC, dstMACBin);

    printf("\n----------------- ADDRESS RESOLUTION -----------------\n");
    printf("SOURCE      : %s -> IP %s -> MAC %s\n", srcUrl, srcIP, srcMAC);
    printf("   IP  binary  (32 bits): %s\n", srcIPBin);
    printf("   MAC binary  (48 bits): %s\n", srcMACBin);
    printf("DESTINATION : %s -> IP %s -> MAC %s\n", dstUrl, dstIP, dstMAC);
    printf("   IP  binary  (32 bits): %s\n", dstIPBin);
    printf("   MAC binary  (48 bits): %s\n", dstMACBin);
    printf("-------------------------------------------------------\n\n");

    /* ======================================================================
       CHANGE 1: Ask the user to type a message, SAVE it to message.txt,
       then OPEN message.txt again and READ it back from the file.
       ====================================================================== */
    char typedMessage[500];
    printf("Enter the message you want to send: ");
    fgets(typedMessage, sizeof(typedMessage), stdin);

    /* remove the newline character that fgets keeps at the end */
    int tlen = strlen(typedMessage);
    if (tlen > 0 && typedMessage[tlen - 1] == '\n') {
        typedMessage[tlen - 1] = '\0';
    }

    /* Step A: SAVE the typed message into message.txt */
    FILE *writeFile = fopen("message.txt", "w");
    if (writeFile == NULL) {
        printf("ERROR: could not create message.txt\n");
        return 1;
    }
    fprintf(writeFile, "%s", typedMessage);
    fclose(writeFile);
    printf("(Message saved to message.txt)\n\n");

    /* Step B: READ the message back from message.txt */
    FILE *readFile = fopen("message.txt", "r");
    if (readFile == NULL) {
        printf("ERROR: could not open message.txt\n");
        return 1;
    }

    char message[500];
    int msgLength = 0;
    int c;
    while ((c = fgetc(readFile)) != EOF && msgLength < 499) {
        if (c == '\n' || c == '\r') continue;   /* skip newlines */
        message[msgLength] = (char) c;
        msgLength++;
    }
    message[msgLength] = '\0';
    fclose(readFile);

    /* ======================================================================
       LAYER 1: APPLICATION LAYER
       Turn the message into binary bits (8 bits per character).
       ====================================================================== */
    printf("=============== APPLICATION LAYER ===============\n");
    printf("Message read from message.txt: \"%s\"\n\n", message);

    char bits[MAX_BITS];
    bits[0] = '\0';
    int i;
    for (i = 0; i < msgLength; i++) {
        char oneByte[9];
        byteToBinary((int)(unsigned char)message[i], oneByte);
        strcat(bits, oneByte);
        printf("  '%c'  ->  %s\n", message[i], oneByte);
    }
    int totalBits = msgLength * 8;

    printf("\nStream      : %s\n", bits);
    printf("Total Bits  : %d bits\n", totalBits);
    printf("===================================================\n\n");

    /* ======================================================================
       LAYER 2: TRANSPORT LAYER
       Random port numbers between 1024 and 65535.
       CHANGE 2: also show the Stream (ports + data) and its Total Bits.
       ====================================================================== */
    int srcPort = 1024 + rand() % (65535 - 1024 + 1);
    int dstPort = 1024 + rand() % (65535 - 1024 + 1);
    char srcPortBin[17], dstPortBin[17];
    numberToBinary16(srcPort, srcPortBin);
    numberToBinary16(dstPort, dstPortBin);

    char transportStream[MAX_BITS];
    sprintf(transportStream, "%s%s%s", srcPortBin, dstPortBin, bits);
    int transportTotalBits = 16 + 16 + totalBits;

    printf("=============== TRANSPORT LAYER ===============\n");
    printf("Source Port      : %d  (binary: %s)\n", srcPort, srcPortBin);
    printf("Destination Port : %d  (binary: %s)\n", dstPort, dstPortBin);
    printf("Stream           : %s\n", transportStream);
    printf("Total Bits       : %d bits\n", transportTotalBits);
    printf("=================================================\n\n");

    /* ======================================================================
       LAYER 3: NETWORK LAYER
       Split the bitstream into 16-bit packets (pad last one with zeros).
       CHANGE 2: also show the Stream (IPs + data) and its Total Bits.
       ====================================================================== */
    char networkStream[MAX_BITS];
    sprintf(networkStream, "%s%s%s", srcIPBin, dstIPBin, bits);
    int networkTotalBits = 32 + 32 + totalBits;

    printf("=============== NETWORK LAYER ===============\n");
    printf("Source IP        : %s\n", srcIPBin);
    printf("Destination IP   : %s\n", dstIPBin);
    printf("Stream           : %s\n", networkStream);
    printf("Total Bits       : %d bits\n\n", networkTotalBits);

    int packetSize = 16;
    int numPackets = totalBits / packetSize;
    if (totalBits % packetSize != 0) {
        numPackets++;   /* one extra packet for leftover bits */
    }

    char packets[200][17];   /* up to 200 packets, 16 bits each */

    printf("Total packets: %d\n\n", numPackets);

    int p;
    for (p = 0; p < numPackets; p++) {
        int start = p * packetSize;
        int b;
        for (b = 0; b < packetSize; b++) {
            int pos = start + b;
            if (pos < totalBits) {
                packets[p][b] = bits[pos];
            } else {
                packets[p][b] = '0';   /* zero padding */
            }
        }
        packets[p][packetSize] = '\0';

        printf("Packet %d:\n", p + 1);
        printf("  Data      : %s\n", packets[p]);
        printf("  Src IP    : %s\n", srcIPBin);
        printf("  Dst IP    : %s\n\n", dstIPBin);
    }
    printf("===============================================\n\n");

    /* ======================================================================
       LAYER 4: DATA LINK LAYER
       Split each 16-bit packet into two 8-bit frames.
       CHANGE 2: show Stream (MACs + IPs + data + tail) and its Total Bits.
       CHANGE 3: the Tail of every frame is always 8 zero bits.
       ====================================================================== */
    char tail[9] = "00000000";   /* Tail is always 8 zero bits */

    char dataLinkStream[MAX_BITS];
    sprintf(dataLinkStream, "%s%s%s%s", srcMACBin, dstMACBin, networkStream, tail);
    int dataLinkTotalBits = 48 + 48 + networkTotalBits + 8;

    printf("=============== DATA LINK LAYER ===============\n");
    printf("Source MAC       : %s\n", srcMACBin);
    printf("Destination MAC  : %s\n", dstMACBin);
    printf("Stream           : %s\n", dataLinkStream);
    printf("Total Bits       : %d bits\n\n", dataLinkTotalBits);

    int frameSize = 8;
    int framesPerPacket = packetSize / frameSize;   /* = 2 */
    int totalFrames = numPackets * framesPerPacket;

    printf("Total frames: %d\n\n", totalFrames);

    printf("====Each frame and its content======\n\n");

    int frameNumber;
    for (p = 0; p < numPackets; p++) {
        for (frameNumber = 1; frameNumber <= framesPerPacket; frameNumber++) {

            char frameData[9];
            int start = (frameNumber - 1) * frameSize;
            int k;
            for (k = 0; k < frameSize; k++) {
                frameData[k] = packets[p][start + k];
            }
            frameData[frameSize] = '\0';

            printf("---------------------------------------------\n");
            printf("Packet No : %d\n", p + 1);
            printf("Frame No  : %d\n", frameNumber);
            printf("Source Port      : %s\n", srcPortBin);
            printf("Destination Port : %s\n\n", dstPortBin);
            printf("Source IP        : %s\n", srcIPBin);
            printf("Destination IP   : %s\n", dstIPBin);
            printf("Source MAC       : %s\n", srcMACBin);
            printf("Destination MAC  : %s\n", dstMACBin);
            printf("Frame Data       : %s\n", frameData);
            printf("Tail             : %s\n", tail);

            /* ---- Extra line: Error Code on the LEFT, everything else on the RIGHT,
                   all joined together in one single line (like a stream) ---- */
            printf("Frame Stream     : %s%s%s%s\n", tail, srcMACBin, dstMACBin, frameData);
        }
    }
    printf("---------------------------------------------\n\n");

    /* ======================================================================
       FINAL SUMMARY
       ====================================================================== */
    printf("=============== SUMMARY ===============\n");
    printf("Message              : \"%s\"\n", message);
    printf("Application bits     : %d\n", totalBits);
    printf("Transport layer bits : %d\n", transportTotalBits);
    printf("Network layer bits   : %d\n", networkTotalBits);
    printf("Data Link layer bits : %d\n", dataLinkTotalBits);
    printf("Total packets        : %d\n", numPackets);
    printf("Total frames         : %d\n", totalFrames);
    printf("=========================================\n");

    return 0;
}
