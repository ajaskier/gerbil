#include <gmock/gmock.h>

using namespace testing;

class SimpleTest : public Test {

public:
    int k = 5;

};

TEST_F(SimpleTest, simple_test) {
    k++;
    ASSERT_EQ(k, 6);

}
