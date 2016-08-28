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

#ifndef VECTORSENSORWIDGET_H
#define VECTORSENSORWIDGET_H

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QWidget>
	#include <QtGui/QVBoxLayout>
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QWidget>
	#include <QtWidgets/QVBoxLayout>
	#include <QtWidgets/QLabel>
#endif

#include <QtCore/QTimer>
#include <QtCore/QVector>

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
	VectorSensorWidget(trikControl::BrickInterface &brick, QWidget *parent = 0);

public slots:
	void renew();

protected:
	void renewFocus() override;

private:
	void generateRandomNumbers();

	trikControl::BrickInterface &mBrick;
	QVBoxLayout mLayout;
	QVector<QLabel *> mValueLabels;
	QVector<int> mValues;
	const int mInterval;
	QTimer mTimer;
};

}

#endif // ACCELEROMETERWIDGET_H
