#include <gtest/gtest.h>
#include "../include/textBuffer.hpp"

// Test fixture for textBuffer
class TextBufferTest : public ::testing::Test {
protected:
    textBuffer buffer;

    void SetUp() override {
        buffer = textBuffer();
    }
};

TEST_F(TextBufferTest, InitialState) {
    EXPECT_EQ(buffer.getSize(), 1);
    EXPECT_TRUE(buffer.is_void());
}

TEST_F(TextBufferTest, NewRowInsertsAtPosition) {
    buffer.new_row("Hello", 0);
    EXPECT_EQ(buffer[0], "Hello");
    EXPECT_EQ(buffer.getSize(), 2);
}

TEST_F(TextBufferTest, NewRowIncreasesSize) {
    buffer.new_row("Line 1", 1);
    buffer.new_row("Line 2", 2);
    EXPECT_EQ(buffer.getSize(), 3);
}

TEST_F(TextBufferTest, InsertLetter) {
    buffer.new_row("Hello", 0);
    buffer.insert_letter(0, 1, 'a');
    EXPECT_EQ(buffer[0], "Haello");
}

TEST_F(TextBufferTest, DeleteLetter) {
    buffer.new_row("Hello", 0);
    buffer.delete_letter(0, 1);
    EXPECT_EQ(buffer[0], "Hllo");
}

TEST_F(TextBufferTest, RowAppend) {
    buffer.new_row("Hello", 0);
    buffer.row_append(0, " World");
    EXPECT_EQ(buffer[0], "Hello World");
}

TEST_F(TextBufferTest, DelRow) {
    buffer.new_row("Hello", 0);
    buffer.new_row("World", 1);
    buffer.del_row(0);
    EXPECT_EQ(buffer[0], "World");
    EXPECT_EQ(buffer.getSize(), 1);
}

TEST_F(TextBufferTest, MergeRows) {
    buffer.new_row("Hello", 0);
    buffer.new_row("World", 1);
    buffer.merge_rows(0, 1);
    EXPECT_EQ(buffer[0], "HelloWorld");
    EXPECT_EQ(buffer.getSize(), 1);
}

TEST_F(TextBufferTest, SliceRow) {
    buffer.new_row("Hello World", 0);
    std::string sliced = buffer.slice_row(0, 6, 11);
    EXPECT_EQ(sliced, "World");
    EXPECT_EQ(buffer[0], "Hello ");
}

TEST_F(TextBufferTest, ClearBuffer) {
    buffer.new_row("Hello", 0);
    buffer.clear();
    EXPECT_EQ(buffer.getSize(), 0);
    EXPECT_TRUE(buffer.is_void());
}

TEST_F(TextBufferTest, PushBack) {
    buffer.push_back("New line");
    EXPECT_EQ(buffer.getSize(), 2);
    EXPECT_EQ(buffer[1], "New line");
}

TEST_F(TextBufferTest, RestoreBuffer) {
    buffer.new_row("Hello", 0);
    buffer.new_row("World", 1);
    buffer.restore();
    EXPECT_EQ(buffer.getSize(), 1);
    EXPECT_TRUE(buffer.is_void());
}

TEST_F(TextBufferTest, SwapRows) {
    buffer.new_row("Hello", 0);
    buffer.new_row("World", 1);
    buffer.swap_rows(0, 1);
    EXPECT_EQ(buffer[0], "World");
    EXPECT_EQ(buffer[1], "Hello");
}

TEST_F(TextBufferTest, IsVoidRow) {
    buffer.new_row("", 0);
    EXPECT_TRUE(buffer.is_void_row(0));
    buffer.new_row("Not Empty", 1);
    EXPECT_FALSE(buffer.is_void_row(1));
}

TEST_F(TextBufferTest, GetStringRow) {
    buffer.new_row("Sample Row", 0);
    EXPECT_EQ(buffer.get_string_row(0), "Sample Row");
}

TEST_F(TextBufferTest, GetBuffer) {
    buffer.new_row("Row 1", 0);
    buffer.new_row("Row 2", 1);
    auto allRows = buffer.get_buffer();
    EXPECT_EQ(allRows.size(), 3);
    EXPECT_EQ(allRows[0], "");
    EXPECT_EQ(allRows[1], "Row 1");
    EXPECT_EQ(allRows[2], "Row 2");
}

TEST_F(TextBufferTest, IsVoid) {
    buffer.clear();
    EXPECT_TRUE(buffer.is_void());
    buffer.new_row("Non-empty", 0);
    EXPECT_FALSE(buffer.is_void());
}
