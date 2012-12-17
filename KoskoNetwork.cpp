#include <QtXml>
#include "KoskoNetwork.h"

KoskoNetwork::KoskoNetwork(const int countOfANeurons, const int countOfBNeurons) {
	mCountOfANeurons = countOfANeurons;
	mCountOfBNeurons = countOfBNeurons;
	for (int i = 0; i < mCountOfANeurons; i++) {
		Neuron newNeuron (mCountOfBNeurons);
		mANeuronLayer << newNeuron;
	}
	mBNeuronLayer.resize(mCountOfBNeurons);
}

KoskoNetwork::KoskoNetwork(const QString &filename) {
	QDomDocument kn;
	QFile knIn(filename);
	if (knIn.open(QIODevice::ReadOnly)) {
		if (kn.setContent(&knIn)) {
			QDomElement network = kn.documentElement();
			mCountOfANeurons = network.attribute("neuronsInALayer").toInt();
			mCountOfBNeurons = network.attribute("neuronsInBLayer").toInt();
			QDomElement neuron = network.firstChildElement("neuron");
			for (int i = 0; i < mCountOfANeurons; i++) {
				Neuron newNeuron (neuron, mCountOfBNeurons);
				mANeuronLayer << newNeuron;
				neuron = neuron.nextSiblingElement("neuron");
			}
		}
	}
	mBNeuronLayer.resize(mCountOfBNeurons);
	knIn.close();
}

int KoskoNetwork::getBNeuronNewValue(const int BNeuronNumber) {
	int sum = 0;
	for (int i = 0; i < mCountOfANeurons; i++) {
		sum += mANeuronLayer[i].mValue * mANeuronLayer[i].mWeights.at(BNeuronNumber);
	}
	return sum;
}

int KoskoNetwork::getANeuronNewValue(const int ANeuronNumber) {
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

QVector<int> KoskoNetwork::recognize(const QString &xmlFile) {
	QVector<int> prevALayer(mCountOfANeurons, 0), prevBLayer(mCountOfBNeurons, 0);
	int i, j;
	QDomDocument picture;
	QFile pictIn(xmlFile);
	pictIn.open(QIODevice::ReadOnly);
	picture.setContent(&pictIn);
	QDomElement image = picture.firstChildElement("image");
	int width = image.attribute("width").toInt();
	int height = image.attribute("height").toInt();
	QDomElement line = image.firstChildElement("line");
	QString lineStr (width);
	for (i = 0; i < height; i++) {
		lineStr = line.attribute("value");
		for (j = 0; j < width; j++) {
			if (lineStr[j] == '1') {
				mANeuronLayer[i * width + j].setValue(1);
			}
			else {
				mANeuronLayer[i * width + j].setValue(-1);
			}
		}
		line = line.nextSiblingElement("line");
	}
	pictIn.close();
	rememberALayer(prevALayer);
	fillBLayer();
	while (prevBLayer != mBNeuronLayer) {
		rememberBLayer(prevBLayer);
		fillALayer();
		rememberALayer(prevALayer);
		fillBLayer();
	}
	QVector<int> resultVec(mCountOfBNeurons, 0);
	for (int i = 0; i < mCountOfBNeurons; i++) {
		if(mBNeuronLayer.at(i) == 1) {
			resultVec[i] = 1;
		}
	}
	return resultVec;
}

void KoskoNetwork::learn(const QString &xmlFile) {
	QDomDocument sampleXml;
	QFile sampleIn(xmlFile);
	if(sampleIn.open(QIODevice::ReadOnly)) {
		if(sampleXml.setContent(&sampleIn)) {
			for (int i = 0; i < mCountOfANeurons; i++) {
				mANeuronLayer[i].eraseWeights();
			}
			QDomElement sample = sampleXml.documentElement();
			int countOfImgs = sample.attribute("countOfImages").toInt();
			int width = sample.attribute("width").toInt();
			int height = sample.attribute("height").toInt();
			QDomElement image = sample.firstChildElement("image");
			for (int i = 0; i < countOfImgs; i++) {
				learnImage(image, width, height);
				image = image.nextSiblingElement("image");
			}
		}
	}
	sampleIn.close();
}

void KoskoNetwork::learnImage(QDomElement &image, const int width, const int height) {
	QVector<int> nowExpectedResult(mCountOfBNeurons, -1);
	nowExpectedResult[image.attribute("imageClass").toInt()] = 1;
	int i, j, k;
	QDomElement line = image.firstChildElement("line");
	QString lineStr;
	for (i = 0; i < height; i++) {
		lineStr = line.attribute("value");
		for (j = 0; j < width; j++) {
			if (lineStr.at(j) == '1') {
				for (k = 0; k < mCountOfBNeurons; k++) {
					mANeuronLayer[i * width + j].changeWeight(k, nowExpectedResult.at(k));
				}
			}
			else {
				for (k = 0; k < mCountOfBNeurons; k++) {
					mANeuronLayer[i * width + j].changeWeight(k, -nowExpectedResult.at(k));
				}
			}
		}
	}
}

bool KoskoNetwork::saveWeights(const QString &xmlFile) {
	QDomDocument kn("KoskoNetwork");
	QDomElement network = kn.createElement("KoskoNetwork");
	network.setAttribute("neuronsInALayer", mCountOfANeurons);
	network.setAttribute("neuronsInBLayer", mCountOfBNeurons);
	kn.appendChild(network);
	for (int i = 0; i < mCountOfANeurons; i++) {
		network.appendChild(mANeuronLayer[i].save(kn, i + 1));
	}
	QFile saveKN(xmlFile);
	if(saveKN.open(QIODevice::WriteOnly)) {
		QTextStream(&saveKN) << kn.toString();
		saveKN.close();
	}
	else{
		return true;
	}
	return false;
}

void KoskoNetwork::loadWeights (const QString &xmlFile) {
	QDomDocument kn;
	QFile knIn(xmlFile);
	if (knIn.open(QIODevice::ReadOnly)) {
		if (kn.setContent(&knIn)) {
			QDomElement network = kn.documentElement();
			QDomElement neuron = network.firstChildElement("neuron");
			for (int i = 0; i < mCountOfANeurons; i++) {
				mANeuronLayer[i].setWeights(neuron);
				neuron = neuron.nextSiblingElement("neuron");
			}
		}
	}
	knIn.close();
}
