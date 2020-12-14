#include "XPlayListItem.h"
#include <QFileInfo>
#include "XItemSetting.h"

XPlayListItem::XPlayListItem(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.btnStart, &QToolButton::clicked, this, &XPlayListItem::playItem);
	connect(ui.btnDelete, &QToolButton::clicked, this, &XPlayListItem::deleteItem);
}

XPlayListItem::~XPlayListItem()
{
}

void XPlayListItem::SetData(QString url)
{
	ui.btnSet->hide();
	QString ss = QString("QLabel{image:url(%1);}");
	if (url.startsWith("http", Qt::CaseInsensitive))
	{
		ui.labelIcon->setStyleSheet(ss.arg(":/XWallpaper2/res/URL.png"));
	}
	else if (url.endsWith("html", Qt::CaseInsensitive))
	{
		ui.labelIcon->setStyleSheet(ss.arg(":/XWallpaper2/res/html.png"));
		hasSetting(url);
	}
	else 
		ui.labelIcon->setStyleSheet(ss.arg(":/XWallpaper2/res/MP4.png"));
	ui.labelIcon->setFixedSize(30, 30);
	m_url = url;

}

void XPlayListItem::resizeEvent(QResizeEvent * re)
{
	__super::resizeEvent(re);
	//adjustSize();
	QFontMetrics fontWidth(ui.labelUrl->font());//得到每个字符的宽度
	QString elideNote = fontWidth.elidedText(m_url, Qt::ElideMiddle, ui.labelUrl->width());//最大宽度

	ui.labelUrl->setText(elideNote);//显示省略好的字符串
	ui.labelUrl->setToolTip(m_url);//设置tooltips
}

void XPlayListItem::showEvent(QShowEvent * se)
{
	__super::showEvent(se);
	//adjustSize();
	QFontMetrics fontWidth(ui.labelUrl->font());//得到每个字符的宽度
	QString elideNote = fontWidth.elidedText(m_url, Qt::ElideMiddle, ui.labelUrl->width());//最大宽度

	ui.labelUrl->setText(elideNote);//显示省略好的字符串
	ui.labelUrl->setToolTip(m_url);//设置tooltips
}

void XPlayListItem::hasSetting(QString file)
{
	QString setFile = file + ".json";
	if (QFileInfo(setFile).isFile())
	{
		ui.btnSet->show();
	}
}

void XPlayListItem::on_btnSet_clicked()
{
	QString configFile = m_url + ".json";
	XItemSetting dlg(configFile,this);
	dlg.exec();
}
