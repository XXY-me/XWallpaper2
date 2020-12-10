#include "XWallpaper2.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QCloseEvent>
//#include <QWindow>
#include <qt_windows.h>
#include <QLabel>
#include <QWebEngineSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include "XPlayListItem.h"

XWallpaper2::XWallpaper2(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	m_TrayMenu = new QMenu(this);

	m_TrayMenu->addAction(QString::fromLocal8Bit("ֹͣ"), this, &XWallpaper2::stop);
	m_TrayMenu->addAction(QString::fromLocal8Bit("��һ��"), this, &XWallpaper2::next);
	m_TrayMenu->addAction(QString::fromLocal8Bit("��һ��"), this, &XWallpaper2::prev);
	m_TrayMenu->addAction(QString::fromLocal8Bit("ˢ��"), this, &XWallpaper2::flush);
	m_TrayMenu->addAction(QString::fromLocal8Bit("��ʾ������"), this, &XWallpaper2::showNormal);
	m_TrayMenu->addAction(QString::fromLocal8Bit("�˳�"), this, &XWallpaper2::close);

	//��������ͼ��
	m_TrayIcon = new QSystemTrayIcon(this);
	m_TrayIcon->setIcon(QIcon(":/XWallpaper2/res/XXY.png"));
	m_TrayIcon->setToolTip(QString::fromLocal8Bit("˫����ʾ���ڣ��Ҽ���ʾ�˵���"));
	m_TrayIcon->setContextMenu(m_TrayMenu);
	m_TrayIcon->show();
	//m_TrayIcon->showMessage(QString("XWallpaper2"), QString("XWallpaper2 Started!"));
	connect(m_TrayIcon, &QSystemTrayIcon::activated, this, &XWallpaper2::onTrayIconActivated);

	//��ť����
	connect(ui.btnPrev, &QPushButton::clicked, this, &XWallpaper2::prev);
	connect(ui.btnNext, &QPushButton::clicked, this, &XWallpaper2::next);
	connect(ui.btnStart, &QPushButton::clicked, this, &XWallpaper2::stop);
}

void XWallpaper2::setDeskopWnd(HWND h)
{
	m_DesktopWnd = h;
	if (m_DesktopWnd != nullptr)
	{
		//��ȡ���ڴ�С
		RECT r;
		GetWindowRect(m_DesktopWnd,&r);
		//�ȱ���һ�����汳���ĸ���
		QPixmap pix = QPixmap::grabWindow((WId)m_DesktopWnd);
		//����һ������������������渱������ֹͣ���ŵ�ʱ����ʾ�˴���
		m_PaperWidget = new QWidget();
		m_PaperWidget->setWindowFlags(Qt::FramelessWindowHint);
		m_PaperWidget->setFixedSize({ r.right,r.bottom });
		m_PaperWidget->move(0, 0);
		QLabel * la = new QLabel(m_PaperWidget);
		la->setFixedSize({ r.right,r.bottom });
		la->move(0, 0);
		la->setPixmap(pix);
		la->show();
		SetParent((HWND)m_PaperWidget->winId(), m_DesktopWnd);

		//����WebView����������ʾ
		m_WebView = new QWebEngineView();
		connect(m_WebView, &QWebEngineView::loadFinished, this, &XWallpaper2::pageLoadFinished);
		m_WebView->setWindowFlags(Qt::FramelessWindowHint);
		SetParent((HWND)m_WebView->winId(), m_DesktopWnd);
		m_WebView->setFixedSize({r.right,r.bottom});
		m_WebView->move(0,0);
		//m_WebView->settings()->defaultSettings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls,true);
		//m_WebView->settings()->defaultSettings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls,true);
		//m_WebView->settings()->defaultSettings()->setAttribute(QWebEngineSettings::LocalStorageEnabled,true);
		//m_WebView->settings()->defaultSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);		
		//m_WebView->settings()->defaultSettings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
		//m_WebView->settings()->defaultSettings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
		//m_WebView->settings()->defaultSettings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);
		//m_WebView->settings()->defaultSettings()->setAttribute(QWebEngineSettings::SpatialNavigationEnabled, true);
		//m_WebView->settings()->defaultSettings()->setAttribute(QWebEngineSettings::LinksIncludedInFocusChain, true);
		//m_WebView->settings()->defaultSettings()->setAttribute(QWebEngineSettings::AutoLoadImages, true);

		//m_WebView->setUrl(QUrl::fromLocalFile(QString::fromLocal8Bit("D:/page/cloud1111/��css3�Ʋʶ���Ч��/index.html")));
		//m_WebView->show();
		next();
	}
}

void XWallpaper2::stop()
{
	if (m_WebView)
	{
		if (m_currentIndex == -1)
		{
			next();
		}
		else if (m_WebView->isHidden())
		{
			m_PaperWidget->hide();
			m_WebView->show();
			m_TrayMenu->actions().first()->setText(QString::fromLocal8Bit("ֹͣ"));
		}
		else
		{
			m_WebView->hide();
			m_PaperWidget->show();
			m_TrayMenu->actions().first()->setText(QString::fromLocal8Bit("��ʼ"));
		}
	}
}

void XWallpaper2::next()
{
	if (m_PlayList.isEmpty() || m_WebView == nullptr)
	{
		return;
	}
	m_currentIndex++;
	if (m_currentIndex >= m_PlayList.count())
	{
		m_currentIndex = 0;
	}
	QString str = m_PlayList[m_currentIndex];
	if (str.startsWith("http"))
	{
		m_WebView->load(QUrl(str));
	}
	else
	{
		if (str.endsWith("html"))
		{
			m_WebView->load(QUrl::fromLocalFile(str));
		}
	}
	m_WebView->show();
}

void XWallpaper2::prev()
{
	if (m_PlayList.isEmpty() || m_WebView == nullptr)
	{
		return;
	}
	m_currentIndex--;
	if (m_currentIndex < 0)
	{
		m_currentIndex = m_PlayList.count() - 1;
	}
	QString str = m_PlayList[m_currentIndex];
	if (str.startsWith("http"))
	{
		m_WebView->load(QUrl(str));
	}
	else
	{
		if (str.endsWith("html"))
		{
			m_WebView->load(QUrl::fromLocalFile(str));
		}
	}
	m_WebView->show();
}

void XWallpaper2::flush()
{
	m_WebView->reload();
}

void XWallpaper2::close()
{
	m_CloseForExit = true;
	__super::close();
}

void XWallpaper2::pageLoadFinished(bool bok)
{
	if (!bok)
	{
		next();
	}
}

void XWallpaper2::on_btnAddFile_clicked()
{
	QString str = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("ѡ���ļ�"),"",QString("Html File (*.html)"));
	if (str.isEmpty())
	{
		return;
	}
	if (m_PlayList.contains(str))
	{
		QMessageBox::information(this, QString("XWallpaper2"), QString::fromLocal8Bit("�б����Ѵ��ڸ��ļ���"));
		return;
	}
	m_PlayList.append(str);
}

void XWallpaper2::on_btnAddUrl_clicked()
{
	QString str = ui.lineEditURL->text();
	if (str.isEmpty() )
	{
		return;
	}
	if (m_PlayList.contains(str))
	{
		QMessageBox::information(this, QString("XWallpaper2"), QString::fromLocal8Bit("�б����Ѵ��ڸ��ļ���"));
		return;
	}
	m_PlayList.append(str);
}

void XWallpaper2::closeEvent(QCloseEvent * event)
{
	if (!m_CloseForExit)
	{
		hide();
		event->ignore();
	}
	else
	{
		if (m_WebView)
		{
			m_WebView->close();
			m_PaperWidget->show();
			qApp->processEvents();
			Sleep(100);
			m_PaperWidget->close();
		}
	}
}

void XWallpaper2::loadPlayList()
{
	m_PlayList.clear();
	QFile f("./list.xd");
	if (f.open(QFile::ReadOnly))
	{
		QString str = f.readLine();
		while (!str.isEmpty())
		{
			m_PlayList.append(str);
			str = f.readLine();
		}
		f.close();
	}
}

void XWallpaper2::savePlayList()
{
	QFile f("./list.xd");
	if (f.open(QFile::WriteOnly))
	{
		for each (auto var in m_PlayList)
		{
			f.write(var.toUtf8());
			f.write("\r\n");
		}
		f.close();
	}
}

void XWallpaper2::showPlayList()
{

}

void XWallpaper2::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason)
	{
	case QSystemTrayIcon::Unknown:
		break;
	case QSystemTrayIcon::Context:
		break;
	case QSystemTrayIcon::DoubleClick:
		showNormal();
		break;
	case QSystemTrayIcon::Trigger:
		break;
	case QSystemTrayIcon::MiddleClick:
		break;
	default:
		break;
	}
}
