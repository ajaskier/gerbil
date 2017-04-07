#include <QString>

#include <QtTest/QTest>

class FakeTest : public QObject
{
	Q_OBJECT

public:
	FakeTest();

private Q_SLOTS:
	void testCase1();
};

FakeTest::FakeTest()
{}

void FakeTest::testCase1()
{
	QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(FakeTest)

//#include "tst_faketest.moc"
