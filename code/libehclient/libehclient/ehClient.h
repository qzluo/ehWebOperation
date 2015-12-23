/********************************************************************
 File: ehClient.h
 Function: provided interaface to communicate with ehome server. 
 Author: Luobihe
 Date: 2015-12-16
*********************************************************************/

/*
2015-8-13
///////////////////////////////////////////////////////////
ehome server protocol 
///////////////////////////////////////////////////////////

commmand packet from client

position	contend             sense
1	         0XF0               protocol header
2	         0X0F               protocol header
3  	         0X02               command id
4-5          0X01,00            address id
6-9          0X00,00,00,00      data length
10	         0X--               data, may be empty
бн	         0X--               data, may be empty
last-2       0X--               data, may be empty
last-1       0X--               check sum
last	     0X--               check sum

///////////////////////////////////////////////////////////

commmand packet back to client

position	contend             sense
1	         0X0F               protocol header
2	         0XF0               protocol header
3  	         0X82               command id
4            0X01               command check result
5            0X01               result is gotten or not
6-7          0X01,00            address id
8-11         0X00,00,00,00      data length
12	         0X--               data, may be empty
бн	         0X--               data, may be empty
last-2       0X--               data, may be empty
last-1       0X--               check sum
last	     0X--               check sum

*/

#ifndef __EHCLIENT_H__
#define __EHCLIENT_H__

//command id
#define NEWBEST_CMD_GETJSONFILE      0X01
#define NEWBEST_CMD_ADDR_READ        0X02
#define NEWBEST_CMD_ADDR_WRITE       0X03
#define NEWBEST_CMD_GETJSONFILE_ACK  0X81
#define NEWBEST_CMD_ADDR_READ_ACK    0X82
#define NEWBEST_CMD_ADDR_WRITE_ACK   0X83
#define NEWBEST_CMD_ADDR_PUSHOUT     0X84

#define MIN_SERVER_RES_LEN   13  

#define BUFFER_SIZE   1024

//effective command data
typedef struct {
    int iCmdId;             //command id, read or write
    int iAddrId;            //address by a number
    int iDataLen;           //data len
    int iDataStartPos;      //data start position, -1 represents uneffective
} CMD_BODY, *PCMD_BODY;

class CEHClient {
private:
    int m_sfd;
    int m_bIsClosed;

public:
    CEHClient(void);

    /*
        open socket, connect to server.
        return: 0 -- success, -1 -- failed.
    */
    int open(const char* ipAddr, int port);

    /*
        open socket, connect to server in local host.
        return: 0 -- success, -1 -- failed.
    */
    int open(int port);

    /*
        close socket
    */
    void close(void);

    /*
        operate device.
        return: 0 -- success, -1 -- failed.
    */
    int writeDevByte(int iDevNo, int value);

    /*
        read device.
        return: 0 -- success, -1 -- failed.
    */
    int readDevByte(int iDevNo, int* pData);

private:
    int sendReadDevByteCmd(int iDevNo);

    //get the command infomation from input
    //0 -- correct, -1 -- not correct
    int getCmdBodyFromInput(const char* szIn, int iInLen, PCMD_BODY p_result);

    int checkInput(const char* szBuf, int iLen);
};

int readDev(const char* ipAddr, int port, int devno, int* p_data);
int writeDev(const char* ipAddr, int port, int devno, int data);

#endif  //__EHCLIENT_H__