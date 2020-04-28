#pragma once
//#include <QtWidgets/QGraphicsView>
#include <qgraphicsview.h>

class FloodingView :
	public QGraphicsView
{
public:
	FloodingView(QWidget* parent = nullptr);
protected:
	virtual void wheelEvent(QWheelEvent* e) override;
private:
	void zoom_in();
	void zoom_out();
};

