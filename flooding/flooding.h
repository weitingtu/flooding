#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_flooding.h"

class flooding : public QMainWindow
{
	Q_OBJECT

public:
	flooding(QWidget *parent = Q_NULLPTR);

private:
	Ui::floodingClass ui;
};
