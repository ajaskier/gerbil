#include "test_random_data_subscription.h"

#include "task_scheduler_mock.h"

#include <cstdlib>
#include <ctime>

TestRandomDataSubscription::TestRandomDataSubscription()
    : TestTaskCore()
{
	srand(time(NULL));
}

bool TestRandomDataSubscription::compareVectorsContent(std::vector<QString> v1,
                                                       std::vector<QString> v2)
{
	for (QString &s : v1) {
		if (std::find(v2.begin(), v2.end(), s) == v2.end())
			return false;
	}

	for (QString &s : v2) {
		if (std::find(v1.begin(), v1.end(), s) == v1.end())
			return false;
	}


	return true;
}


//in this test it's hard to compare the order of tasks, because
//when subscribing for many data objects at once, the outcome order
//is hard to predict
bool TestRandomDataSubscription::multiDataOrderRandomDelay(std::vector<QString> ids,
                                                           std::vector<QString> expected)
{
	std::vector<Subscription*> subs;

	bool roi = false;
	for (size_t i = 0; i < ids.size(); i++) {
		QString id = ids[i];

		if (std::find(dataTaskChain(id).begin(), dataTaskChain(id).end(),
		              "ROI") != dataTaskChain(id).end() && !roi) {
			imageModel->setROI(cv::Rect(0, 0, 128, 128));
			roi = true;
			QTest::qWait(250);
		}

		Subscription * sub =
		    DataRegister::subscribe(Dependency(id, SubscriptionType::READ,
		                                       AccessType::DEFERRED), this,
		                            std::bind(&TestRandomDataSubscription::fakeSlot, this));
		subs.push_back(sub);

		int waitMs = rand() % 250 + 30;
		QTest::qWait(waitMs);
	}

	QTest::qWait(2000);
	auto schedulerOrder = scheduler->taskOrder;
	bool res = compareVectorsContent(schedulerOrder, expected);
	for (Subscription* sub : subs) {
		sub->deleteLater();
	}


	TaskScheduler* sch = this->scheduler;
//	qDebug() << "final" << sch->timestamp();
	if (!res) {
		qDebug() << "ERROR!";
	}

	return res;

	//QCOMPARE(res, true);
}


void TestRandomDataSubscription::printIds(std::vector<QString> ids)
{
	qDebug() << "IDs: ";
	for (auto& id : ids) {
		qDebug() << id;
	}
}

void TestRandomDataSubscription::rand_interval_tst()
{
	std::vector<QString> ids = dataTaskChain.randomIds(5);
	//std::vector<QString> ids = { "image", "image", "image.IMGPCA", "image.bgr", "image.GRADPCA" };

	printIds(ids);
	MultiDataChainResult expected = dataTaskChain.multiDataChain(ids);

	bool res = multiDataOrderRandomDelay(ids, expected.totalChain);

	QCOMPARE(res, true);
}

//void TestRandomDataSubscription::multi_rand_interval_tst()
//{
//	cleanup();

//	for (int i = 0; i < 5; i++) {
//		init();
//		rand_interval_tst();
//		cleanup();

//		QTest::qWait(500);

//		qDebug() << "\n\n\n\n" << i + 1;
//	}

//	init();
//}

QTEST_MAIN(TestRandomDataSubscription)
