#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_XWallpaper2.h"

class XWallpaper2 : public QMainWindow
{
    Q_OBJECT

public:
    XWallpaper2(QWidget *parent = Q_NULLPTR);

private:
    Ui::XWallpaper2Class ui;
};
