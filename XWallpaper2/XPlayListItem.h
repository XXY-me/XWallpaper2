#pragma once

#include <QWidget>
#include "ui_XPlayListItem.h"


class XPlayListItem : public QWidget
{
	Q_OBJECT

public:
	XPlayListItem(QWidget *parent = Q_NULLPTR);
	~XPlayListItem();
	void SetData(QString url);
signals:
	void playItem();
	void deleteItem();
	void configChanged();
protected slots:
	void on_btnSet_clicked();
protected:
	void resizeEvent(QResizeEvent* re);
	void showEvent(QShowEvent*se);
private:
	void hasSetting(QString file);
private:
	Ui::XPlayListItem ui;
	QString m_url;
};
