/********************************************************************
 File: ehClient.cpp
 Function: provided interaface to communicate with ehome server. 
 Author: Luobihe
 Date: 2015-12-16
*********************************************************************/

#include "ehClient.h"
#include "platcomm.h"

#ifdef _WIN32

#include <winsock.h>
#pragma comment(lib, "ws2_32.lib")

#else

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#endif

CEHClient::CEHClient(void)
{
    m_sfd = -1;
    m_bIsClosed = 1;
}

/*-------------------------------------------------------------------
 Function: open(ipAddr, port)
 Purpose: open socket and connect to server.
 Parameters: ipAddr -- [IN], server ip address
             port -- [IN], server port
 return: 0 -- success, -1 -- failed
-------------------------------------------------------------------*/
int CEHClient::open(const char* ipAddr, int port)
{
    if (!ipAddr)
        return -1;

    m_sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sfd < 0) 
        return -1;

    struct sockaddr_in srvaddr = {0};
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = inet_addr(ipAddr);
    srvaddr.sin_port = htons(port);

    if (connect(m_sfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr)))
        return -1;

    m_bIsClosed = 0;

    return 0;
}

/*
  open socket, connect to server in local host.
  return: 0 -- success, -1 -- failed.
*/
int CEHClient::open(int port)
{
    char szIp[24] = {0};
    if (getLocalIp(szIp))
            return -1;

    m_sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sfd < 0) 
        return -1;

    struct sockaddr_in srvaddr = {0};
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = inet_addr(szIp);
    srvaddr.sin_port = htons(port);

    if (connect(m_sfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr)))
        return -1;

    m_bIsClosed = 0;

    return 0;
}

void CEHClient::close(void)
{
    if (m_sfd != -1) {
#ifdef _WIN32
        closesocket(m_sfd);
#else
        ::close(m_sfd);
#endif
        m_sfd = -1;

        m_bIsClosed = 1;
    }
}

/*
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
*/

/*-------------------------------------------------------------------
 Function: writeDevByte(iDevNo, value)
 Purpose: write device byte with value.
 Parameters: iDevNo -- [IN], device address
             value -- [IN], value to set
 return: 0 -- success, -1 -- failed
-------------------------------------------------------------------*/
int CEHClient::writeDevByte(int iDevNo, int value)
{
    char szWBuf[12] = {0};
    unsigned short checksum = 0;
    char szRBuf[BUFFER_SIZE] = {0};

    //head
    szWBuf[0] = 0xF0;
    szWBuf[1] = 0x0F;

    //cmd
    szWBuf[2] = NEWBEST_CMD_ADDR_WRITE;

    //address
    szWBuf[3] = iDevNo & 0xFF;
    szWBuf[4] = iDevNo >> 8;

    //data len
    szWBuf[5] = 0x01;
    szWBuf[6] = 0x00;
    szWBuf[7] = 0x00;
    szWBuf[8] = 0x00;

    //data
    szWBuf[9] = value;

    checksum = calcByteSum(szWBuf, 10);
    szWBuf[10] = checksum & 0xFF;
    szWBuf[11] = checksum >> 8;

    if (send(m_sfd, szWBuf, 12, 0) < 0)
        return -1;
    
    if (recv(m_sfd, szRBuf, BUFFER_SIZE, 0) < 0)
        return -1;

    return 0;
}

/*-------------------------------------------------------------------
 Function: readDevByte(iDevNo, pData)
 Purpose: read device byte.
 Parameters: iDevNo -- [IN], device address
             pData -- [OUT], reading result
 return: 0 -- success, -1 -- failed
-------------------------------------------------------------------*/
int CEHClient::readDevByte(int iDevNo, int* pData)
{
    char szRBuf[BUFFER_SIZE] = {0};
    CMD_BODY cmdbody = {0};
    int iDataStartPos = 0;

    if (sendReadDevByteCmd(iDevNo))
        return -1;

    int iLen = recv(m_sfd, szRBuf, BUFFER_SIZE, 0);
    if (iLen < 0)
        return -1;

    //check cmd and get device infomation
    if (getCmdBodyFromInput(szRBuf, iLen, &cmdbody)) 
        return -1;

    *pData = 0;
    iDataStartPos = cmdbody.iDataStartPos;
    for (int i = 0; i < cmdbody.iDataLen; i++)
        *pData += (unsigned char)szRBuf[i + iDataStartPos] << (8 * i);

    return 0;
}

/*-------------------------------------------------------------------
 Function: sendReadDevByteCmd(iDevNo)
 Purpose: send read dev cmd to server
 Parameters: iDevNo -- [IN], device address
 return: 0 -- success, -1 -- failed
-------------------------------------------------------------------*/
int CEHClient::sendReadDevByteCmd(int iDevNo)
{
    char szWBuf[11] = {0};
    //head
    szWBuf[0] = 0xF0;
    szWBuf[1] = 0x0F;

    //cmd
    szWBuf[2] = NEWBEST_CMD_ADDR_READ;

    //address
    szWBuf[3] = iDevNo & 0xFF;
    szWBuf[4] = iDevNo >> 8;

    //data len
    szWBuf[5] = 0x00;
    szWBuf[6] = 0x00;
    szWBuf[7] = 0x00;
    szWBuf[8] = 0x00;

    unsigned short checksum = calcByteSum(szWBuf, 9);
    szWBuf[9] = checksum & 0xFF;
    szWBuf[10] = checksum >> 8;

    if (send(m_sfd, szWBuf, 11, 0) < 0)
        return -1;

    return 0;
}

/*-------------------------------------------------------------------
 Function: getCmdBodyFromInput(szIn, iInLen, p_result)
 Purpose: get the command infomation from input
 Parameters: szIn -- [IN], information from server
             iInLen -- [IN], command length
             p_result -- [OUT], command information gotten
 return: 0 --  correct command
         -1 -- not correct
-------------------------------------------------------------------*/
int CEHClient::getCmdBodyFromInput(const char* szIn, 
                                   int iInLen, 
                                   PCMD_BODY p_result)
{
    if (checkInput(szIn, iInLen))
        return -1;

    p_result->iCmdId = (unsigned char)szIn[2];
    p_result->iAddrId = (unsigned char)szIn[5] + 
        ((unsigned char)szIn[6] << 8);

    p_result->iDataLen = (unsigned char)szIn[7];
    p_result->iDataLen += (unsigned char)szIn[8] << 8;
    p_result->iDataLen += (unsigned char)szIn[9] << (2 * 8);
    p_result->iDataLen += (unsigned char)szIn[10] << (3 * 8);

    p_result->iDataStartPos = p_result->iDataLen ? 11 : -1;

    return 0;
}

/*-------------------------------------------------------------------
 Function: checkInput(szIn, iInLen)
 Purpose: check whether the response from server is correct or not
 Parameters: szIn -- [IN], command form client
             iInLen -- [IN], command length
 return: 0 --  correct command
         -1 -- not correct
-------------------------------------------------------------------*/
int CEHClient::checkInput(const char* szIn, int iInLen)
{
    //check length
    if (iInLen < MIN_SERVER_RES_LEN)
        return -1;

    if ((unsigned char)szIn[0] != 0x0F ||
        (unsigned char)szIn[1] != 0xF0)
        return -1;
    
    unsigned char uchCmdId = (unsigned char)szIn[2];
    if (uchCmdId != NEWBEST_CMD_GETJSONFILE_ACK &&
        uchCmdId != NEWBEST_CMD_ADDR_READ_ACK &&
        uchCmdId != NEWBEST_CMD_ADDR_WRITE_ACK)
        return -1;

    //check response bits
    if (szIn[3] == 0 || szIn[4] == 0)
        return -1;
    
    //get data length
    int iDataLen = 0;
    iDataLen += (unsigned char)szIn[7];
    iDataLen += (unsigned char)szIn[8] << 8;
    iDataLen += (unsigned char)szIn[9] << (2 * 8);
    iDataLen += (unsigned char)szIn[10] << (3 * 8);

    if (iDataLen + MIN_SERVER_RES_LEN != iInLen)
        return -1;

    //check sum;
    unsigned short usCheckSum = calcByteSum(szIn, iInLen - 2);
    if ( ((unsigned char)szIn[iInLen - 2] != (unsigned char)(usCheckSum & 0xff)) ||
         ((unsigned char)szIn[iInLen - 1] != (unsigned char)(usCheckSum >> 8)) )
         return -1;

    return 0;
}

/*-------------------------------------------------------------------
 Function: readDev(ipAddr, port, devno, p_data)
 Purpose: read dev value
 Parameters: ipAddr -- [IN], server ip address
             port -- [IN], server port 
             devno -- [IN], device id
             p_data -- [OUT], value 
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int readDev(const char* ipAddr, int port, int devno, int* p_data)
{
    int ret = 0;
    initSocket();

    CEHClient m_testInst;
    if (ipAddr)
        ret = m_testInst.open(ipAddr, port);
    else
        ret = m_testInst.open(port);
    
    if (!ret)  //open success, then read
        ret = m_testInst.readDevByte(devno, p_data);

    m_testInst.close();
    deinitSocket();

    return ret;
}

/*-------------------------------------------------------------------
 Function: writeDev(ipAddr, port, devno, data)
 Purpose: read dev value
 Parameters: ipAddr -- [IN], server ip address
             port -- [IN], server port 
             devno -- [IN], device id
             data -- [IN], value 
 return: 0 -- success
         -1 -- failed
-------------------------------------------------------------------*/
int writeDev(const char* ipAddr, int port, int devno, int data)
{
    int ret = 0;
    initSocket();

    CEHClient m_testInst;
    if (ipAddr)
        ret = m_testInst.open(ipAddr, port);
    else
        ret = m_testInst.open(port);
    
    if (!ret)  //open success, then write
        ret = m_testInst.writeDevByte(devno, data);

    m_testInst.close();
    deinitSocket();

    return ret;
}