#pragma once

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
#else
	#include <QtWidgets/QWidget>
#endif

#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtCore/QVector>
#include <QtCore/QList>
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
			, trikControl::VectorSensorInterface::Type type
			, int maximum
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
	void drawWidgetName(QPainter &painter);
	void drawLegend(QPainter &painter);
	void drawChart(QPainter &painter, QVector<QPointF> &points, const QPen &pen);
	void drawAxis(QPainter &painter);
	void drawAxisXName(QPainter &painter);
	void setMatrix(QPainter &painter);
	void updateReadings(QVector<QPointF> &points, QPointF &newPoint);
	void markTimeAxis(QPainter &painter);
	void markVerticalAxis(QPainter &painter);
	void updateData();
	int getAverage(QList<int> &list);
	void reserveMemory();
	void clearNewData();
	qreal xCoordinate();
	qreal yCoordinate(int value);

	trikControl::VectorSensorInterface &mSensor;
	trikControl::VectorSensorInterface::Type mType;
	const int maxValue;
	const int mInterval = 500;
	QTimer mTimer;
	int mTime;
	const int maxTime = 10;
	const int axisMargin = 5;
	QVector<int> mData;
	QVector<QPointF> pointsX;
	QVector<QPointF> pointsY;
	QVector<QPointF> pointsZ;
	QList<int> newDataX;
	QList<int> newDataY;
	QList<int> newDataZ;
};

}
