#include "gtest/gtest.h"

extern "C" {
    #include "multi.c"
}

TEST(struct_List, add_element_and_free) {
    List *freq_list = new List[ALPHABET_LENGTH];
    ASSERT_TRUE(freq_list);
    add_list_element(freq_list, 2);
    ASSERT_EQ(freq_list->first->val, (size_t)2);
    list_free(freq_list);
    auto die = [](List *lst)->size_t { return lst->first->val; };
    ASSERT_EXIT((die(freq_list),exit(0)), ::testing::KilledBySignal(SIGSEGV), ".*");
}

TEST(find_most_common_sequence, seq1) {
    char const data[] = "jjjkkkiiiddffuuiioopprraa";
    ASSERT_EQ(find_most_common_sequence_char(&data[0], sizeof(data)/sizeof(data[0])), 'a');
}

TEST(find_most_common_sequence, seq2) {
    char const data[] = "iiooddd";
    ASSERT_EQ(find_most_common_sequence_char(&data[0], sizeof(data)/sizeof(data[0])), 'i');
}

TEST(find_most_common_sequence, seq3) {
    char const data[] = "oooopppplllldddddjjjjjhhhhhyyyyy";
    ASSERT_EQ(find_most_common_sequence_char(&data[0], sizeof(data)/sizeof(data[0])), 'd');
}

TEST(find_most_common_sequence, seq4) {
    char const data[] = "oooooooppppppppjjjkkkiiiooopppmmmaaa";
    ASSERT_EQ(find_most_common_sequence_char(&data[0], sizeof(data)/sizeof(data[0])), 'a');
}

TEST(find_most_common_sequence, seq5) {
    char const data[] = "ooooppiiiuuurrrryyyhhhyyssssiiiiiiijjjzzzaazzzaazzzaazzzmmzzzkkkkzzzkkkkkkkzzz";
    ASSERT_EQ(find_most_common_sequence_char(&data[0], sizeof(data)/sizeof(data[0])), 'h');
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
