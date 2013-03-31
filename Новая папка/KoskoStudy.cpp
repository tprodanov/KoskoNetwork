#include <QString>
#include <iostream>
#include <cmath>
#include "KoskoStudy.h"

#include <string>
#include <fstream>

KoskoStudy::KoskoStudy(const int &countOfGestures
					   , const int &gestureWidth
					   , const int &gestureHeight
					   , const int &xCentreMassPosition
					   , const int &yCentreMassPosition)
{
	mCountOfGestures = countOfGestures;
	mCountOfBNeurons = std::ceil(std::log(mCountOfGestures)/std::log(2));
	mWidth = gestureWidth;
	mHeight = gestureHeight;
	mCountOfANeurons = mWidth * mHeight;
	mWeights.resize(mCountOfANeurons * mCountOfBNeurons);
	mXCMPos = xCentreMassPosition;
	mYCMPos = yCentreMassPosition;
}

void KoskoStudy::study(const QString &xmlFile) {
	QDomDocument gesturesXml;
	QFile gesturesIn(xmlFile);
	if(gesturesIn.open(QIODevice::ReadOnly)) {
		if(gesturesXml.setContent(&gesturesIn)) {
			mWeights.fill(0);
			QDomElement usrGesture = gesturesXml.documentElement();
			mGesturesNames.clear();
			QDomElement gesture = usrGesture.firstChildElement("gesture");
			int gestureNum = 0;
			while ((!gesture.isNull()) && (gestureNum < mCountOfGestures)) {
				mGesturesNames << gesture.attribute("name");
				learnGesture(gesture, gestureNum);
				gesture = gesture.nextSiblingElement("gesture");
				gestureNum++;
			}
		}
	}
	gesturesIn.close();
}

void KoskoStudy::learnGesture(const QDomElement &gesture, int gestureNum) {
	QVector<int> nowExpectedResult(mCountOfBNeurons);
	//gestureNum += 3;
	for (int i = 0; i < mCountOfBNeurons; i++) {
		nowExpectedResult[i] = gestureNum & 1;
		if (nowExpectedResult[i] == 0)
			nowExpectedResult[i] = -1;
		gestureNum = gestureNum >> 1;
	}
	int i, j;
	QString path = gesture.attribute("idealPath");
	QVector<int> nowImg (mCountOfANeurons);
	makePath(path, nowImg);
	for (i = 0; i < mCountOfANeurons; i++) {
		if (nowImg.at(i) == 1) {
			for (j = 0; j < mCountOfBNeurons; j++) {
				mWeights[i * mCountOfBNeurons + j] += nowExpectedResult.at(j);
			}
		}
		else {
			for (j = 0; j < mCountOfBNeurons; j++) {
				mWeights[i * mCountOfBNeurons + j] -= nowExpectedResult.at(j);
			}
		}
	}
}

void KoskoStudy::makePath(QString path, QVector<int> &nowImg) {
	int lenOfNum;
	QVector<int> intPath;
	int leftBorder = 10000, upperBorder = -1000, rightBorder = -1000, bottomBorder = 10000, tmp;
	QVector<bool> isInc;
	while (path != "") {
		if (path[1] == '|') {
			isInc << 0;
			path = path.right(path.size() - 3);
			if (path == "")
				break;
		}
		else
			isInc << 1;
		lenOfNum = path.indexOf(",", 0);
		tmp = path.left(lenOfNum).toInt();
		intPath << tmp;
		if (tmp < leftBorder) {
			leftBorder = tmp;
		}
		if (tmp > rightBorder) {
			rightBorder = tmp;
		}
		path = path.right(path.size() - lenOfNum - 2);
		lenOfNum = path.indexOf(" ", 0);
		tmp = path.left(lenOfNum).toInt();
		intPath << tmp;
		if (tmp < bottomBorder) {
			bottomBorder = tmp;
		}
		if (tmp > upperBorder) {
			upperBorder = tmp;
		}
		path = path.right(path.size() - lenOfNum - 3);
	}
	int i;
	for (i = 0; i < intPath.size(); i += 2) {
		intPath[i] -= leftBorder;
		intPath[i + 1] -= bottomBorder;
	}
	nowImg.fill(0);
	int width = rightBorder - leftBorder + 1;
	int height = upperBorder - bottomBorder + 1;
	QVector<int> img (width * height, 0);
	for (i = 0; i < intPath.size() - 2; i += 2) {
		if (isInc[1 + i / 2]) {
			makeLine(intPath.at(i)
					 , intPath.at(i + 1)
					 , intPath.at(i + 2)
					 , intPath.at(i + 3)
					 , width
					 , height
					 , img
					 );
		}
	}
	int leftCM, rightCM, upperCM, bottomCM;
	makeMassCentres(img, width, height, leftCM, rightCM, upperCM, bottomCM);
	fitToSize(intPath, isInc, nowImg, leftCM, rightCM, upperCM, bottomCM);
	outp(nowImg);
}

void KoskoStudy::makeLine(int x1
						  , int y1
						  , int x2
						  , int y2
						  , const int &width
						  , const int &height
						  , QVector<int> &img)
{
	if ((x1 == x2) && (y1 == y2)) {
		if ((x1 < 0) || (x1 >= width) || (y1 < 0) || (y1 >= height)) {
			return;
		}
		img[y1 * width + x1] = 0;
		return;
	}
	int x, y;
	if (abs(x2 - x1) >= abs(y2 - y1)) {
		if (x2 >= x1) {
			for (x = x1; x <= x2; x++) {
				if ((x < 0) || (x >= width)) {
					continue;
				}
				y = (x - x1)*(y2 - y1)/(x2 - x1) + y1;
				if ((y < 0) || (y >= height)) {
					continue;
				}
				img[y * width + x] = 1;
			}
			return;
		}
		for (x = x1; x >= x2; x--) {
			if ((x < 0) || (x >= width)) {
				continue;
			}
			y = (x - x1)*(y2 - y1)/(x2 - x1) + y1;
			if ((y < 0) || (y >= height)) {
				continue;
			}
			img[y * width + x] = 1;
		}
		return;
	}
	if (y2 >= y1) {
		for (y = y1; y <= y2; y++) {
			if ((y < 0) || (y >= height)) {
				continue;
			}
			x = (y - y1)*(x2 - x1)/(y2 - y1) + x1;
			if ((x < 0) || (x >= width)) {
				continue;
			}
			img[y * width + x] = 1;
		}
		return;
	}
	for (y = y1; y >= y2; y--) {
		if ((y < 0) || (y >= height)) {
			continue;
		}
		x = (y - y1)*(x2 - x1)/(y2 - y1) + x1;
		if ((x < 0) || (x >= width)) {
			continue;
		}
		img[y * width + x] = 1;
	}
}

void KoskoStudy::fitToSize(QVector<int> &intPath
						   , const QVector<bool> &isInc
						   , QVector<int> &newImg
						   , const int &leftCM
						   , const int &rightCM
						   , const int &upperCM
						   , const int &bottomCM)
{
	int tmp, i;
	for (i = 0; i < intPath.size(); i += 2) {
		tmp = mXCMPos + (intPath[i] - leftCM) * ((double) (mWidth - 2 * mXCMPos)) / (rightCM - leftCM);
		intPath[i] = tmp;
		tmp = mYCMPos + (intPath[i + 1] - bottomCM) * ((double) (mHeight - 2 * mYCMPos)) / (upperCM - bottomCM);
		intPath[i + 1] = tmp;
	}
	newImg.clear();
	newImg.fill(0, mWidth * mHeight);
	for (i = 0; i < intPath.size() - 2; i += 2) {
		if (isInc[1 + i / 2]) {
			makeLine(intPath.at(i)
				 , intPath.at(i + 1)
				 , intPath.at(i + 2)
				 , intPath.at(i + 3)
				 , mWidth
				 , mHeight
				 , newImg);
		}
	}
}

void KoskoStudy::makeMassCentres(const QVector<int> &img
								 , const int &width
								 , const int &height
								 , int &leftCM
								 , int &rightCM
								 , int &upperCM
								 , int &bottomCM)
{
	int numOfDotsLeft = 0, numOfDotsRight = 0, numOfDotsUpper = 0, numOfDotsBottom = 0;
	int i, j;
	leftCM = 0;
	rightCM = 0;
	upperCM = 0;
	bottomCM = 0;
	for (i = 0; i <= width / 2; i++) {
		for (j = 0; j < height; j++) {
			leftCM += img[j * width + i] * i;
			numOfDotsLeft += img[j * width + i];
			rightCM += img[j * width + width - i - 1] * (width - i - 1);
			numOfDotsRight += img[j * width + width - i - 1];
		}
	}
	leftCM /= numOfDotsLeft;
	rightCM /= numOfDotsRight;
	for (j = 0; j <= height / 2; j++) {
		for (i = 0; i < width; i++) {
			bottomCM += img[j * width + i] * j;
			numOfDotsBottom += img[j * width + i];
			upperCM += img[(height - j - 1) * width + i] * (height - j - 1);
			numOfDotsUpper += img[(height - j - 1) * width + i];
		}
	}
	upperCM /= numOfDotsUpper;
	bottomCM /= numOfDotsBottom;
}

bool KoskoStudy::saveWeights(const QString &xmlFile) {
	QDomDocument kn("KoskoNetwork");
	QDomElement network = kn.createElement("KoskoNetwork");
	network.setAttribute("neuronsInALayer", mCountOfANeurons);
	network.setAttribute("neuronsInBLayer", mCountOfBNeurons);
	kn.appendChild(network);
	for (int i = 0; i < mCountOfANeurons; i++) {
		network.appendChild(saveNeuron(kn, i));
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

void KoskoStudy::outp(const QVector<int> &nowImg)
{
	std::ofstream ou ("output.txt", std::ios::app);
	//ou.seekp(std::ios_base::end);
	char ch;
	for (int i = 0; i < mHeight; i++) {
		for (int j = 0; j < mWidth; j++) {
			if (nowImg[i * mWidth + j] == 1)
				ch = 7;
			else
				ch = ' ';
			ou << ch;
		}
		ou << std::endl;
	}
	ou << std::endl;
	ou.close();
}

QDomElement KoskoStudy::saveNeuron(QDomDocument &kn, const int &ANeuronNum) {
	QDomElement neuron = kn.createElement("neuron");
	neuron.setAttribute("neuronNumber", ANeuronNum + 1);
	QString line = "", strTmp;
	for (int i = 0; i < mCountOfBNeurons; i++) {
		strTmp.append(QString("%1").arg(mWeights.at(ANeuronNum * mCountOfBNeurons + i)));
		line.append(strTmp + ", ");
		strTmp.clear();
	}
	neuron.setAttribute("weights", line);
	return neuron;
}
