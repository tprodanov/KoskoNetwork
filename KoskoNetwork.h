#pragma once

#include <QtXml>
#include <QtCore/QVector>
#include <QtCore/QString>

class Neuron;

class KoskoNetwork
{
public:
	KoskoNetwork (const QString &filename
						   , const int &width
						   , const int &xCentreMassPos
						   , const int &yCentreMassPos);
	int recognize(QString path);
private:
	void outp(const QVector<int> &nowImg);

	void loadLine(const QDomElement &line, const int &epochNum, const int &lineNum);
	int getBNeuronNewValue(const int &BNeuronNum, const int &epoch);
	int getANeuronNewValue(const int &ANeuronNum, const int &epoch);
	int makeBipolar(const int &prevValue);
	void rememberALayer(QVector<int> &intVector);
	void rememberBLayer(QVector<int> &intVector);
	void fillALayer(const int &epoch);
	void fillBLayer(const int &epoch);
	void makePath(QString path, QVector<int> &nowImg);
	void makeLine(int x1
				  , int y1
				  , int x2
				  , int y2
				  , const int &width
				  , const int &height
				  , QVector<int> &img);
	void fitToSize(QVector<int> &intPath
				   , const QVector<bool> &isInc
				   , QVector<int> &newImg
				   , const int &leftCM
				   , const int &rightCM
				   , const int &upperCM
				   , const int &bottomCM);
	void makeMassCentres(const QVector<int> &img
						 , const int &width
						 , const int &height
						 , int &leftCM
						 , int &rightCM
						 , int &upperCM
						 , int &bottomCM);
	QVector<int> mANeuronLayer, mBNeuronLayer, mWeights;
	int mCountOfANeurons, mCountOfBNeurons, mWidth, mHeight, mXCMPos, mYCMPos, mCountOfEpochs;
	QVector<QString> mGesturesNames;
};
