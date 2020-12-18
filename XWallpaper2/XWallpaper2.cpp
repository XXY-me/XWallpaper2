#include "XWallpaper2.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <qt_windows.h>
#include <QLabel>
#include <QWebEngineSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QMenu>
#include "config.h"
#include <QWebChannel>

XWallpaper2::XWallpaper2(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	m_TrayMenu = new QMenu(this);

	m_TrayMenu->addAction(QIcon(":/XWallpaper2/res/start.png"), QString("开始"), this, &XWallpaper2::stop);
	m_TrayMenu->addAction(QIcon(":/XWallpaper2/res/next.png"), QString("下一个"), this, &XWallpaper2::next);
	m_TrayMenu->addAction(QIcon(":/XWallpaper2/res/prev.png"), QString("上一个"), this, &XWallpaper2::prev);
	m_TrayMenu->addAction(QIcon(":/XWallpaper2/res/flush.png"), QString("刷新"), this, &XWallpaper2::flush);
	m_TrayMenu->addAction(QString("显示主窗口"), this, &XWallpaper2::showNormal);
	m_TrayMenu->addAction(QString("退出"), this, &XWallpaper2::close);

	//创建托盘图标
	m_TrayIcon = new QSystemTrayIcon(this);
	m_TrayIcon->setIcon(QIcon(":/XWallpaper2/res/XXY.png"));
	m_TrayIcon->setToolTip(QString("双击显示窗口，右键显示菜单。"));
	m_TrayIcon->setContextMenu(m_TrayMenu);
	m_TrayIcon->show();
	//m_TrayIcon->showMessage(QString("XWallpaper2"), QString("XWallpaper2 Started!"));
	connect(m_TrayIcon, &QSystemTrayIcon::activated, this, &XWallpaper2::onTrayIconActivated);

	//按钮操作
	connect(ui.btnPrev, &QToolButton::clicked, this, &XWallpaper2::prev);
	connect(ui.btnNext, &QToolButton::clicked, this, &XWallpaper2::next);
	connect(ui.btnStart, &QToolButton::clicked, this, &XWallpaper2::stop);
	connect(ui.btnFlush, &QToolButton::clicked, this, &XWallpaper2::flush);
	loadPlayList();
	startTimer(3000);
	//种个种子
	srand(QDateTime::currentDateTime().toMSecsSinceEpoch());
}

void XWallpaper2::setDeskopWnd(HWND h)
{
	m_DesktopWnd = h;
	if (m_DesktopWnd != nullptr)
	{
		//获取窗口大小
		RECT r;
		GetWindowRect(m_DesktopWnd, &r);
		//先保存一份桌面背景的副本
		QPixmap pix = QPixmap::grabWindow((WId)m_DesktopWnd);
		//创建一个窗口来保存这个桌面副本，当停止播放的时候显示此窗口
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

		//创建WebView窗口用来显示
		m_WebView = new QWebEngineView();
		connect(m_WebView, &QWebEngineView::loadFinished, this, &XWallpaper2::pageLoadFinished);
		m_WebView->setWindowFlags(Qt::FramelessWindowHint);
		SetParent((HWND)m_WebView->winId(), m_DesktopWnd);
		m_WebView->setFixedSize({ r.right,r.bottom });
		m_WebView->move(0, 0);
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
			m_TrayMenu->actions().first()->setText(QString("停止"));
			m_TrayMenu->actions().first()->setIcon(QIcon(":/XWallpaper2/res/stop.png"));
			ui.btnStart->setIcon(QIcon(":/XWallpaper2/res/stop.png"));
		}
		else
		{
			m_WebView->hide();
			m_PaperWidget->show();
			m_TrayMenu->actions().first()->setText(QString("开始"));
			m_TrayMenu->actions().first()->setIcon(QIcon(":/XWallpaper2/res/start.png"));
			ui.btnStart->setIcon(QIcon(":/XWallpaper2/res/start.png"));
		}
	}
}

void XWallpaper2::next()
{
	if (m_PlayList.isEmpty() || m_WebView == nullptr)
	{
		if (m_PlayList.isEmpty())
		{
			m_WebView->hide();
			m_TrayMenu->actions().first()->setText(QString("开始"));
			m_TrayMenu->actions().first()->setIcon(QIcon(":/XWallpaper2/res/start.png"));
			ui.btnStart->setIcon(QIcon(":/XWallpaper2/res/start.png"));
		}
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
	m_LastChangeTime = QTime::currentTime();
	m_WebView->show();
	m_TrayMenu->actions().first()->setText(QString("停止"));
	m_TrayMenu->actions().first()->setIcon(QIcon(":/XWallpaper2/res/stop.png"));
	ui.btnStart->setIcon(QIcon(":/XWallpaper2/res/stop.png"));
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
	else
	{
		//检查是否有设置json
		QString url = m_WebView->url().toLocalFile();
		if (!url.isEmpty())
		{
			QString configUrl = url + ITEM_CONF_SUFFIX;
			QFile f(configUrl);
			if (!f.exists())
			{
				return;
			}
			if (f.open(QFile::ReadOnly))
			{
				QString configCon;
				configCon = f.readAll();
				f.close();
				configCon.remove(QRegExp("\\s"));
				configCon.insert(0, "window.XCONFIG = '");			//设置全局变量
				configCon.append("';XInit();");							//调用初始化函数
				m_WebView->page()->runJavaScript(configCon);
			}
		}
	}
}

void XWallpaper2::on_btnAddFile_clicked()
{
	QString str = QFileDialog::getOpenFileName(this, QString("选择文件"), "", QString("Html File (*.html)"));
	if (str.isEmpty())
	{
		return;
	}
	if (m_PlayList.contains(str))
	{
		QMessageBox::information(this, QString("XWallpaper2"), QString("列表中已存在该文件！"));
		return;
	}
	m_PlayList.append(str);
	showPlayList();
	savePlayList();
}

void XWallpaper2::on_btnAddUrl_clicked()
{
	QString str = ui.lineEditURL->text();
	if (str.isEmpty())
	{
		return;
	}
	if (m_PlayList.contains(str))
	{
		QMessageBox::information(this, QString("XWallpaper2"), QString("列表中已存在该文件！"));
		return;
	}
	ui.lineEditURL->setText("");
	m_PlayList.append(str);
	showPlayList();
	savePlayList();
}

void XWallpaper2::on_PlayItem()
{
	int index = m_ListItems.indexOf((XPlayListItem*)sender());
	m_currentIndex = index - 1;
	next();
}

void XWallpaper2::on_DeleteItem()
{
	int index = m_ListItems.indexOf((XPlayListItem*)sender());
	m_PlayList.removeAt(index);
	QBoxLayout* layout = (QBoxLayout*)ui.scrollAreaWidgetContents->layout();
	layout->removeWidget(m_ListItems[index]);
	delete m_ListItems[index];
	m_ListItems.removeAt(index);
	if (m_currentIndex == index)
	{
		m_currentIndex -= 1;
		next();
	}
	savePlayList();
}

void XWallpaper2::on_ItemConfigChanged()
{
	int index = m_ListItems.indexOf((XPlayListItem*)sender());
	if (index == m_currentIndex)
	{
		flush();
	}
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

void XWallpaper2::showEvent(QShowEvent * se)
{
	showPlayList();
}

void XWallpaper2::timerEvent(QTimerEvent* te)
{
	if (ui.comboBoxLoopType->currentIndex() == 0)
	{
		return;
	}
	int ns = 0;			//间隔秒数
	switch (ui.comboBoxChangeTime->currentIndex())
	{
	case 0:				//1分钟
		ns = 60;
		break;
	case 1:				//3分钟
		ns = 180;
		break;
	case 2:				//10分钟
		ns = 600;
		break;
	case 3:				//30分钟
		ns = 1800;
		break;
	case 4:				//1小时
		ns = 3600;
		break;
	case 5:				//6小时
		ns = 21600;
		break;
	default:
		return;
		break;
	}
	if (m_LastChangeTime.secsTo(QTime::currentTime()) > ns)
	{
		if (ui.comboBoxLoopType->currentIndex() == 2)
		{
			m_currentIndex = rand() % m_PlayList.count() - 1;			//取随机位置
		}
		next();
	}
}

void XWallpaper2::loadPlayList()
{
	m_PlayList.clear();
	QFile f("./list.xd");
	if (f.open(QFile::ReadOnly))
	{
		QString str = QString::fromUtf8(f.readLine()).trimmed();

		while (!str.isEmpty())
		{
			m_PlayList.append(str);
			str = QString::fromUtf8(f.readLine()).trimmed();
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
	QBoxLayout* layout = (QBoxLayout*)ui.scrollAreaWidgetContents->layout();
	for each (auto var in m_ListItems)
	{
		layout->removeWidget((QWidget*)var);
		disconnect(var);
		delete var;
	}
	m_ListItems.clear();
	delete layout->takeAt(0);

	for each (auto var in m_PlayList)
	{
		XPlayListItem* item = new XPlayListItem(ui.scrollAreaWidgetContents);
		item->SetData(var);
		layout->addWidget(item);
		m_ListItems.append(item);
		connect(item, &XPlayListItem::playItem, this, &XWallpaper2::on_PlayItem);
		connect(item, &XPlayListItem::deleteItem, this, &XWallpaper2::on_DeleteItem);
		connect(item, &XPlayListItem::configChanged, this, &XWallpaper2::on_ItemConfigChanged);
	}
	//创建个弹簧把内容顶一下
	layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
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
