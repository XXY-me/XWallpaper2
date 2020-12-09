#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_XWallpaper2.h"
#include <QSystemTrayIcon>
#include <QWebEngineView>

class XWallpaper2 : public QMainWindow
{
    Q_OBJECT

public:
    XWallpaper2(QWidget *parent = Q_NULLPTR);
	void setDeskopWnd(HWND h);

protected slots:
	void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
	void stop();
	void next();
	void prev();
	void flush();
	void close();
protected:
	void closeEvent(QCloseEvent *event);
private:
    Ui::XWallpaper2Class ui;
	HWND m_DesktopWnd = nullptr;
	QSystemTrayIcon *m_TrayIcon;
	QMenu *m_TrayMenu;
	bool m_CloseForExit = false;
	QWebEngineView *m_WebView = nullptr;
	QWidget *m_PaperWidget = nullptr;
};
