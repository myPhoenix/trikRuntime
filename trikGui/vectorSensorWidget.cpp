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

#include "vectorSensorWidget.h"

#include <trikControl/brickInterface.h>
#include <trikControl/vectorSensorInterface.h>
#include <QTime>

using namespace trikGui;

VectorSensorWidget::VectorSensorWidget(trikControl::BrickInterface &brick
	, trikControl::VectorSensorInterface &sensor
	, QWidget *parent)
	: TrikGuiDialog(parent)
	, mBrick(brick)
	, mSensor(sensor)
	, mInterval(500)
	, mTime(0)
	, maxTime(10)
	, axisMargin(5)
	, maxValue(6000)
{
	mData << 0 << 0 << 0;

	mTimer.setInterval(mInterval);
	connect(&mTimer, SIGNAL(timeout()), this, SLOT(update()));
	connect(&mSensor, SIGNAL(newData(QVector<int>, trikKernel::TimeVal)), this, SLOT(renew()));
}


QString VectorSensorWidget::menuEntry(trikControl::VectorSensorInterface::Type type)
{
	switch (type) {
		case trikControl::VectorSensorInterface::Type::accelerometer: {
			return tr("Accelerometer");
		}
		case trikControl::VectorSensorInterface::Type::gyroscope: {
			return tr("Gyroscope");
		}
	}

	return QString();
}

void VectorSensorWidget::paintEvent(QPaintEvent *)
{
	QPen bluePen(Qt::blue, 2, Qt::SolidLine);
	QPen redPen(Qt::red, 2, Qt::SolidLine);
	QPen greenPen(Qt::green, 2, Qt::SolidLine);

	updateReadings(pointsX, QPointF(xCoordinate(), yCoordinate(mData[0])));
	updateReadings(pointsY, QPointF(xCoordinate(), yCoordinate(mData[1])));
	updateReadings(pointsZ, QPointF(xCoordinate(), yCoordinate(mData[2])));

	if (mTime <= maxTime)
	{
		mTime++;
	}

	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing);

	setMatrix(painter);
	drawAxis(painter);
	painter.restore();
	drawAxisXName(painter);
	painter.setRenderHint(QPainter::Antialiasing);
	setMatrix(painter);
	markTimeAxis(painter);

	// Paint OX readings
	drawDiagram(painter, pointsX, redPen);

	// Paint OY readings
	drawDiagram(painter, pointsY, bluePen);

	// Paint OZ readings
	drawDiagram(painter, pointsZ, greenPen);
}

int VectorSensorWidget::exec()
{
	mTimer.start();
	TrikGuiDialog::exec();
}

void VectorSensorWidget::exit()
{
	mTimer.stop();
	TrikGuiDialog::exit();
}

void VectorSensorWidget::renewFocus()
{
	this->setFocus();
}

void VectorSensorWidget::setMatrix(QPainter &painter)
{
	const qreal x0 = static_cast<qreal>(axisMargin);
	const qreal y0 = height() / 2;

	QMatrix m;
	m.translate(x0, y0);

	// Scale to flip y axis
	m.scale(1.0, -1.0);

	painter.setMatrix(m);
}

void VectorSensorWidget::drawAxis(QPainter &painter)
{
	QPen blackPen(Qt::black, 2, Qt::SolidLine);
	painter.setPen(blackPen);

	// Draw x axis
	painter.drawLine(0 - axisMargin, 0, width() - 2 * axisMargin, 0);

	// Draw y axis
	painter.drawLine(0, 0 - (height() / 2), 0, height() / 2);

	// Draw direction arrows
	painter.setBrush(Qt::black);
	QPolygonF xArrow;
	xArrow << QPointF(width() - (2 * axisMargin), 0)
		  << QPointF(width() - (3 * axisMargin), axisMargin / 2)
		  << QPointF(width() - (3 * axisMargin), 0 - (axisMargin / 2));
	painter.drawPolygon(xArrow);

	QPolygonF yArrow;
	yArrow << QPointF(0, height() / 2)
		  << QPointF(axisMargin / 2, height() / 2 - axisMargin)
		  << QPointF(0 - (axisMargin / 2), height() / 2 - axisMargin);
	painter.drawPolygon(yArrow);
}

void VectorSensorWidget::drawAxisXName(QPainter &painter)
{
	const QString name = "time, sec";
	QPointF position(width() - 4 * axisMargin, height() / 2 + 4 * axisMargin);
	const int boundingRectSize = 100;
	QRect rect = QRect(position.x() - boundingRectSize / 2
		, position.y() - boundingRectSize / 2
		, boundingRectSize, boundingRectSize);
	QFont font = QFont("times");
	font.setPixelSize(10);
	painter.setFont(font);
	painter.drawText(rect, Qt::AlignCenter, name);
}

void VectorSensorWidget::drawDiagram(QPainter &painter, QVector<QPointF> points, QPen pen)
{
	painter.setPen(pen);
	int i = 0;
	for (i = 0; i < points.size() - 1; i++)
	{
		painter.drawLine(points[i], points[i + 1]);
	}
}

void VectorSensorWidget::renew()
{
	const int dimensions = 3;
	for (int i = 0; i < dimensions; i++)
	{
		mData[i] = mSensor.read()[i];
	}
}

void VectorSensorWidget::updateReadings(QVector<QPointF> &points, QPointF newPoint)
{
	if (mTime > maxTime)
	{
		for (int i = 0; i < points.size() - 1; i++)
		{
			points[i].setY(points[i + 1].y());
		}
		points[points.size() - 1] = newPoint;
	}
	else
	{
		points.append(newPoint);
	}
}

void VectorSensorWidget::markTimeAxis(QPainter &painter)
{
	painter.save();

	// flip y axis back
	painter.scale(1, -1);
	const int pixelSize = 5;
	const int margin = 3;
	QFont font = QFont("times");
	font.setPixelSize(pixelSize);

	qreal time = 0;
	const qreal timeShift = 0.5;
	QPointF textPosition = QPointF(0, 0 + 2 * margin);
	for (int i = 0; i < maxTime; i++)
	{
		painter.drawLine(textPosition.x(), margin, textPosition.x(), 0 - margin);
		const int size = 100;
		QRect rect = QRect(textPosition.x() - size / 2, textPosition.y() - size / 2, size, size);
		painter.drawText(rect, Qt::AlignCenter, QString::number(time));
		textPosition.setX(textPosition.x() + (width() - 2 * axisMargin) / maxTime);
		time = time + timeShift;
	}
	painter.restore();
}

qreal VectorSensorWidget::xCoordinate()
{
	if (mTime >= maxTime)
	{
		return width() - (2 * axisMargin);
	}
	return (width() - 2 * axisMargin) / maxTime * mTime;
}

qreal VectorSensorWidget::yCoordinate(int value)
{
	if (value > maxValue)
	{
		return (height() / 2);
	}
	else if (value < 0 - maxValue)
	{
		return (0.0 - (height() / 2));
	}
	else
	{
		const qreal realValue = static_cast<qreal>(value);
		const qreal y = realValue / maxValue * (height() / 2);
		return y;
	}
}
