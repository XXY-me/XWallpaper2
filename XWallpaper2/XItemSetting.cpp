#include "XItemSetting.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QFileDialog>

XItemSetting::XItemSetting(QString configFile,QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_configFile = configFile;

	InitUi();
}

XItemSetting::~XItemSetting()
{
}

void XItemSetting::on_btnOK_clicked()
{
	//保存json文件
	QJsonArray ja;
	for each (auto var in m_ConfigList)
	{
		QJsonObject obj;
		obj["name"] = var.name;
		obj["des"] = var.des;
		obj["default"] = var.defaultValue;
		switch (var.type)
		{
		case CT_input:
		{
			obj["value"] = ((QLineEdit*)var.ctl)->text();
			obj["type"] = "input";
		}
		break;
		case CT_combo:
		{
			obj["value"] = ((QComboBox*)var.ctl)->currentText();
			QJsonArray ja2;
			for each (auto var2 in var.valueList)
			{
				ja2.append(var2);
			}
			obj["valueList"] = ja2;
			obj["type"] = "combo";
		}
		break;
		case CT_openFile:
		{
			obj["value"] = ((QLineEdit*)var.ctl)->text();
			obj["type"] = "openFile";
		}
		break;
		case CT_openFolder:
		{
			obj["value"] = ((QLineEdit*)var.ctl)->text();
			obj["type"] = "openFolder";
		}
		break;
		case CT_switch:
		{
			obj["value"] = ((QCheckBox*)var.ctl)->isChecked()?"true":"false";
			obj["type"] = "switch";
		}
		break;
		default:
			break;
		}
		ja.append(obj);
	}
	QFile f(m_configFile);
	if (f.open(QFile::WriteOnly))
	{
		QJsonDocument doc(ja);
		f.write(doc.toJson());
		f.close();
	}
	done(1);
}

void XItemSetting::on_btnReset_clicked()
{
	for each (auto var in m_ConfigList)
	{
		switch (var.type)
		{
		case CT_input:
		{
			((QLineEdit*)var.ctl)->setText(var.defaultValue);
		}
		break;
		case CT_combo:
		{
			((QComboBox*)var.ctl)->setCurrentText(var.defaultValue);
		}
		break;
		case CT_openFile:
		{
			((QLineEdit*)var.ctl)->setText(var.defaultValue);
		}
		break;
		case CT_openFolder:
		{
			((QLineEdit*)var.ctl)->setText(var.defaultValue);
		}
		break;
		case CT_switch:
		{
			if (var.defaultValue =="true")
			{
				((QCheckBox*)var.ctl)->setChecked(true);
			}
			else
				((QCheckBox*)var.ctl)->setChecked(false);
			
		}
		break;
		default:
			break;
		}
	}
}

void XItemSetting::on_btnCancle_clicked()
{
	done(0);
}

void XItemSetting::InitUi()
{
	QVBoxLayout *layout = new QVBoxLayout(ui.scrollAreaWidgetContents);
	ui.scrollAreaWidgetContents->setLayout(layout);
	//读取设置信息
	QFile f(m_configFile);
	if (f.open(QFile::ReadOnly))
	{
		QByteArray ba = f.readAll();
		QJsonDocument doc;
		QJsonParseError err;
		doc = QJsonDocument::fromJson(ba, &err);
		if (err.error == QJsonParseError::NoError && doc.isArray())
		{
			QJsonArray ar = doc.array();
			for each (auto var in ar)
			{
				QJsonObject obj = var.toObject();
				ConfigItem item;
				item.name = obj["name"].toString();
				item.des = obj["des"].toString();
				item.defaultValue = obj["default"].toString();
				//item.value = obj["value"].toString();
				QString type = obj["type"].toString();
				
				if (type == "input")
				{
					item.type = CT_input;
				}
				else if (type == "combo")
				{
					item.type = CT_combo;
				}
				else if (type == "openFile")
				{
					item.type = CT_openFile;
				}
				else if (type == "openFolder")
				{
					item.type = CT_openFolder;
				}
				else if (type =="switch")
				{
					item.type = CT_switch;
				}
				if (item.type == CT_unknow || item.name.isEmpty())
				{
					continue;
				}
				//创建控件
				QGroupBox* grpName = new QGroupBox(ui.scrollAreaWidgetContents);
				grpName->setTitle(item.name);
				grpName->setStyleSheet("QGroupBox{font-size:20px;font-weight:bold;}");
				layout->addWidget(grpName);

				QVBoxLayout *layout2 = new QVBoxLayout(grpName);
				grpName->setLayout(layout2);

				QLabel* labDes = new QLabel(grpName);
				labDes->setWordWrap(true);
				labDes->setText(item.des);
				layout2->addWidget(labDes);

				switch (item.type)
				{
				case CT_input:
				{
					QLineEdit* le = new QLineEdit(grpName);
					le->setText(obj["value"].toString());
					layout2->addWidget(le);
					item.ctl = le;
				}
					break;
				case CT_combo:
				{
					QComboBox* cb = new QComboBox(grpName);
					for each (auto var in obj["valueList"].toArray())
					{
						cb->addItem(var.toString());
						item.valueList.append(var.toString());
					}
					cb->setCurrentText(obj["value"].toString());
					layout2->addWidget(cb);
					item.ctl = cb;
				}
					break;
				case CT_openFile:
				{
					QHBoxLayout *layout3 = new QHBoxLayout(grpName);
					layout2->addLayout(layout3);
					QLineEdit *lab = new QLineEdit(grpName);
					lab->setText(obj["value"].toString());
					//lab->setStyleSheet("QLabel{border:1px solid #c0c0c0;}");
					layout3->addWidget(lab);
					QPushButton* btn = new QPushButton(grpName);
					btn->setText("选择文件");
					btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
					layout3->addWidget(btn);
					connect(btn, &QPushButton::clicked, [lab, this]() {
						QString str = QFileDialog::getOpenFileName(this, "选择文件");
						if (!str.isEmpty())
						{
							lab->setText(str);
						}
					});
					item.ctl = lab;
				}
					break;
				case CT_openFolder:
				{
					QHBoxLayout *layout3 = new QHBoxLayout(grpName);
					layout2->addLayout(layout3);
					QLineEdit *lab = new QLineEdit(grpName);
					lab->setText(obj["value"].toString());
					//lab->setStyleSheet("QLabel{border:1px solid #c0c0c0;}");
					layout3->addWidget(lab);
					QPushButton* btn = new QPushButton(grpName);
					btn->setText("选择文件夹");
					btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
					layout3->addWidget(btn);
					connect(btn, &QPushButton::clicked, [lab,this]() {
						QString str = QFileDialog::getExistingDirectory(this,"选择文件夹");
						if (!str.isEmpty())
						{
							lab->setText(str);
						}
					});
					item.ctl = lab;
				}
					break;
				case CT_switch:
				{
					QCheckBox* cb = new QCheckBox(grpName);
					cb->setText(item.name);
					if (obj["value"].toString() == "true")
					{
						cb->setChecked(true);
					}
					else
					{
						cb->setChecked(false);
					}
					layout2->addWidget(cb);
					item.ctl = cb;
				}
					break;
				default:
					break;
				}
				m_ConfigList.append(item);
			}
		}
		f.close();
	}
	//创建个弹簧把内容顶一下
	layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
}
