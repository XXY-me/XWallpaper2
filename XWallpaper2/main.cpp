#include "XWallpaper2.h"
#include <QtWidgets/QApplication>
#include <qt_windows.h>

//枚举窗口回调
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
	//首次查找背景窗口，
	HWND wallpaper_hwnd = nullptr;
	EnumWindows(EnumWindowsProc, (LPARAM)&wallpaper_hwnd);
	if (wallpaper_hwnd != nullptr)
	{
		return wallpaper_hwnd;
	}
	//如果没有找到，尝试向ProgMan发送以下消息，
	//当窗口接收到这个消息之后会创建我们需要的窗口。
	HWND progman = ::FindWindow(L"ProgMan", NULL);
	SendMessageTimeout(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, nullptr);
	//再查找一遍
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
