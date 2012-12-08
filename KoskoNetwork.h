#pragma once

#include <QtCore/QVector>
#include "Neuron.h"

class Neuron;

class KoskoNetwork
{
public:
	KoskoNetwork(const int countOfANeurons, const int countOfBNeurons, const double sensitivity);
	QVector<double> recognize(const QVector<double> &input);
	QVector<double> recognize(const QVector<double> &input, const int countOfIterations);
	void learn(const int countOfImages
			   , const QVector<double> &inputVectors
			   , const QVector<double> &expectedResults
			   , const double outputSensitivity);
	void learn(const int countOfImages
			   , const QVector<double> &inputVectors
			   , const QVector<int> &characteristicResultsVector);
	void saveWeights(const char *filename);
	void loadWeights(const char *filename);
private:
	QVector<Neuron> mANeuronLayer;
	QVector<int> mBNeuronLayer;
	double getBNeuronNewValue(const int BNeuronNumber);
	double getANeuronNewValue(const int ANeuronNumber);
	int makeBipolar(const int prevValue);
	void makeBipolarInput(QVector<int> &intVector, const QVector<double> &doubleVector, const int imageNumber);
	void makeBipolarOutput(QVector<int> &intVector
						   , const QVector<double> &doubleVector
						   , const int imageNumber
						   , const double outputSensitivity);
	void makeBipolarOutput(QVector<int> &intVector, const QVector<int> &characteristicVector, const int imageNumber);
	void rememberALayer(QVector<int> &intVector);
	void rememberBLayer(QVector<int> &intVector);
	void fillALayer();
	void fillBLayer();
	int mCountOfANeurons, mCountOfBNeurons;
	double mSensitivity;
};
