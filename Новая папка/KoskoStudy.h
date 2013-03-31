#pragma once

#include <QtCore/QVector>
#include <QtXml>

class KoskoStudy
{
public:
	KoskoStudy(const int &countOfGestures
			   , const int &gestureWidth
			   , const int &gestureHeigh
			   , const int &xCentreMassPosition
			   , const int &yCentreMassPosition);
	void study(const QString &xmlFile);
	bool saveWeights(const QString &xmlFile);

private:
	void outp(const QVector<int> &nowImg);

	void learnGesture(const QDomElement &gesture, int gestureNum);
	void makePath(QString path, QVector<int> &nowImg);
	void makeLine(int x1
				  , int y1
				  , int x2
				  , int y2
				  , const int &width
				  , const int &height
				  , QVector<int> &img);
	void fitToSize(QVector<int> &intPath
				   , const QVector<bool> &isInc, QVector<int> &newImg
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
	QDomElement saveNeuron(QDomDocument &kn, const int &ANeuronNum);
	int mCountOfGestures, mCountOfANeurons, mCountOfBNeurons, mWidth, mHeight, mXCMPos, mYCMPos;
	double mImgRatio, mIdealImgRatio;
	QVector<int> mWeights;
	QVector<QString> mGesturesNames;
};

