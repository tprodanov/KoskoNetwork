#include <QtCore/QVector>
#include <iostream>
#include "KoskoNetwork.h"
#include "KoskoStudy.h"

using namespace std;

int s; int s1;

void S() {
	KoskoStudy StudyTest(10, s, s, s1, s1);
	StudyTest.study("sample.xml");
	cout << 1;
	StudyTest.saveWeights("weights.xml");
	cout << 2;
}

void N() {
	KoskoNetwork NetworkTest("weights.xml", s, s1, s1);
	QString path = "90, 0 : 100, 10 : 100, 50 : 0, 50 : 0, 0 : 90, 0 : 90, 10 : 100, 10 : ";
	cout <<  NetworkTest.recognize(path);
}

int main()
{
	s = 80;
	s1 = 27;
	S();
	cout << endl;
	N();
}
