#pragma once

#include <QDialog>
#include "ui_XItemSetting.h"

enum ConfigType
{
	CT_unknow,
	CT_input,
	CT_combo,
	//CT_openFile,
	//CT_openFolder,
	CT_switch,
	CT_color
};

struct ConfigItem
{
	ConfigType type = CT_unknow;
	QString name;
	QString des;
	//QString value;
	QString defaultValue;
	QStringList valueList;
	QWidget* ctl;
};

class XItemSetting : public QDialog
{
	Q_OBJECT

public:
	XItemSetting(QString configFile,QWidget *parent = Q_NULLPTR);
	~XItemSetting();
protected slots:
	void on_btnOK_clicked();
	void on_btnReset_clicked();
	void on_btnCancle_clicked();
private:
	void InitUi();
private:
	Ui::XItemSetting ui;
	QString m_configFile;
	QList<ConfigItem> m_ConfigList;
};
