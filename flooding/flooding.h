#pragma once

#include <QtWidgets/QMainWindow>
//#include "ui_flooding.h"

class QLineEdit;
class QPushButton;
class QGraphicsView;
class FloodingScene;

class flooding : public QMainWindow
{
	Q_OBJECT

public:
	flooding(QWidget *parent = Q_NULLPTR);

private slots:
	void _generate() const;
	void _flooding() const;
private:
	void _create_dock_widget();

	QLineEdit* _width_line_edit;
	QLineEdit* _height_line_edit;
	QLineEdit* _num_rect_line_edit;
	QLineEdit* _num_point_line_edit;
	QPushButton* _generate_button;
	QPushButton* _flooding_button;
	QGraphicsView* _view;
	FloodingScene* _scene;
	//Ui::floodingClass ui;
};
