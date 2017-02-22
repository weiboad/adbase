#include <adbase/Utility.hpp>
#include <gtest/gtest.h>
#include <random>
#include <thread>

// {{{ construct
TEST(SequenceTest, testSequenceConstruct) {
    adbase::Sequence sequence;
}
// }}}

// {{{ getSeqId
TEST(SequenceTest, SequenceGetId) {
    adbase::Sequence sequence;

    //generate macId & appid
    std::default_random_engine random_engine;
    std::uniform_int_distribution<uint16_t> num_range(1000, 2000);
    uint16_t macId = num_range(random_engine);
    uint16_t appid = num_range(random_engine);

    uint64_t seq_id_prev = sequence.getSeqId(macId,appid);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    uint64_t seq_id_pos = sequence.getSeqId(macId,appid);

    EXPECT_EQ(seq_id_prev == seq_id_pos, false);

}
// }}}
