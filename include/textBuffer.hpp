#pragma once

#include <string>
#include <vector>

#pragma once

/**
 * @class Buffer
 * @brief A class to manage a text buffer for a text editor.
 *
 * The Buffer class provides functionalities to manipulate lines of text as rows,
 * allowing for operations such as inserting, deleting, and merging rows, as well
 * as manipulating individual characters within those rows. It serves as a
 * foundational component for text manipulation within a text editor application.
 *
 * The buffer is implemented as a vector of strings, with each string representing
 * a line of text. The class also maintains the size of the buffer to facilitate
 * operations that depend on the number of rows present.
 */
class textBuffer
{
private:
  std::vector<std::string> buffer;   ///< The internal storage for the lines of text.
  int size;   ///< The current number of rows in the buffer.
  int nonEmptyRowCount;

public:
  /**
   * @brief Constructs a new Buffer instance and initializes it with one empty row.
   */
  textBuffer();

  /**
   * @brief Access a specific row in the buffer.
   * @param row The index of the row to access.
   * @return A reference to the specified row.
   */
  std::string& operator [] (int row);

  /**
   * @brief Inserts a new row at the specified position.
   * @param row The content of the new row.
   * @param pos The position to insert the new row.
   */
  void new_row(std::string row, int pos);

  /**
   * @brief Deletes a row at the specified position.
   * @param pos The index of the row to delete.
   */
  void del_row(int pos);

  /**
   * @brief Inserts a letter at a specific position in a specified row.
   * @param row The index of the row to modify.
   * @param pos The position within the row to insert the letter.
   * @param letter The character to insert.
   */
  void insert_letter(int row, int pos, char letter);

  /**
   * @brief Merges two rows by appending the content of row2 to row1 and deleting row2.
   * @param row1 The index of the first row.
   * @param row2 The index of the second row to merge into the first.
   */
  void merge_rows(int row1, int row2);

  /**
   * @brief Deletes a letter at a specified position in a specified row.
   * @param row The index of the row to modify.
   * @param pos The position within the row to delete the letter.
   */
  void delete_letter(int row, int pos);

  /**
   * @brief Appends a string to the end of a specified row.
   * @param row The index of the row to append to.
   * @param str The string to append.
   */
  void row_append(int row, std::string str);

  /**
   * @brief Adds a new row to the end of the buffer.
   * @param str The content of the new row to add.
   */
  void push_back(std::string str);

  /**
   * @brief Checks if a specified row is empty.
   * @param row The index of the row to check.
   * @return True if the row is empty, otherwise false.
   */
  bool is_void_row(int row);

  /**
   * @brief Clears the buffer and resets it to its initial state.
   */
  void clear();

  /**
   * @brief Slices a portion of a specified row, removing it from the row.
   * @param row The index of the row to slice.
   * @param pos The starting position of the slice.
   * @param pos2 The ending position of the slice.
   * @return The sliced string that was removed.
   */
  std::string slice_row(int row, int pos, int pos2);

  /**
   * @brief Retrieves the content of a specified row.
   * @param pointed_row The index of the row to retrieve.
   * @return The content of the specified row.
   */
  std::string get_string_row(int pointed_row) const;

  /**
   * @brief Gets the current number of rows in the buffer.
   * @return The number of rows.
   */
  int getSize() const;

  /**
   * @brief Retrieves the entire buffer as a vector of strings.
   * @return A vector containing all rows of the buffer.
   */
  std::vector<std::string> get_buffer() const;

  /**
   * @brief Restores the buffer to its initial state with one empty row.
   */
  void restore();

  /**
   * @brief Checks if the buffer is empty (void).
   * @return True if the buffer has no rows or all rows are empty, otherwise false.
   */
  bool is_void() const;

  /**
   * @brief Swaps the contents of two specified rows.
   * @param row1 The index of the first row.
   * @param row2 The index of the second row.
   */
  void swap_rows(int row1, int row2);
};
