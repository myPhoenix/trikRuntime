#include "vectorSensorWidget.h"

#include <trikControl/brickInterface.h>
#include <trikControl/vectorSensorInterface.h>

using namespace trikGui;

VectorSensorWidget::VectorSensorWidget(trikControl::VectorSensorInterface &sensor
		, trikControl::VectorSensorInterface::Type type
		, int maximum
		, QWidget *parent)
		: TrikGuiDialog(parent)
		, mSensor(sensor)
		, mType(type)
		, maxValue(maximum)
		, mTime(0)
{
	reserveMemory();
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
	const QPen bluePen(Qt::blue, 2, Qt::SolidLine);
	const QPen redPen(Qt::red, 2, Qt::SolidLine);
	const QPen greenPen(Qt::green, 2, Qt::SolidLine);

	updateData();

	QPointF newPointX(xCoordinate(), yCoordinate(mData[0]));
	QPointF newPointY(xCoordinate(), yCoordinate(mData[1]));
	QPointF newPointZ(xCoordinate(), yCoordinate(mData[2]));

	updateReadings(pointsX, newPointX);
	updateReadings(pointsY, newPointY);
	updateReadings(pointsZ, newPointZ);

	if (mTime <= maxTime) {
		mTime++;
	}

	QPainter painter(this);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing);
	drawLegend(painter);
	drawWidgetName(painter);

	setMatrix(painter);
	drawAxis(painter);
	painter.restore();
	drawAxisXName(painter);
	painter.setRenderHint(QPainter::Antialiasing);
	setMatrix(painter);
	markTimeAxis(painter);
	markVerticalAxis(painter);

	// Paint OX readings
	drawChart(painter, pointsX, redPen);

	// Paint OY readings
	drawChart(painter, pointsY, bluePen);

	// Paint OZ readings
	drawChart(painter, pointsZ, greenPen);

	clearNewData();
}

int VectorSensorWidget::exec()
{
	mTimer.start();
	return TrikGuiDialog::exec();
}

void VectorSensorWidget::exit()
{
	mTimer.stop();
	TrikGuiDialog::exit();
}

void VectorSensorWidget::drawWidgetName(QPainter &painter)
{
	const QPen blackPen(Qt::black, 2, Qt::SolidLine);
	QFont font = QFont("times");
	font.setPixelSize(7);
	painter.setFont(font);
	painter.setPen(blackPen);
	const QRect rec(0, 0, width(), height());
	const QString accelerometerName = "accelerometer";
	const QString gyroscopeName = "gyroscope";

	if (mType == trikControl::VectorSensorInterface::Type::accelerometer) {
		painter.drawText(rec, Qt::AlignTop | Qt::AlignHCenter, accelerometerName);
	} else if (mType == trikControl::VectorSensorInterface::Type::gyroscope) {
		painter.drawText(rec, Qt::AlignTop | Qt::AlignHCenter, gyroscopeName);
	}
}

void VectorSensorWidget::drawLegend(QPainter &painter)
{
	const QPen bluePen(Qt::blue, 2, Qt::SolidLine);
	const QPen redPen(Qt::red, 2, Qt::SolidLine);
	const QPen greenPen(Qt::green, 2, Qt::SolidLine);

	QFont font = QFont("times");
	font.setPixelSize(7);
	font.setCapitalization(QFont::AllUppercase);
	font.setBold(QFont::Bold);
	painter.setFont(font);

	QRect rec(width() -  axisMargin, height() - 5 * axisMargin, axisMargin, 5 * axisMargin);
	const QString x = "x";
	const QString y = "y";
	const QString z = "z";
	painter.setPen(redPen);
	painter.drawText(rec, Qt::AlignTop | Qt::AlignRight, x);
	painter.setPen(bluePen);
	painter.drawText(rec, Qt::AlignCenter | Qt:: AlignRight, y);
	painter.setPen(greenPen);
	painter.drawText(rec, Qt::AlignBottom | Qt:: AlignRight, z);
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
	const QPen blackPen(Qt::black, 2, Qt::SolidLine);
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
	const QPointF position(width() - 5 * axisMargin, height() / 2 + 4 * axisMargin);
	const int boundingRectSize = 100;
	QRect rect = QRect(position.x() - boundingRectSize / 2
			, position.y() - boundingRectSize / 2
			, boundingRectSize, boundingRectSize);
	QFont font = QFont("times");
	font.setPixelSize(10);
	painter.setFont(font);
	painter.drawText(rect, Qt::AlignCenter, name);
}

void VectorSensorWidget::drawChart(QPainter &painter, QVector<QPointF> &points, const QPen &pen)
{
	painter.setPen(pen);
	int i = 0;
	for (i = 0; i < points.size() - 1; i++) {
		painter.drawLine(points[i], points[i + 1]);
	}
}

void VectorSensorWidget::renew()
{
	newDataX.append(mSensor.read()[0]);
	newDataY.append(mSensor.read()[1]);
	newDataZ.append(mSensor.read()[2]);
}

void VectorSensorWidget::updateReadings(QVector<QPointF> &points, QPointF &newPoint)
{
	if (mTime > maxTime) {
		for (int i = 0; i < points.size() - 1; i++)
		{
			points[i].setY(points[i + 1].y());
		}
		points[points.size() - 1] = newPoint;
	} else {
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

void VectorSensorWidget::markVerticalAxis(QPainter &painter)
{
	painter.save();

	const int pixelSize = 7;
	const int margin = 3;
	QFont font = QFont("times");
	font.setPixelSize(pixelSize);
	painter.setFont(font);

	painter.drawLine(0 - margin, height() / 4, margin, height() / 4);
	painter.drawLine(0 - margin, 0 - height() / 4, margin, 0 - height() / 4);

	// flip y axis back
	painter.scale(1, -1);
	const int size = 100;
	QRect rect1 = QRect(5 * margin - size / 2, 0 - height() / 4 - size / 2, size, size);
	painter.drawText(rect1, Qt::AlignCenter, QString::number(maxValue / 2));
	QRect rect2 = QRect(5 * margin - size / 2, height() / 4 - size / 2, size, size);
	painter.drawText(rect2, Qt::AlignCenter, QString::number(0 - maxValue / 2));

	painter.restore();
}

void VectorSensorWidget::updateData()
{
	if (!newDataX.empty()) {
		mData[0] = getAverage(newDataX);
	}

	if (!newDataX.empty()) {
		mData[1] = getAverage(newDataY);
	}

	if (!newDataX.empty()) {
		mData[2] = getAverage(newDataZ);
	}
}

int VectorSensorWidget::getAverage(QList<int> &list)
{
	qlonglong sum = 0;
	for (int i = 0; i < list.size(); i++) {
		sum = sum + list[i];
	}

	int average = sum / list.size();
	return average;
}

void VectorSensorWidget::reserveMemory()
{
	const int capacity = 500;
	newDataX.reserve(capacity);
	newDataY.reserve(capacity);
	newDataZ.reserve(capacity);
}

void VectorSensorWidget::clearNewData()
{
	newDataX.clear();
	newDataY.clear();
	newDataZ.clear();
}

qreal VectorSensorWidget::xCoordinate()
{
	if (mTime >= maxTime) {
		return width() - (2 * axisMargin);
	}

	return (width() - 2 * axisMargin) / maxTime * mTime;
}

qreal VectorSensorWidget::yCoordinate(int value)
{
	if (value > maxValue) {
		return (height() / 2);
	} else if (value < 0 - maxValue) {
		return (0.0 - (height() / 2));
	} else {
		const qreal realValue = static_cast<qreal>(value);
		const qreal y = realValue / maxValue * (height() / 2);
		return y;
	}
}
