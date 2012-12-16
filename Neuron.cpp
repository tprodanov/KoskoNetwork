#include "Neuron.h"

Neuron::Neuron (const int countOfWeights) {
	mValue = 0;
	mCountOfWeights = countOfWeights;
	mWeights.resize(mCountOfWeights);
}

Neuron::Neuron (const QDomElement &neuron, const int countOfWeights) {
	mValue = 0;
	mCountOfWeights = countOfWeights;
	QDomElement weight = neuron.firstChild().toElement();
	for (int i = 0; i < mCountOfWeights; i++) {
		mWeights << weight.attribute("value").toInt();
		weight = weight.nextSibling().toElement();
	}
}

void Neuron::setValue(const int neuronValue) {
	if (neuronValue > 0) {
		mValue = 1;
	}
	else {
		mValue = -1;
	}
}

void Neuron::changeWeight(const int to, const int deltaValue) {
	mWeights[to] += deltaValue;
}

QDomElement Neuron::save(QDomDocument &kn, int neuronNumber) {
	QDomElement neuron = kn.createElement("neuron");
	neuron.setAttribute("neuronNumber", neuronNumber);
	for (int i =0; i < mCountOfWeights; i++) {
		QDomElement weight = kn.createElement("weight");
		weight.setAttribute("number", i + 1);
		weight.setAttribute("value", mWeights.at(i));
		neuron.appendChild(weight);
	}
	return neuron;
}

void Neuron::setWeights(const QDomElement &neuron) {
	mValue = 0;
	QDomElement weight = neuron.firstChild().toElement();
	for (int i = 0; i < mCountOfWeights; i++) {
		mWeights[i] = weight.attribute("value").toInt();
		weight = weight.nextSibling().toElement();
	}
}
