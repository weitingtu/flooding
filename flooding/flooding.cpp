#include "flooding.h"
#include "FloodingView.h"
#include "FloodingScene.h"
#include "FloorplanManager.h"
#include "DisplayManager.h"
#include "type.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDockWidget>
#include <QPushButton>
#include <QIntValidator>
#include <QGroupBox>
#include <QRadioButton>
#include <QSpinBox>

flooding::flooding(QWidget *parent)
	: QMainWindow(parent),
	_idv_flooding_radio(nullptr),
	_total_flooding_radio(nullptr),
	_idv_pred_radio(nullptr),
	_total_pred_radio(nullptr),
	_flooding_spin_box(nullptr),
	_pred_spin_box(nullptr),
	_width_line_edit(nullptr),
	_height_line_edit(nullptr),
	_num_rect_line_edit(nullptr),
	_num_point_line_edit(nullptr),
	_generate_button(nullptr),
	_hanan_grid_button(nullptr),
	_flooding_button(nullptr),
	_backtracking_button(nullptr),
	_idv_completed_steiner_tree_button(nullptr),
	_completed_steiner_tree_button(nullptr),
	_total_dis_label(nullptr),
	_view(nullptr),
	_scene(nullptr)
{
	//ui.setupUi(this);
	_create_dock_widget();

	get_floorplan_manager().init();
	_total_dis_label->setText(QString("Total distance : %1").arg(get_floorplan_manager().get_total_pred_dis()));

    DisplayManager& dm = get_display_manager();
	dm.set_source_termianl_only(true);

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

	layout->addWidget(_create_exclusive_group(), idx, 0, 1, 2);
	++idx;

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

	_hanan_grid_button = new QPushButton("Hana grid");
	layout->addWidget(_hanan_grid_button, idx, 0, 1, 2);
	connect(_hanan_grid_button, SIGNAL(released()), this, SLOT(_hanan_grid()));
	++idx;

	_flooding_button = new QPushButton("Flooding");
	layout->addWidget(_flooding_button, idx, 0, 1, 2);
	connect(_flooding_button, SIGNAL(released()), this, SLOT(_flooding()));
	++idx;

	_backtracking_button = new QPushButton("Backtracking");
	layout->addWidget(_backtracking_button, idx, 0, 1, 2);
	connect(_backtracking_button, SIGNAL(released()), this, SLOT(_backtracking()));
	++idx;

	_idv_completed_steiner_tree_button = new QPushButton("Completed steiner tree step by step");
	layout->addWidget(_idv_completed_steiner_tree_button, idx, 0, 1, 2);
	connect(_idv_completed_steiner_tree_button, SIGNAL(released()), this, SLOT(_idv_complete_steiner_tree()));
	++idx;

	_completed_steiner_tree_button = new QPushButton("Completed steiner tree");
	layout->addWidget(_completed_steiner_tree_button, idx, 0, 1, 2);
	connect(_completed_steiner_tree_button, SIGNAL(released()), this, SLOT(_complete_steiner_tree()));
	++idx;

	_total_dis_label = new QLabel("Total distance : 0");
	layout->addWidget(_total_dis_label, idx, 0, 1, 2);
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

QGroupBox* flooding::_create_exclusive_group()
{
	QGroupBox *groupBox = new QGroupBox(tr("Display:"));
	_idv_flooding_radio = new QRadioButton(tr("&Individual flooding"));
	_total_flooding_radio = new QRadioButton(tr("&Total flooding"));
	_idv_pred_radio = new QRadioButton(tr("&Individual backtracking"));
	_total_pred_radio = new QRadioButton(tr("&Total backtracking"));

	_total_flooding_radio->setChecked(true);
	get_display_manager().set_total_flooding(true);

	connect(_idv_flooding_radio,   SIGNAL(toggled(bool)), this, SLOT(_set_display()));
	connect(_total_flooding_radio, SIGNAL(toggled(bool)), this, SLOT(_set_display()));
	connect(_idv_pred_radio,       SIGNAL(toggled(bool)), this, SLOT(_set_display()));
	connect(_total_pred_radio,     SIGNAL(toggled(bool)), this, SLOT(_set_display()));

	_flooding_spin_box = new QSpinBox(this);
	_flooding_spin_box->setMinimum(0);
	_flooding_spin_box->setMaximum(default_num_point - 1);

	connect(_flooding_spin_box, SIGNAL(valueChanged(int)), this, SLOT(_set_display()));

	_pred_spin_box = new QSpinBox(this);
	_pred_spin_box->setMinimum(0);
	_pred_spin_box->setMaximum(default_num_point - 1);

	connect(_pred_spin_box, SIGNAL(valueChanged(int)), this, SLOT(_set_display()));

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(_idv_flooding_radio);
	vbox->addWidget(_flooding_spin_box);
	vbox->addWidget(_total_flooding_radio);
	vbox->addWidget(_idv_pred_radio);
	vbox->addWidget(_pred_spin_box);
	vbox->addWidget(_total_pred_radio);
	groupBox->setLayout(vbox);

	return groupBox;
}

void flooding::_generate() const
{
    DisplayManager& dm = get_display_manager();
	dm.set_source_termianl_only(false);

	int width  = _width_line_edit->text().toInt();
	int height = _height_line_edit->text().toInt();
	int num_rect  = _num_rect_line_edit->text().toInt();
	int num_point = _num_point_line_edit->text().toInt();

	_flooding_spin_box->setMaximum(num_point - 1);
	_pred_spin_box->setMaximum(num_point - 1);

	_scene->clear();
	get_floorplan_manager().generate(width, height, num_rect, num_point);
	//_scene->init();
	_scene->init_point_rect();
	_total_dis_label->setText(QString("Total distance : 0"));
}

void flooding::_hanan_grid() const
{
	_scene->clear();
	_scene->init();
}

void flooding::_flooding() const
{
	_total_flooding_radio->setChecked(true);
	_scene->clear();
	get_floorplan_manager().flooding();
	_scene->init();
}

void flooding::_backtracking() const
{
	_total_pred_radio->setChecked(true);
	_scene->clear();
	get_floorplan_manager().backtrack();
	_scene->init();
}

void flooding::_idv_complete_steiner_tree() const
{
    DisplayManager& dm = get_display_manager();
	dm.set_source_termianl_only(true);

	_scene->clear();
	get_floorplan_manager().idv_complete_steiner_tree();
	_scene->init();
	_total_dis_label->setText(QString("Total distance : %1").arg(get_floorplan_manager().get_total_pred_dis()));
}

void flooding::_complete_steiner_tree() const
{
    DisplayManager& dm = get_display_manager();
	dm.set_source_termianl_only(true);

	_total_pred_radio->setChecked(true);
	_scene->clear();
	get_floorplan_manager().complete_steiner_tree();
	_scene->init();
	_total_dis_label->setText(QString("Total distance : %1").arg(get_floorplan_manager().get_total_pred_dis()));
}

void flooding::_set_display() const
{
    DisplayManager& dm = get_display_manager();
	dm.set_indivisual_flooding(_idv_flooding_radio->isChecked());
	dm.set_indivisual_flooding_idx(_flooding_spin_box->value());
	dm.set_total_flooding(_total_flooding_radio->isChecked());
	dm.set_indivisual_pred(_idv_pred_radio->isChecked());
	dm.set_indivisual_pred_idx(_pred_spin_box->value());
	dm.set_total_pred(_total_pred_radio->isChecked());
	_scene->update();
}

