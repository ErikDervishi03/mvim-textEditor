#include <gtest/gtest.h>
#include "../include/bufferManager.hpp" // Include the BufferManager header file

class BufferManagerTest : public ::testing::Test {
protected:
    BufferManager bufferManager; // Instance of BufferManager for testing

    void SetUp() override {
        // Set up before each test if needed
    }

    void TearDown() override {
        // Clean up after each test if needed
    }
};

// Test for creating buffers and checking the count
TEST_F(BufferManagerTest, CreateBuffers) {
    bufferManager.create_buffer("Buffer1");
    bufferManager.create_buffer("Buffer2");
    bufferManager.create_buffer("Buffer3");
    bufferManager.create_buffer("Buffer4");

    EXPECT_EQ(bufferManager.getBufferCount(), 4);
    EXPECT_THROW(bufferManager.create_buffer("Buffer5"), std::runtime_error);
}

// Test for switching to the next buffer
TEST_F(BufferManagerTest, SwitchToNextBuffer) {
    std::string bufName;

    bufferManager.create_buffer("Buffer1");

    bufferManager.next();
    bufName = "Buffer1";
    ASSERT_STREQ(bufferManager.get_current_buffer_name().c_str(), bufName.c_str());

    bufferManager.create_buffer("Buffer2");
    bufferManager.create_buffer("Buffer3");

    bufName = "Buffer1";
    ASSERT_STREQ(bufferManager.get_current_buffer_name().c_str(), bufName.c_str());

    bufferManager.next();
    bufName = "Buffer2";
    ASSERT_STREQ(bufferManager.get_current_buffer_name().c_str(), bufName.c_str());

    bufferManager.next();
    bufName = "Buffer3";
    ASSERT_STREQ(bufferManager.get_current_buffer_name().c_str(), bufName.c_str());
}

// Test for switching to the previous buffer
TEST_F(BufferManagerTest, SwitchToPreviousBuffer) {
    std::string bufName;

    bufferManager.create_buffer("Buffer1");

    bufferManager.previous();
    bufName = "Buffer1";
    ASSERT_STREQ(bufferManager.get_current_buffer_name().c_str(), bufName.c_str());

    bufferManager.create_buffer("Buffer2");
    bufferManager.create_buffer("Buffer3");

    bufferManager.previous();
    bufName = "Buffer3";
    ASSERT_STREQ(bufferManager.get_current_buffer_name().c_str(), bufName.c_str());

    bufferManager.previous();
    bufName = "Buffer2";
    ASSERT_STREQ(bufferManager.get_current_buffer_name().c_str(), bufName.c_str());

    bufferManager.previous();
    bufName = "Buffer1";
    ASSERT_STREQ(bufferManager.get_current_buffer_name().c_str(), bufName.c_str());
}

// Test for setting active buffer by index
TEST_F(BufferManagerTest, SetActiveBuffer) {
    bufferManager.create_buffer("Buffer1");
    bufferManager.create_buffer("Buffer2");

    bufferManager.set_active_buffer(1);
    EXPECT_EQ(bufferManager.get_active_buffer().name, "Buffer2");

    EXPECT_THROW(bufferManager.set_active_buffer(5), std::out_of_range);
    EXPECT_THROW(bufferManager.set_active_buffer(-1), std::out_of_range);
}

// Test for getting a specific buffer
TEST_F(BufferManagerTest, GetSpecificBuffer) {
    bufferManager.create_buffer("Buffer1");
    bufferManager.create_buffer("Buffer2");

    EXPECT_EQ(bufferManager.get_buffer(0).name, "Buffer1");
    EXPECT_EQ(bufferManager.get_buffer(1).name, "Buffer2");

    EXPECT_THROW(bufferManager.get_buffer(5), std::out_of_range);
    EXPECT_THROW(bufferManager.get_buffer(-1), std::out_of_range);
}

// Test for maximum buffer limit
TEST_F(BufferManagerTest, MaxBufferLimit) {
    bufferManager.create_buffer("Buffer1");
    bufferManager.create_buffer("Buffer2");
    bufferManager.create_buffer("Buffer3");
    bufferManager.create_buffer("Buffer4");

    EXPECT_THROW(bufferManager.create_buffer("Buffer5"), std::runtime_error);
}

// Test for getting a buffer by name
TEST_F(BufferManagerTest, GetBufferByName) {
    bufferManager.create_buffer("Buffer1");
    bufferManager.create_buffer("Buffer2");
    bufferManager.create_buffer("Buffer3");

    // Test for a valid buffer name
    BufferManager::BufferStructure* buffer = bufferManager.get_buffer_by_name("Buffer2");
    ASSERT_NE(buffer, nullptr); // Ensure the buffer is not null
    EXPECT_EQ(buffer->name, "Buffer2"); // Verify the buffer name matches

    // Test for an invalid buffer name
    buffer = bufferManager.get_buffer_by_name("NonExistentBuffer");
    EXPECT_EQ(buffer, nullptr); // Ensure the buffer is null
}

TEST_F(BufferManagerTest, DeleteBuffer) {
    // Create several buffers
    bufferManager.create_buffer("Buffer1");
    bufferManager.create_buffer("Buffer2");
    bufferManager.create_buffer("Buffer3");

    // Check initial buffer count
    EXPECT_EQ(bufferManager.getBufferCount(), 3);

    // Delete the second buffer and check the new buffer count
    bufferManager.delete_buffer(1);  // Deletes "Buffer2"
    EXPECT_EQ(bufferManager.getBufferCount(), 2);

    // Verify remaining buffers' names and active buffer
    EXPECT_EQ(bufferManager.get_buffer(0).name, "Buffer1");
    EXPECT_EQ(bufferManager.get_buffer(1).name, "Buffer3");
    EXPECT_EQ(bufferManager.get_active_buffer().name, "Buffer1");

    // Delete the active buffer and verify that the remaining buffer becomes active
    bufferManager.delete_buffer();  // Deletes "Buffer1" (active buffer)
    EXPECT_EQ(bufferManager.getBufferCount(), 1);
    EXPECT_EQ(bufferManager.get_active_buffer().name, "Buffer3");

    // Test deleting a non-existent buffer (should throw an exception)
    EXPECT_THROW(bufferManager.delete_buffer(5), std::out_of_range);

    // Delete the last buffer and check if the buffer list is empty
    bufferManager.delete_buffer();  // Deletes "Buffer3"
    EXPECT_EQ(bufferManager.getBufferCount(), 0);
}

// Test for interactions between create, delete, previous, and next functions
TEST_F(BufferManagerTest, BufferInteraction) {
    // Step 1: Create buffers and check initial count
    bufferManager.create_buffer("Buffer1");
    bufferManager.create_buffer("Buffer2");
    bufferManager.create_buffer("Buffer3");
    bufferManager.create_buffer("Buffer4");

    EXPECT_EQ(bufferManager.getBufferCount(), 4);

    // Step 2: Navigate to the next buffer and verify the active buffer changes
    bufferManager.next();
    EXPECT_EQ(bufferManager.get_current_buffer_name(), "Buffer2");

    bufferManager.next();
    EXPECT_EQ(bufferManager.get_current_buffer_name(), "Buffer3");

    // Step 3: Delete the current buffer ("Buffer3") and verify buffer count and active buffer
    bufferManager.delete_buffer();  // Deletes "Buffer3"
    EXPECT_EQ(bufferManager.getBufferCount(), 3);
    EXPECT_EQ(bufferManager.get_current_buffer_name(), "Buffer4");  // "Buffer4" should be the new active buffer

    // Step 4: Navigate to the previous buffer and verify the active buffer changes
    bufferManager.previous();
    EXPECT_EQ(bufferManager.get_current_buffer_name(), "Buffer2");

    bufferManager.previous();
    EXPECT_EQ(bufferManager.get_current_buffer_name(), "Buffer1");

    // Step 5: Delete the first buffer ("Buffer1") and check the new active buffer
    bufferManager.delete_buffer(0);  // Deletes "Buffer1"
    EXPECT_EQ(bufferManager.getBufferCount(), 2);
    EXPECT_EQ(bufferManager.get_current_buffer_name(), "Buffer2");

    // Step 6: Attempt to navigate beyond the remaining buffers to verify proper circular navigation
    bufferManager.next();
    EXPECT_EQ(bufferManager.get_current_buffer_name(), "Buffer4");

    bufferManager.next();
    EXPECT_EQ(bufferManager.get_current_buffer_name(), "Buffer2");

    bufferManager.previous();
    EXPECT_EQ(bufferManager.get_current_buffer_name(), "Buffer4");

    // Step 7: Delete all remaining buffers one by one and verify the buffer list is empty
    bufferManager.delete_buffer();  // Deletes "Buffer4"
    EXPECT_EQ(bufferManager.getBufferCount(), 1);
    EXPECT_EQ(bufferManager.get_current_buffer_name(), "Buffer2");

    bufferManager.delete_buffer();  // Deletes "Buffer2"
    EXPECT_EQ(bufferManager.getBufferCount(), 0);
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*
    TEST THIS MANUALLY USING THE COMMAND : 

    g++ -o test_buffer_manager test_bufferManager.cpp ../src/buffer.cpp ../src/cursor.cpp ../include/windowManager.hpp -I include -lgtest -lgtest_main -lpthread -lncurses

*/