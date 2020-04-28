#include "FloodingView.h"
#include <qevent.h>

FloodingView::FloodingView(QWidget* parent ) : QGraphicsView(parent) 
{
	setRenderHint(QPainter::Antialiasing);
}

const double zoom_in_scale = 2.0;

void FloodingView::wheelEvent(QWheelEvent* e)
{
	if (e->delta() > 0)
	{
		zoom_in();
	}
	else if (e->delta() < 0)
	{
		zoom_out();
	}
	else
	{
		return QGraphicsView::wheelEvent(e);
	}
	e->accept();
}

void FloodingView::zoom_in()
{
	scale(zoom_in_scale, zoom_in_scale);
}

void FloodingView::zoom_out()
{
	scale(1.0 / zoom_in_scale, 1.0 / zoom_in_scale);
}
