#include "Neuron.h"

Neuron::Neuron (const int countOfWeights) {
	mValue = 0;
	mCountOfWeights = countOfWeights;
	mWeights.resize(mCountOfWeights);
}

void Neuron::setValue(const double neuronValue, const double sensitivity) {
	if (neuronValue >= sensitivity) {
		mValue = 1;
	}
	else {
		mValue = -1;
	}
}

void Neuron::changeWeight(const int to, const double deltaValue) {
	mWeights[to] += deltaValue;
}
