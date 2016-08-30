/* Copyright 2016 QReal Research Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QLabel>
#endif

#include <QTWidgets>
#include <QVector>
#include <QTimer>
#include <QPen>
#include <QtGlobal>

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
	VectorSensorWidget(trikControl::BrickInterface &brick
	, trikControl::VectorSensorInterface &sensor
	, QWidget *parent = 0);

	/// String that shall appear in menu for this widget.
	static QString menuEntry(trikControl::VectorSensorInterface::Type type);

public slots:
	void renew();

protected:
	void paintEvent(QPaintEvent *);
	void exit();
	void renewFocus();

private:
	void delay(int millisecondsToWait);
	void drawDiagram(QPainter &painter, QVector<QPointF> points, QPen pen);
	void drawAxis(QPainter &painter);
	void drawAxisXName(QPainter & painter);
	void setMatrix(QPainter &painter);
	void updateReadings(QVector<QPointF> &points, QPointF newPoint);
	void markTimeAxis(QPainter &painter);
	qreal xCoordinate();
	qreal yCoordinate(int value);

	trikControl::BrickInterface &mBrick;
	trikControl::VectorSensorInterface &mSensor;
	const int mInterval;
	QTimer mTimer;
	int mTime;
	const int maxTime;
	const int axisMargin;
	const int maxValue;
	QVector<int> mData;
	QVector<QPointF> pointsX;
	QVector<QPointF> pointsY;
	QVector<QPointF> pointsZ;
};

}
