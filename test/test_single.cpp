#include "gtest/gtest.h"

extern "C" {
    #include "single.c"
}

TEST(struct_list, add_element_and_free) {
    list *freq_list = new list[ALPHABET_LENGTH];
    ASSERT_TRUE(freq_list);
    add_list_element(freq_list, 2, ALPHABET_LENGTH);
    ASSERT_EQ(freq_list->first->val, (size_t)2);
    ASSERT_EQ(list_free(freq_list, ALPHABET_LENGTH), 0);
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
