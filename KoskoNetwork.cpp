#include <fstream>
#include "KoskoNetwork.h"

KoskoNetwork::KoskoNetwork(const int countOfANeurons, const int countOfBNeurons, const double sensitivity) {
	mCountOfANeurons = countOfANeurons;
	mCountOfBNeurons = countOfBNeurons;
	int i;
	for (i = 0; i < mCountOfANeurons; i++) {
		Neuron newNeuron (mCountOfBNeurons);
		mANeuronLayer << newNeuron;
	}
	mBNeuronLayer.resize(mCountOfBNeurons);
	mSensitivity = sensitivity;
}

double KoskoNetwork::getBNeuronNewValue(const int BNeuronNumber) {
	int sum = 0;
	for (int i = 0; i < mCountOfANeurons; i++) {
		sum += mANeuronLayer[i].mValue * mANeuronLayer[i].mWeights.at(BNeuronNumber);
	}
	return sum;
}

double KoskoNetwork::getANeuronNewValue(const int ANeuronNumber) {
	int sum = 0;
	for (int i = 0; i < mCountOfBNeurons; i++) {
		sum += mBNeuronLayer.at(i) * mANeuronLayer[ANeuronNumber].mWeights.at(i);
	}
	return sum;
}

int KoskoNetwork::makeBipolar(const int prevValue) {
	if (prevValue > 0) {
		return 1;
	}
	else {
		return -1;
	}
}

void KoskoNetwork::rememberALayer(QVector<int> &intVector) {
	for (int i = 0; i < mCountOfANeurons; i++) {
		intVector[i] = mANeuronLayer[i].mValue;
	}
}

void KoskoNetwork::rememberBLayer(QVector<int> &intVector) {
	for (int i = 0; i < mCountOfBNeurons; i++) {
		intVector[i] = mBNeuronLayer.at(i);
	}
}

void KoskoNetwork::fillALayer() {
	for (int i = 0; i < mCountOfANeurons; i++) {
		mANeuronLayer[i].mValue = makeBipolar(getANeuronNewValue(i));
	}
}

void KoskoNetwork::fillBLayer() {
	for (int i = 0; i < mCountOfBNeurons; i++) {
		mBNeuronLayer[i] = makeBipolar(getBNeuronNewValue(i));
	}
}

QVector<double> KoskoNetwork::recognize(const QVector<double> &input) {
	QVector<int> prevALayer(mCountOfANeurons, 0), prevBLayer(mCountOfBNeurons, 0);
	for (int i = 0; i < mCountOfANeurons; i++) {
		mANeuronLayer[i].setValue(input.at(i), mSensitivity);
	}
	rememberALayer(prevALayer);
	fillBLayer();
	while (prevBLayer != mBNeuronLayer) {
		rememberBLayer(prevBLayer);
		fillALayer();
		rememberALayer(prevALayer);
		fillBLayer();
	}
	QVector<double> resultVec(mCountOfBNeurons, 0);
	for (int i = 0; i < mCountOfBNeurons; i++) {
		if(mBNeuronLayer.at(i) == 1) {
			resultVec[i] = 1;
		}
	}
	return resultVec;
}

QVector<double> KoskoNetwork::recognize(const QVector<double> &input, const int countOfIterations) {
	QVector<int> prevALayer(mCountOfANeurons, 0), prevBLayer(mCountOfBNeurons, 0);
	int i;
	for (i = 0; i < mCountOfANeurons; i++) {
		mANeuronLayer[i].setValue(input.at(i), mSensitivity);
	}
	rememberALayer(prevALayer);
	fillBLayer();
	i = 0;
	while ((i++ < countOfIterations) && (prevBLayer != mBNeuronLayer)) {
		rememberBLayer(prevBLayer);
		fillALayer();
		rememberALayer(prevALayer);
		fillBLayer();
	}
	QVector<double> resultVec(mCountOfBNeurons, 0);
	for (i = 0; i < mCountOfBNeurons; i++) {
		if(mBNeuronLayer.at(i) == 1) {
			resultVec[i] = 1;
		}
	}
	return resultVec;
}

void KoskoNetwork::makeBipolarInput(QVector<int> &intVector
									, const QVector<double> &doubleVector
									, const int imageNumber
									) {
	for(int i = 0; i < mCountOfANeurons; i++) {
		if (doubleVector.at(imageNumber * mCountOfANeurons + i) > mSensitivity) {
			intVector [i] = 1;
		}
		else {
			intVector [i] = -1;
		}
	}
}

void KoskoNetwork::makeBipolarOutput(QVector<int> &intVector
									 , const QVector<double> &doubleVector
									 , const int imageNumber
									 , const double outputSensitivity
									 ) {
	for(int i = 0; i < mCountOfBNeurons; i++) {
		if (doubleVector.at(imageNumber * mCountOfBNeurons + i) > outputSensitivity) {
			intVector [i] = 1;
		}
		else {
			intVector [i] = -1;
		}
	}
}

void KoskoNetwork::makeBipolarOutput(QVector<int> &intVector
									 , const QVector<int> &characteristicVector
									 , const int imageNumber
									 ) {
	for (int i = 0; i < mCountOfBNeurons; i++) {
		intVector [i] = -1;
	}
	intVector[characteristicVector.at(imageNumber)] = 1;
}

void KoskoNetwork::learn(const int countOfImages
						, const QVector<double> &inputVectors
						, const QVector<double> &expectedResults
						, const double outputSensitivity
						) {
	QVector<int> bipolarNowInputVector(mCountOfANeurons), bipolarNowExpectedResult(mCountOfBNeurons);
	int i, j, k;
	for (i = 0; i < countOfImages; i++) {
		makeBipolarInput(bipolarNowInputVector, inputVectors, i);
		makeBipolarOutput(bipolarNowExpectedResult, expectedResults, i, outputSensitivity);
		for (j = 0; j < mCountOfANeurons; j++) {
			for (k = 0; k < mCountOfBNeurons; k++) {
				mANeuronLayer[j].changeWeight(k
											  , mANeuronLayer[j].mValue * mBNeuronLayer.at(k)
											  );
			}
		}
	}
}

void KoskoNetwork::learn(const int countOfImages
						, const QVector<double> &inputVectors
						, const QVector<int> &characteristicResultsVector
						) {
	QVector<int> bipolarNowInputVector(mCountOfANeurons), bipolarNowExpectedResult(mCountOfBNeurons);
	int i, j, k;
	for (i = 0; i < countOfImages; i++) {
		makeBipolarInput(bipolarNowInputVector, inputVectors, i);
		makeBipolarOutput(bipolarNowExpectedResult, characteristicResultsVector, i);
		for (j = 0; j < mCountOfANeurons; j++) {
			for (k = 0; k < mCountOfBNeurons; k++) {
				mANeuronLayer[j].changeWeight(k
											  , bipolarNowInputVector.at(j) * bipolarNowExpectedResult.at(k)
											  );
			}
		}
	}
}

void KoskoNetwork::saveWeights(const char *filename) {
	std::fstream fout;
	fout.open (filename, std::ios::out);
	int i, j;
	for (i = 0; i < mCountOfANeurons; i++) {
		for (j = 0; j < mCountOfBNeurons; j++) {
			fout << mANeuronLayer[i].mWeights.at(j) << std::endl;
		}
	}
	fout.close();
}

void KoskoNetwork::loadWeights(const char *filename) {
	std::fstream fin;
	fin.open (filename, std::ios::in);
	int i, j, tmp;
	for (i = 0; i < mCountOfANeurons; i++) {
		for (j = 0; j < mCountOfBNeurons; j++) {
			fin >> tmp;
			mANeuronLayer[i].mWeights[j] = tmp;
		}
	}
	fin.close();
}
