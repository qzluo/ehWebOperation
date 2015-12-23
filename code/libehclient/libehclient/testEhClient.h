#ifndef __TESTEHCLIENT_H__
#define __TESTEHCLIENT_H__

#include "ehClient.h"

class CTestEhClient
{
private:
    CEHClient m_testInst;

    void testOpen(void);
    void testWriteDevByte(void);
    void testReadDevByte(void);
    void testReadDev(void);
    void testWriteDev(void);

public:
    void test(void);
};

#endif  //__TESTEHCLIENT_H__