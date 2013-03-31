#include "Neuron.h"

int Neuron::mCountOfWeights;

Neuron::Neuron (const QDomElement &neuron) {
	mValue = 0;
	QString weightsStr = neuron.attribute("weights");
	mWeights.resize(mCountOfWeights);
	int lenOfNum, nowWeight;
	for (int i = 0; i < mCountOfWeights; i++) {
		lenOfNum = weightsStr.indexOf(",", 0);
		nowWeight = weightsStr.left(lenOfNum).toInt();
		mWeights[i] = nowWeight;
		weightsStr = weightsStr.right(weightsStr.size() - lenOfNum - 2);
	}
}

void Neuron::setValue(const int &newValue) {
	if (newValue > 0) {
		mValue = 1;
	}
	else {
		mValue = -1;
	}
}

int Neuron::getWeight(const int &BNeuronNum) const
{
	return mWeights[BNeuronNum];
}

int Neuron::getValueTo(const int &BNeuronNum) const
{
	return mValue * mWeights[BNeuronNum];
}

int Neuron::getValue() const
{
	return mValue;
}
