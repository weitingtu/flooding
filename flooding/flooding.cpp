#include "flooding.h"
#include "FloodingView.h"
#include "FloodingScene.h"
#include "FloorplanManager.h"
#include "type.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDockWidget>
#include <QPushButton>
#include <QIntValidator>

flooding::flooding(QWidget *parent)
	: QMainWindow(parent),
	_width_line_edit(nullptr),
	_height_line_edit(nullptr),
	_num_rect_line_edit(nullptr),
	_num_point_line_edit(nullptr),
	_generate_button(nullptr),
	_flooding_button(nullptr),
	_view(nullptr),
	_scene(nullptr)
{
	//ui.setupUi(this);
	_create_dock_widget();

	get_floorplan_manager().init();

	_view = new FloodingView(this);
	_scene = new FloodingScene(this);
	_scene->init();
	_view->setScene(_scene);
	setCentralWidget(_view);
}

void flooding::_create_dock_widget()
{
	QWidget* panel = new QWidget(this);
	QGridLayout* layout = new QGridLayout;
	int idx = 0;

	QLabel* width_label = new QLabel("Width: ");
	_width_line_edit = new QLineEdit;
	_width_line_edit->setValidator(new QIntValidator(0, 10000, this));
	_width_line_edit->setText(QString::number(default_width));
	layout->addWidget(width_label, idx, 0);
	layout->addWidget(_width_line_edit, idx, 1);
	++idx;

	QLabel* height_label = new QLabel("Height: ");
	_height_line_edit = new QLineEdit;
	_height_line_edit->setValidator(new QIntValidator(0, 10000, this));
	_height_line_edit->setText(QString::number(default_height));
	layout->addWidget(height_label, idx, 0);
	layout->addWidget(_height_line_edit, idx, 1);
	++idx;

	QLabel* num_rect_label = new QLabel("Obstacles: ");
	_num_rect_line_edit = new QLineEdit;
	_num_rect_line_edit->setValidator(new QIntValidator(0, 100, this));
	_num_rect_line_edit->setText(QString::number(default_num_rect));
	layout->addWidget(num_rect_label, idx, 0);
	layout->addWidget(_num_rect_line_edit, idx, 1);
	++idx;

	QLabel* num_point_label = new QLabel("Terminals: ");
	_num_point_line_edit = new QLineEdit;
	_num_point_line_edit->setValidator(new QIntValidator(0, 100, this));
	_num_point_line_edit->setText(QString::number(default_num_point));
	layout->addWidget(num_point_label, idx, 0);
	layout->addWidget(_num_point_line_edit, idx, 1);
	++idx;

	_generate_button = new QPushButton("Generate");
	layout->addWidget(_generate_button, idx, 0, 1, 2);
	connect(_generate_button, SIGNAL(released()), this, SLOT(_generate()));
	++idx;

	_flooding_button = new QPushButton("Flooding");
	layout->addWidget(_flooding_button, idx, 0, 1, 2);
	connect(_flooding_button, SIGNAL(released()), this, SLOT(_flooding()));
	++idx;

	QWidget* empty = new QWidget();
	empty->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	layout->addWidget(empty, idx, 0);
	panel->setLayout(layout);

	QDockWidget *dock = new QDockWidget(tr("Customers"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	dock->setWidget(panel);
	addDockWidget(Qt::RightDockWidgetArea, dock);
}

void flooding::_generate() const
{
	int width  = _width_line_edit->text().toInt();
	int height = _height_line_edit->text().toInt();
	int num_rect  = _num_rect_line_edit->text().toInt();
	int num_point = _num_point_line_edit->text().toInt();

	_scene->clear();
	get_floorplan_manager().generate(width, height, num_rect, num_point);
	//get_floorplan_manager().flooding();
	_scene->init();
}

void flooding::_flooding() const
{
	get_floorplan_manager().flooding();
}
