#include <stdio.h>
#include "testEhClient.h"
#include "platcomm.h"


void CTestEhClient::testOpen(void)
{
    printf("---------------test open-----------------\n");
    int ret = 0;
    if (m_testInst.open("192.168.1.153", 2222))
        printf("client open socket failed\n");
    else
        printf("client open socket success\n");
}

void CTestEhClient::testWriteDevByte(void)
{
    int ret = 0;
    printf("---------------test WriteDevByte-----------------\n");
    ret = m_testInst.writeDevByte(4, 0);
    if (ret < 0)
        printf("write dev failed\n");
    else
        printf("write dev success\n");

    platSleep(3);

    ret = m_testInst.writeDevByte(4, 1);
    if (ret < 0)
        printf("write dev failed\n");
    else
        printf("write dev success\n");
}

void CTestEhClient::testReadDevByte(void)
{
    int ret = 0;
    int data = 0;
    int addr = 0;
    printf("---------------test ReadDevByte-----------------\n");
    for (addr = 1; addr < 10; addr++) {
        ret = m_testInst.readDevByte(addr, &data);
        if (ret < 0)
            printf("read dev failed, addr = %d\n", addr);
        else
            printf("write dev success, addr = %d, value = %d\n",
            addr, data);

        platSleep(3);
    }
}

void CTestEhClient::testReadDev(void)
{
    int ret = 0;
    int data = 0;
    int addr = 0;
    printf("---------------test readDev-----------------\n");
    for (addr = 1; addr < 10; addr++) {
        ret = readDev("192.168.1.130", 2222, addr, &data);
        if (ret < 0)
            printf("read dev failed, addr = %d\n", addr);
        else
            printf("read dev success, addr = %d, value = %d\n",
            addr, data);

        platSleep(3);
    }
}

void CTestEhClient::testWriteDev(void)
{
    int ret = 0;
    int data = 0;
    int addr = 0;
    printf("---------------test WriteDev-----------------\n");
    addr = 4;
    data = 0;
    ret = writeDev("192.168.1.130", 2222, addr, data);
    if (ret < 0)
        printf("write dev failed, addr = %d\n", addr);
    else
        printf("write dev success, addr = %d, value = %d\n",
        addr, data);

    platSleep(3);

    data = 1;
    ret = writeDev("192.168.1.130", 2222, addr, data);
    if (ret < 0)
        printf("write dev failed, addr = %d\n", addr);
    else
        printf("write dev success, addr = %d, value = %d\n",
        addr, data);
}

void CTestEhClient::test(void)
{
    //testOpen();

    //testWriteDevByte();
    //testReadDevByte();

    //m_testInst.close();

    testWriteDev();
}
