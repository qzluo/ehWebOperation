#include <stdio.h>
#include "platcomm.h"
#include "testEhClient.h"
#include "ehClient.h"

int main(int argc, char* argv[])
{
    /*
    initSocket();

    CEHClient m_testInst;
    m_testInst.open("192.168.1.153", 2222);
    m_testInst.writeDevByte(4, 0);

    platSleep(3);

    m_testInst.writeDevByte(4, 1);

    m_testInst.close();

    deinitSocket();

    printf("content-type:text/html\n\n");
    printf("<html><head>");
    printf("<meta http-equiv='Content-Type' content='text/html; charset=gb2312' />");
    printf("<title>Start service</title></head>");

    printf("<body><p>启动服务成功</p></body></html>\n");
    */

    CTestEhClient tester;
    tester.test();

    return 0;
}