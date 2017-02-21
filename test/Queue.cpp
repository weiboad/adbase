#include <adbase/Utility.hpp>
#include <gtest/gtest.h>

template<typename T>
void fill_n(adbase::Queue<T>& queue,size_t n_size,T value) {
    for (int64_t i = 0; i < n_size; i++) {
        queue.push(value);
    }
}

// {{{ construct
TEST(QueueTest, testQueueConstruct) {
	//default constructor
	adbase::Queue<int64_t> queue;
	EXPECT_EQ(queue.empty(), true);

	//copy constructor
	adbase::Queue<int64_t> queue_copy(queue);
	EXPECT_EQ(queue.empty(), true);
}
// }}}
// {{{ push
TEST(QueueTest, testQueuePush) {
	adbase::Queue<int64_t> queue;

	const int SIZE_STEP = 100000;

	fill_n(queue,SIZE_STEP * 1,static_cast<int64_t>(1));
	EXPECT_EQ(queue.getSize(), SIZE_STEP * 1);

	fill_n(queue,SIZE_STEP * 2,static_cast<int64_t>(2));
	EXPECT_EQ(queue.getSize(), SIZE_STEP * 3);

	fill_n(queue,SIZE_STEP * 4,static_cast<int64_t>(3));
	EXPECT_EQ(queue.getSize(), SIZE_STEP * 7);
}
// }}}

// {{{ pop
TEST(QueueTest, testQueuePop) {
	adbase::Queue<int64_t> queue;

	queue.push(0);
	queue.push(1);
	queue.push(2);

	std::shared_ptr<int64_t> n_ret_ptr_0 = queue.tryPop();
	EXPECT_EQ(*(n_ret_ptr_0.get()), 0);

	std::shared_ptr<int64_t>  n_ret_ptr_1 = queue.waitPop();
	EXPECT_EQ(*(n_ret_ptr_1.get()), 1);

	int64_t n_ret_2 = 0;
	queue.waitPop(n_ret_2);
	EXPECT_EQ(n_ret_2, 2);
}
// }}}

