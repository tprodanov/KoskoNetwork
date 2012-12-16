#pragma once

#include <QtCore/QVector>
#include <QtXml>
#include "KoskoNetwork.h"

class Neuron
{
	friend class KoskoNetwork;
public:
	Neuron() {}
private:
	explicit Neuron(const int countOfWeights);
	Neuron(const QDomElement &neuron, const int countOfWeights);
	void setValue(const int neuronValue);
	void changeWeight(const int to, const int deltaValue);
	QDomElement save(QDomDocument &kn, int neuronNumber);
	void setWeights(const QDomElement &neuron);
	int mValue, mCountOfWeights;
	QVector<int> mWeights;
};
