#pragma once

#include <QtWidgets/QMainWindow>
//#include "ui_flooding.h"

class QLineEdit;
class QPushButton;
class QGraphicsView;
class FloodingScene;
class QGroupBox;
class QRadioButton;
class QSpinBox;

class flooding : public QMainWindow
{
	Q_OBJECT

public:
	flooding(QWidget *parent = Q_NULLPTR);

private slots:
	void _generate() const;
	void _hanan_grid() const;
	void _flooding() const;
	void _idv_backtracking() const;
	void _backtracking() const;
	void _set_display() const;

private:
	void _create_dock_widget();
	QGroupBox* _create_exclusive_group();

	QRadioButton *_idv_flooding_radio;
	QRadioButton *_total_flooding_radio;
	QRadioButton *_idv_pred_radio;
	QRadioButton *_total_pred_radio;
	QSpinBox*  _flooding_spin_box;
	QSpinBox*  _pred_spin_box;
	QLineEdit* _width_line_edit;
	QLineEdit* _height_line_edit;
	QLineEdit* _num_rect_line_edit;
	QLineEdit* _num_point_line_edit;
	QPushButton* _generate_button;
	QPushButton* _hanan_grid_button;
	QPushButton* _flooding_button;
	QPushButton* _idv_backtracking_button;
	QPushButton* _backtracking_button;
	QGraphicsView* _view;
	FloodingScene* _scene;
	//Ui::floodingClass ui;
};
