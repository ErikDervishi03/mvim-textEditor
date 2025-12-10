#include <gtest/gtest.h>
#include "../include/editor.hpp"
#include "../include/textBuffer.hpp"
#include "../include/cursor.hpp"

// Define a Test Fixture to reset global state before each test
class UndoExtendedTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 1. Reset Buffer
        buffer.clear();
        buffer.new_row("", 0); 

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
        
        // Default Mock dimensions (Standard Terminal)
        max_row = 20;
        max_col = 20; 
        
        status = Status::saved;
        is_undoing = false;
        
        pointed_window = nullptr; 
    }
};

// --- SCENARIO 1: Horizontal Scrolling (starting_col > 0) ---
TEST_F(UndoExtendedTest, UndoHorizontalScroll) {
    // Setup a small screen width to force scrolling easily
    max_col = 5; 
    
    // 1. Type enough characters to scroll right
    // "0123456789" (Length 10). On screen width 5, starting_col should shift.
    for (int i = 0; i < 10; i++) {
        editor::modify::insert_letter('0' + i);
    }

    // Verify we scrolled
    EXPECT_EQ(buffer[0], "0123456789");
    EXPECT_EQ(pointed_col, 10);
    EXPECT_GT(starting_col, 0); // starting_col should be > 0 (likely around 5 or 6 depending on logic)

    // 2. Undo the last character ('9')
    editor::modify::undo();

    // Verify content and cursor
    EXPECT_EQ(buffer[0], "012345678");
    EXPECT_EQ(pointed_col, 9);
    
    // 3. Undo until we scroll back to the start (Undo 5 more times)
    for (int i = 0; i < 5; i++) {
        editor::modify::undo();
    }
    
    // pointed_col should be 4 ('0123'). max_col is 5.
    // starting_col should theoretically be 0 now as it fits in view
    EXPECT_EQ(buffer[0], "0123");
    EXPECT_EQ(pointed_col, 4);
    EXPECT_EQ(starting_col, 0);
}

// --- SCENARIO 2: Vertical Scrolling (starting_row > 0) ---
TEST_F(UndoExtendedTest, UndoVerticalScroll) {
    max_row = 5; // Small height

    // 1. Create 10 lines
    for (int i = 0; i < 10; i++) {
        editor::modify::new_line();
    }
    
    EXPECT_EQ(buffer.getSize(), 11); // 0-10
    EXPECT_EQ(pointed_row, 10);
    EXPECT_GT(starting_row, 0); // Should have scrolled down

    // 2. Undo the newlines until we are back at top
    for (int i = 0; i < 10; i++) {
        editor::modify::undo();
    }

    EXPECT_EQ(buffer.getSize(), 1);
    EXPECT_EQ(pointed_row, 0);
    EXPECT_EQ(starting_row, 0);
}

// --- SCENARIO 3: Undo "Paste" (Larger Text Block) ---
TEST_F(UndoExtendedTest, UndoLargePaste) {
    // Current implementation of paste() performs InsertChar/NewLine in a loop.
    // Undo must be called N times to reverse N characters.
    
    std::string text_to_paste = "Hello\nWorld";
    copy_paste_buffer = text_to_paste;

    // 1. Execute Paste
    editor::modify::paste();

    // Verify Paste Result
    // Row 0: "Hello"
    // Row 1: "World"
    EXPECT_EQ(buffer[0], "Hello");
    EXPECT_EQ(buffer[1], "World");
    EXPECT_EQ(buffer.getSize(), 2);

    // 2. Undo "World" (5 chars)
    for (int i = 0; i < 5; i++) {
        editor::modify::undo();
    }
    EXPECT_EQ(buffer[1], ""); // Row 1 exists but is empty
    EXPECT_EQ(pointed_row, 1);

    // 3. Undo Newline (Merge)
    editor::modify::undo();
    EXPECT_EQ(buffer.getSize(), 1);
    EXPECT_EQ(buffer[0], "Hello");

    // 4. Undo "Hello" (5 chars)
    for (int i = 0; i < 5; i++) {
        editor::modify::undo();
    }

    // Verify completely clean state
    EXPECT_EQ(buffer[0], "");
    EXPECT_EQ(buffer.getSize(), 1);
    EXPECT_TRUE(editor::action_history.empty());
}

// --- SCENARIO 4: Complex Merge/Split edge cases ---
TEST_F(UndoExtendedTest, UndoMergeAndSplitMidLine) {
    // Setup: "AB"
    editor::modify::insert_letter('A');
    editor::modify::insert_letter('B');
    
    // Move cursor between A and B
    editor::movement::move_left(); // Cursor at 'B' (index 1)

    // 1. Split Line: "A", "B"
    editor::modify::new_line(); 
    EXPECT_EQ(buffer[0], "A");
    EXPECT_EQ(buffer[1], "B");

    // 2. Undo Split (Expect "AB")
    editor::modify::undo();
    EXPECT_EQ(buffer[0], "AB");
    EXPECT_EQ(buffer.getSize(), 1);
    EXPECT_EQ(pointed_col, 1); // Cursor should be back between A and B

    // 3. Delete 'A' (Backspace) -> "B"
    editor::modify::delete_letter();
    EXPECT_EQ(buffer[0], "B");

    // 4. Undo Delete (Expect "AB")
    editor::modify::undo();
    EXPECT_EQ(buffer[0], "AB");
}

// --- SCENARIO 5: Recursive Row Deletion ---
TEST_F(UndoExtendedTest, UndoRecursiveRowDeletion) {
    // Create 3 lines: "1", "2", "3"
    buffer[0] = "1";
    buffer.new_row("2", 1);
    buffer.new_row("3", 2);
    pointed_row = 0;

    // 1. Delete Row 1 ("1") -> Buffer becomes "2", "3"
    editor::modify::delete_row(); 
    EXPECT_EQ(buffer[0], "2");
    EXPECT_EQ(buffer[1], "3");

    // 2. Delete Row 1 again (Now "2") -> Buffer becomes "3"
    editor::modify::delete_row();
    EXPECT_EQ(buffer[0], "3");

    // 3. Undo (Restore "2")
    editor::modify::undo();
    EXPECT_EQ(buffer[0], "2");
    EXPECT_EQ(buffer[1], "3");

    // 4. Undo (Restore "1")
    editor::modify::undo();
    EXPECT_EQ(buffer[0], "1");
    EXPECT_EQ(buffer[1], "2");
    EXPECT_EQ(buffer[2], "3");
}

// --- SCENARIO 6: Empty Buffer Handling ---
TEST_F(UndoExtendedTest, UndoOnEmptyHistory) {
    // Ensure no crash on empty undo
    EXPECT_NO_THROW(editor::modify::undo());
    
    editor::modify::insert_letter('x');
    editor::modify::undo();
    
    // Should be empty again
    EXPECT_EQ(buffer[0], "");
    
    // Undo again on empty history
    EXPECT_NO_THROW(editor::modify::undo());
}