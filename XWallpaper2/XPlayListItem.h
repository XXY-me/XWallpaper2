#pragma once

#include <QWidget>
#include "ui_XPlayListItem.h"

class XPlayListItem : public QWidget
{
	Q_OBJECT

public:
	XPlayListItem(QWidget *parent = Q_NULLPTR);
	~XPlayListItem();

private:
	Ui::XPlayListItem ui;
};
