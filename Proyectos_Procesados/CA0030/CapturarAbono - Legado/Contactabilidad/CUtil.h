#pragma once
#include "stdafx.h"
#include "C_Archiv.hpp"
#include "CExcepcion.h"
class CUtil
{
private:

public:
    CUtil();
    ~CUtil(void);
    void GrabarLog(char *cLog);
    void GrabarLog(CString sLog);
    void Click(int iLeft, int iRight);
    void ClickCenter();
    void SendAltTab();
};