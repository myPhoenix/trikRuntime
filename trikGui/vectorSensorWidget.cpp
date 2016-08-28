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

#include "accelerometerWidget.h"

#include <trikControl/brickInterface.h>
#include <trikControl/vectorSensorInterface.h>

using namespace trikGui;

VectorSensorWidget::VectorSensorWidget(trikControl::BrickInterface &brick
		, QWidget *parent)
	: TrikGuiDialog(parent)
	, mBrick(brick)
	, mInterval(500)
{
	mTimer.setInterval(mInterval);
	connect(&mTimer, SIGNAL(timeout()), this, SLOT(renew()));

	int i = 0;
	for (i = 0; i < 3; i++)
	{
		QLabel *newLabel = new QLabel();
		newLabel->setAlignment(Qt::AlignCenter);
		mValueLabels.append(newLabel);
		mValues.append(0);
		mLayout.addWidget(mValueLabels[i]);
	}

	setLayout(&mLayout);

	mTimer.start();
}

void VectorSensorWidget::renew()
{
	generateRandomNumbers();
	for (int i = 0; i < mValueLabels.size(); i++)
	{
		mValueLabels[i]->setText(QString::number(mValues[i]));
	}
}

void VectorSensorWidget::generateRandomNumbers()
{
	const int maxValue = 5000;
	for (int i = 0; i < mValues.size(); i++)
	{
		mValues[i] = rand() % maxValue;
	}
}

void VectorSensorWidget::renewFocus()
{
	setFocus();
}
