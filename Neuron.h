#pragma once

#include <QtCore/QVector>
#include "KoskoNetwork.h"

class Neuron
{
	friend class KoskoNetwork;
public:
	Neuron() {}
private:
	explicit Neuron(const int countOfWeights);
	void setValue(const double neuronValue, const double sensitivity);
	void changeWeight(const int to, const double deltaValue);
	int mValue, mCountOfWeights;
	QVector<int> mWeights;
};
