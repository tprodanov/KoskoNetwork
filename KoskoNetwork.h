#pragma once

#include <QtCore/QVector>
#include <QtCore/QString>
#include "Neuron.h"

class Neuron;

class KoskoNetwork
{
public:
	KoskoNetwork(const int countOfANeurons, const int countOfBNeurons);
	explicit KoskoNetwork (const QString &filename);
	QVector<int> recognize(const QString &xmlFile);
	void learn(const QString &xmlFile);
	bool saveWeights(const QString &xmlFile);
	void loadWeights(const QString &xmlFile);
private:
	QVector<Neuron> mANeuronLayer;
	QVector<int> mBNeuronLayer;
	int getBNeuronNewValue(const int BNeuronNumber);
	int getANeuronNewValue(const int ANeuronNumber);
	int makeBipolar(const int prevValue);
	void rememberALayer(QVector<int> &intVector);
	void rememberBLayer(QVector<int> &intVector);
	void fillALayer();
	void fillBLayer();
	void learnImage(QDomElement &image, const int width, const int height);
	int mCountOfANeurons, mCountOfBNeurons;
};
