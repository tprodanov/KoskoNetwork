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
			Neuron::mCountOfWeights = mCountOfBNeurons;
			QDomElement neuron = network.firstChildElement("neuron");
			for (int i = 0; i < mCountOfANeurons; i++) {
				Neuron newNeuron (neuron);
				mANeuronLayer << newNeuron;
				neuron = neuron.nextSiblingElement("neuron");
			}
		}
	}
	mHeight = mCountOfANeurons / mWidth;
	mBNeuronLayer.resize(mCountOfBNeurons);
	knIn.close();
}

int KoskoNetwork::getBNeuronNewValue(const int &BNeuronNum) {
	int sum = 0;
	for (int i = 0; i < mCountOfANeurons; i++) {
		sum += mANeuronLayer[i].getValueTo(BNeuronNum);
	}
	return sum;
}

int KoskoNetwork::getANeuronNewValue(const int &ANeuronNum) {
	int sum = 0;
	for (int i = 0; i < mCountOfBNeurons; i++) {
		sum += mBNeuronLayer.at(i) * mANeuronLayer[ANeuronNum].getWeight(i);
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
		intVector[i] = mANeuronLayer[i].getValue();
	}
}

void KoskoNetwork::rememberBLayer(QVector<int> &intVector) {
	for (int i = 0; i < mCountOfBNeurons; i++) {
		intVector[i] = mBNeuronLayer.at(i);
	}
}

void KoskoNetwork::fillALayer() {
	for (int i = 0; i < mCountOfANeurons; i++) {
		mANeuronLayer[i].setValue(getANeuronNewValue(i));
	}
}

void KoskoNetwork::fillBLayer() {
	for (int i = 0; i < mCountOfBNeurons; i++) {
		mBNeuronLayer[i] = makeBipolar(getBNeuronNewValue(i));
	}
}

/*QVector<int> KoskoNetwork::recognize(const QString &xmlFile) {
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
}*/

int KoskoNetwork::recognize(QString path) {
	QVector<int> prevALayer(mCountOfANeurons, 0), prevBLayer(mCountOfBNeurons, 0);
	QVector<int> nowImg(mCountOfANeurons);
	makePath(path, nowImg);
	for (int i = 0; i < mCountOfANeurons; i++) {
		mANeuronLayer[i].setValue(nowImg[i]);
	}
	rememberALayer(prevALayer);
	fillBLayer();
	while (prevBLayer != mBNeuronLayer) {
		rememberBLayer(prevBLayer);
		fillALayer();
		rememberALayer(prevALayer);
		fillBLayer();
	}
	int result = 0;
	for (int i = 0; i < mCountOfBNeurons; i++) {
		if (mBNeuronLayer[i] == 1) {
			result += std::pow(2, i);
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
