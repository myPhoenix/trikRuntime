#pragma once

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
#else
	#include <QtWidgets/QWidget>
#endif

#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtCore/QVector>
#include <QtCore/QTimer>

#include <trikControl/vectorSensorInterface.h>
#include "trikGuiDialog.h"

namespace trikControl {
class BrickInterface;
class VectorSensorInterface;
}

namespace trikGui {

class VectorSensorWidget : public TrikGuiDialog
{
	Q_OBJECT

public:
	/// Constructor.
	VectorSensorWidget(trikControl::VectorSensorInterface &sensor
	, QWidget *parent = 0);

	/// String that shall appear in menu for this widget.
	static QString menuEntry(trikControl::VectorSensorInterface::Type type);

	int exec() override;

	void renewFocus() override;

public slots:
	void renew();

protected:
	void paintEvent(QPaintEvent *);
	void exit() override;

private:
	void drawChart(QPainter &painter, QVector<QPointF> &points, const QPen &pen);
	void drawAxis(QPainter &painter);
	void drawAxisXName(QPainter &painter);
	void setMatrix(QPainter &painter);
	void updateReadings(QVector<QPointF> &points, QPointF &newPoint);
	void markTimeAxis(QPainter &painter);
	qreal xCoordinate();
	qreal yCoordinate(int value);

	trikControl::VectorSensorInterface &mSensor;
	const int mInterval = 500;
	QTimer mTimer;
	int mTime;
	const int maxTime = 10;
	const int axisMargin = 5;
	const int maxValue = 6000;
	QVector<int> mData;
	QVector<QPointF> pointsX;
	QVector<QPointF> pointsY;
	QVector<QPointF> pointsZ;
};

}
