#pragma once

#include <QtCore/QVector>
#include <QtXml>

class Neuron
{
public:
	Neuron() {}
	explicit Neuron(const QDomElement &neuron);
	void setValue(const int &newValue);
	int getWeight(const int &BNeuronNum) const;
	int getValueTo(const int &BNeuronNum) const;
	int getValue() const;
	static int mCountOfWeights;
private:
	int mValue;
	QVector<int> mWeights;
};
