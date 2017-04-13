#include "test_multi_data_subscriptions.h"

#include "task_scheduler_mock.h"

#include <cstdlib>
#include <ctime>

TestMultiDataSubscriptions::TestMultiDataSubscriptions()
    : TestTaskCore()
{
	srand(time(NULL));
}

void TestMultiDataSubscriptions::multiDataOrderTest(std::vector<QString> ids,
                                                    MultiDataChainResult expected)
{
	std::vector<Subscription*> subs;
	std::vector<QString>       totalRes;

	bool roi = false;
	for (size_t i = 0; i < ids.size(); i++) {
		QString id = ids[i];

		if (std::find(dataTaskChain(id).begin(), dataTaskChain(id).end(),
		              "ROI") != dataTaskChain(id).end() && !roi) {
			imageModel->setROI(cv::Rect(0, 0, 128, 128));
			roi = true;
			QTest::qWait(100);
		}

		Subscription * sub =
		    DataRegister::subscribe(Dependency(id, SubscriptionType::READ,
		                                       AccessType::DEFERRED), this,
		                            std::bind(&TestMultiDataSubscriptions::fakeSlot, this));
		subs.push_back(sub);

		int waitMs = 250 * dataTaskChain(id).size();
		QTest::qWait(waitMs);

		std::vector<QString> taskOrder = scheduler->taskOrder;
		QCOMPARE(taskOrder, expected.steps[i]);
		scheduler->flushVector();

		totalRes.insert(totalRes.end(), taskOrder.begin(), taskOrder.end());
	}

	for (Subscription* sub : subs) {
		sub->deleteLater();
	}

	QCOMPARE(totalRes, expected.totalChain);
}

void TestMultiDataSubscriptions::printIds(std::vector<QString> ids)
{
	qDebug() << "IDs: ";
	for (auto& id : ids) {
		qDebug() << id;
	}
}

void TestMultiDataSubscriptions::tst()
{
	QSKIP("test");

	std::vector<QString> ids = dataTaskChain.randomIds(5);
	printIds(ids);
	MultiDataChainResult expected = dataTaskChain.multiDataChain(ids);

	multiDataOrderTest(ids, expected);
}

//void TestRandomDataSubscription::multi_tst()
//{
//	cleanup();

//	for (int i = 0; i < 5; i++) {
//		qDebug() << i;
//		init();
//		tst();
//		cleanup();

//		QTest::qWait(200);
//	}

//	init();
//}

QTEST_MAIN(TestMultiDataSubscriptions)
