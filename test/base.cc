
#include <gtest/gtest.h>
import jcast;

using namespace jcast;

TEST(add, basic) {
	ASSERT_EQ(4, add(3, 1));
}
