#include <QtCore/QVector>
#include <iostream>
#include "KoskoNetwork.h"

using namespace std;

int main()
{
	KoskoNetwork Test (6400, 2);
	Test.learn("sample.xml");
	Test.saveWeights("weights.xml");
	KoskoNetwork Test1 (6400, 2);
	Test1.loadWeights("weights.xml");
	QVector<int> answer;
	answer = Test1.recognize("img.xml");
	cout << answer.at(0) << " " << answer.at(1);
}
