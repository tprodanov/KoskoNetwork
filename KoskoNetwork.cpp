#include <QtXml>
#include "KoskoNetwork.h"
#include <fstream>
#include <cmath>

KoskoNetwork::KoskoNetwork(const QString &filename
						   , const int &width
						   , const int &xCentreMassPos
						   , const int &yCentreMassPos)
	: mWidth(width), mXCMPos(xCentreMassPos), mYCMPos(yCentreMassPos)
{
	QDomDocument kn;
	QFile knIn(filename);
	if (knIn.open(QIODevice::ReadOnly)) {
		if (kn.setContent(&knIn)) {
			QDomElement network = kn.documentElement();
			mCountOfANeurons = network.attribute("neuronsInALayer").toInt();
			mCountOfBNeurons = network.attribute("neuronsInBLayer").toInt();
			mCountOfEpochs = network.attribute("epochsNumber").toInt();
			mWeights.resize(mCountOfANeurons * mCountOfBNeurons * mCountOfEpochs);
			QDomElement epoch = network.firstChildElement("epoch");
			QDomElement line;
			int e, i;
			for (e = 0; e < mCountOfEpochs; e++) {
				line = epoch.firstChildElement("line");
				for (i = 0; i < mCountOfBNeurons; i++) {
					loadLine(line, e, i);
					line = line.nextSiblingElement("line");
				}
				epoch = epoch.nextSiblingElement("epoch");
			}
		}
	}
	mHeight = mCountOfANeurons / mWidth;
	mANeuronLayer.fill(0, mCountOfANeurons);
	mBNeuronLayer.resize(mCountOfBNeurons);
	knIn.close();
}

int KoskoNetwork::getBNeuronNewValue(const int &BNeuronNum, const int &epoch) {
	int sum = 0;
	for (int i = 0; i < mCountOfANeurons; i++) {
		sum += mWeights[(epoch * mCountOfBNeurons + BNeuronNum) * mCountOfANeurons + i] * mANeuronLayer[i];
	}
	return sum;
}

int KoskoNetwork::getANeuronNewValue(const int &ANeuronNum, const int &epoch) {
	int sum = 0;
	for (int i = 0; i < mCountOfBNeurons; i++) {
		sum += mWeights[(epoch * mCountOfBNeurons + i) * mCountOfANeurons + ANeuronNum] * mBNeuronLayer[i];
	}
	return sum;
}

int KoskoNetwork::makeBipolar(const int &prevValue) {
	if (prevValue > 0) {
		return 1;
	}
	else {
		return -1;
	}
}

void KoskoNetwork::rememberALayer(QVector<int> &intVector) {
	for (int i = 0; i < mCountOfANeurons; i++) {
		intVector[i] = mANeuronLayer[i];
	}
}

void KoskoNetwork::rememberBLayer(QVector<int> &intVector) {
	for (int i = 0; i < mCountOfBNeurons; i++) {
		intVector[i] = mBNeuronLayer[i];
	}
}

void KoskoNetwork::fillALayer(const int &epoch) {
	for (int i = 0; i < mCountOfANeurons; i++) {
		mANeuronLayer[i] = makeBipolar(getANeuronNewValue(i, epoch));
	}
}

void KoskoNetwork::fillBLayer(const int &epoch) {
	for (int i = 0; i < mCountOfBNeurons; i++) {
		mBNeuronLayer[i] = makeBipolar(getBNeuronNewValue(i, epoch));
	}
}

int KoskoNetwork::recognize(QString path) {
	QVector<int> prevALayer(mCountOfANeurons, 0), prevBLayer(mCountOfBNeurons, 0);
	int countOfGestures = std::pow(2, mCountOfBNeurons);
	QVector<int> nowImg(mCountOfANeurons), resVector(countOfGestures, 0);
	makePath(path, nowImg);
	int i, e, result;
	for (i = 0; i < mCountOfANeurons; i++) {
		mANeuronLayer[i] = makeBipolar(nowImg[i]);
	}
	rememberALayer(prevALayer);
	for (e = 0; e < mCountOfEpochs; e++) {
		mANeuronLayer = prevALayer;
		fillBLayer(e);
		while (prevBLayer != mBNeuronLayer) {
			rememberBLayer(prevBLayer);
			fillALayer(e);
			fillBLayer(e);
		}
		result = 0;
		for (i = 0; i < mCountOfBNeurons; i++) {
			if (mBNeuronLayer[i] == 1) {
				result += std::pow(2, mCountOfBNeurons - i - 1);
			}
		}
		if (result >= e)
			resVector[result - e]++;
	}
	int max = 0;
	result = 0;
	for (i = 0; i < countOfGestures; i++) {
		if (resVector[i] > max) {
			max = resVector[i];
			result = i;
		}
	}
	return result + 1;
}


void KoskoNetwork::makePath(QString path, QVector<int> &nowImg) {
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

void KoskoNetwork::makeLine(int x1
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

void KoskoNetwork::fitToSize(QVector<int> &intPath
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

void KoskoNetwork::makeMassCentres(const QVector<int> &img
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

void KoskoNetwork::outp(const QVector<int> &nowImg)
{
	std::ofstream ou ("output1.txt", std::ios::out);
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

void KoskoNetwork::loadLine(const QDomElement &line, const int &epochNum, const int &lineNum)
{
	QString weightsStr = line.attribute("weights");
	int lenOfNum, nowWeight;
	for (int i = 0; i < mCountOfANeurons; i++) {
		lenOfNum = weightsStr.indexOf(",", 0);
		nowWeight = weightsStr.left(lenOfNum).toInt();
		mWeights[(epochNum * mCountOfBNeurons + lineNum) * mCountOfANeurons + i] = nowWeight;
		weightsStr = weightsStr.right(weightsStr.size() - lenOfNum - 2);
	}
}
