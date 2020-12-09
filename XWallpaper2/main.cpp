#include "XWallpaper2.h"
#include <QtWidgets/QApplication>
#include <qt_windows.h>

//ö�ٴ��ڻص�
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	HWND p = FindWindowEx(hwnd, NULL, L"SHELLDLL_DefView", NULL);
	HWND* ret = (HWND*)lParam;

	if (p)
	{
		// Gets the WorkerW Window after the current one.
		*ret = FindWindowEx(NULL, hwnd, L"WorkerW", NULL);
	}
	return true;
}

HWND findDesktopWnd()
{
	//�״β��ұ������ڣ�
	HWND wallpaper_hwnd = nullptr;
	EnumWindows(EnumWindowsProc, (LPARAM)&wallpaper_hwnd);
	if (wallpaper_hwnd != nullptr)
	{
		return wallpaper_hwnd;
	}
	//���û���ҵ���������ProgMan����������Ϣ��
	//�����ڽ��յ������Ϣ֮��ᴴ��������Ҫ�Ĵ��ڡ�
	HWND progman = ::FindWindow(L"ProgMan", NULL);
	SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);
	//�ٲ���һ��
	EnumWindows(EnumWindowsProc, (LPARAM)&wallpaper_hwnd);
	return wallpaper_hwnd;

}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XWallpaper2 w;
	w.setDeskopWnd(findDesktopWnd());
    w.show();
    return a.exec();
}
