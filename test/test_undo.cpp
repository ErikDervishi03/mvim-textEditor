#include <gtest/gtest.h>
#include "../include/editor.hpp"
#include "../include/textBuffer.hpp"
#include "../include/cursor.hpp"

// Define a Test Fixture to reset global state before each test
class UndoTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 1. Reset Buffer
        buffer.clear();
        buffer.new_row("", 0); // Start with one empty row

        // 2. Clear History
        while (!editor::action_history.empty()) {
            editor::action_history.pop();
        }

        // 3. Reset Cursor and System Variables
        cursor.set(0, 0);
        pointed_row = 0;
        pointed_col = 0;
        starting_row = 0;
        starting_col = 0;
        
        // Mock screen dimensions to prevent division by zero in logic
        max_row = 50;
        max_col = 80; 
        
        status = Status::saved;
        is_undoing = false;
        
        // Ensure pointed_window is nullptr or safe if possible, 
        // though logic tests shouldn't draw to it.
        pointed_window = nullptr; 
    }

    void TearDown() override {
        // Cleanup if needed
    }
};

// --- Test Cases ---

TEST_F(UndoTest, UndoInsertCharacter) {
    // Action: Type 'a'
    editor::modify::insert_letter('a');
    
    // Assert State After Action
    EXPECT_EQ(buffer[0], "a");
    EXPECT_EQ(pointed_col, 1);
    EXPECT_FALSE(editor::action_history.empty());

    // Undo
    editor::modify::undo();

    // Assert State After Undo
    EXPECT_EQ(buffer[0], ""); // Should be empty again
    EXPECT_EQ(pointed_col, 0); // Cursor should move back
    EXPECT_TRUE(editor::action_history.empty());
}

TEST_F(UndoTest, UndoDeleteCharacter) {
    // Setup: Buffer has "Hi"
    buffer[0] = "Hi";
    pointed_col = 2; // Cursor at end

    // Action: Backspace (delete 'i')
    editor::modify::delete_letter();

    // Assert State
    EXPECT_EQ(buffer[0], "H");
    
    // Undo
    editor::modify::undo();

    // Assert State
    EXPECT_EQ(buffer[0], "Hi"); // 'i' should be restored
    EXPECT_EQ(pointed_col, 2);
}

TEST_F(UndoTest, UndoNewLine) {
    // Setup: "Line1"
    buffer[0] = "Line1";
    pointed_col = 5;

    // Action: Press Enter
    editor::modify::new_line();

    // Assert State
    EXPECT_EQ(buffer.getSize(), 2);
    EXPECT_EQ(buffer[0], "Line1");
    EXPECT_EQ(buffer[1], "");

    // Undo
    editor::modify::undo();

    // Assert State
    EXPECT_EQ(buffer.getSize(), 1); // Should merge back to 1 row
    EXPECT_EQ(buffer[0], "Line1");
}

TEST_F(UndoTest, UndoSplitLine) {
    // Setup: "HelloWorld" with cursor between 'Hello' and 'World'
    buffer[0] = "HelloWorld";
    pointed_col = 5;

    // Action: Press Enter to split
    editor::modify::new_line();

    // Assert State
    EXPECT_EQ(buffer[0], "Hello");
    EXPECT_EQ(buffer[1], "World");

    // Undo
    editor::modify::undo();

    // Assert State
    EXPECT_EQ(buffer.getSize(), 1);
    EXPECT_EQ(buffer[0], "HelloWorld");
}

TEST_F(UndoTest, UndoDeleteRow) {
    // Setup: Two rows
    buffer[0] = "Row1";
    buffer.new_row("Row2", 1);
    pointed_row = 1; // Point to Row2

    // Action: Delete Row2
    editor::modify::delete_row();

    // Assert State
    EXPECT_EQ(buffer.getSize(), 1);
    EXPECT_EQ(buffer[0], "Row1");

    // Undo
    editor::modify::undo();

    // Assert State
    EXPECT_EQ(buffer.getSize(), 2);
    EXPECT_EQ(buffer[1], "Row2"); // Row2 restored
}