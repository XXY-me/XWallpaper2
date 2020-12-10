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
	void pageLoadFinished(bool bok);
	void on_btnAddFile_clicked();
	void on_btnAddUrl_clicked();
protected:
	void closeEvent(QCloseEvent *event);
private:
	void loadPlayList();
	void savePlayList();
	void showPlayList();
private:
    Ui::XWallpaper2Class ui;
	HWND m_DesktopWnd = nullptr;
	QSystemTrayIcon *m_TrayIcon;
	QMenu *m_TrayMenu;
	bool m_CloseForExit = false;
	QWebEngineView *m_WebView = nullptr;
	QWidget *m_PaperWidget = nullptr;
	QList<QString> m_PlayList;
	int m_currentIndex = -1;
};
