#pragma once

#include <QtCore/QVector>
#include <QtCore/QString>
#include "Neuron.h"

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

	int getBNeuronNewValue(const int &BNeuronNum);
	int getANeuronNewValue(const int &ANeuronNum);
	int makeBipolar(const int &prevValue);
	void rememberALayer(QVector<int> &intVector);
	void rememberBLayer(QVector<int> &intVector);
	void fillALayer();
	void fillBLayer();
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
	QVector<Neuron> mANeuronLayer;
	QVector<int> mBNeuronLayer;
	int mCountOfANeurons, mCountOfBNeurons, mWidth, mHeight, mXCMPos, mYCMPos;
	QVector<QString> mGesturesNames;
};
