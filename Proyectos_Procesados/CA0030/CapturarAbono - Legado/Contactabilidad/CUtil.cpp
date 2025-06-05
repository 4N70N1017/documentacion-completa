#include "CUtil.h"
#include "general.hpp"
CUtil::CUtil()
{
}
CUtil::~CUtil(void)
{
}
void CUtil::GrabarLog(char *cTexto)
{
    char sArch[80] = { 0 };
	char cTextoGrabar[2048] = {0};
    CTime tFecha = CTime::GetCurrentTime();

	if(sprintf_s(sArch, "C:\\sys\\mem\\CapturarAbonos%02d%02d.log", tFecha.GetMonth(), tFecha.GetDay()) < 0){;}

    if(sprintf_s(cTextoGrabar, "%04d-%02d-%02d %02d:%02d:%02d %s\n",
        tFecha.GetYear(), tFecha.GetMonth(), tFecha.GetDay(), tFecha.GetHour(), tFecha.GetMinute(),
		tFecha.GetSecond(), cTexto) < 0){;}

    C_Archivo arch(sArch, &cTextoGrabar, strnlen(cTextoGrabar, sizeof(cTextoGrabar)));
    arch.posicionar((long)0, BASE_FINAL);
    arch.grabar(cTextoGrabar, (unsigned int)strnlen(cTextoGrabar, sizeof(cTextoGrabar)));
}

void CUtil::GrabarLog(CString sLog)
{
	char cLog[2048] = {0};
	if(sprintf_s(cLog, sLog) < 0){;}
    this->GrabarLog(cLog);

}

void CUtil::ClickCenter()
{
    int iWidth = 0;
    int iHeight = 0;
    gnConsultarResolucion(iWidth, iHeight);
    iWidth = iWidth / 2;
    iHeight = iHeight / 2;
    Click(iWidth, iHeight);
}

void CUtil::Click(int iWidth, int iHeight)
{
    SetCursorPos(iWidth, iHeight);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}
void CUtil::SendAltTab()
{
    // press DOWN "Alt-Tab"
    keybd_event(VK_MENU, 0, 0, 0);
    keybd_event(VK_TAB, 0, 0, 0);
    Sleep(100);
    // stop pressing "Alt-Tab"
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
}