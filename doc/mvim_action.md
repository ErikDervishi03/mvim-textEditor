## class Buffer

A class to manage a text buffer for a text editor.  

---

```c++
void del_row (int pos)
```
Deletes a row at the specified position. 

**Parameters**
- `pos` The index of the row to delete. 

---

```c++
void delete_letter (int row, int pos)
```
Deletes a letter at a specified position in a specified row. 

**Parameters**
- `row` The index of the row to modify. 
- `pos` The position within the row to delete the letter. 

---

```c++
stdvector<stdstring> get_buffer ()
```
Retrieves the entire buffer as a vector of strings. 

**Returns:**
- A vector containing all rows of the buffer. 

---

```c++
int get_number_rows ()
```
Gets the current number of rows in the buffer. 

**Returns:**
- The number of rows. 

---

```c++
stdstring get_string_row (int pointed_row)
```
Retrieves the content of a specified row. 

**Parameters**
- `pointed_row` The index of the row to retrieve. 

**Returns:**
- The content of the specified row. 

---

```c++
void insert_letter (int row, int pos, char letter)
```
Inserts a letter at a specific position in a specified row. 

**Parameters**
- `row` The index of the row to modify. 
- `pos` The position within the row to insert the letter. 
- `letter` The character to insert. 

---

```c++
bool is_void ()
```
Checks if the buffer is empty (void). 

**Returns:**
- True if the buffer has no rows or all rows are empty, otherwise false. 

---

```c++
bool is_void_row (int row)
```
Checks if a specified row is empty. 

**Parameters**
- `row` The index of the row to check. 

**Returns:**
- True if the row is empty, otherwise false. 

---

```c++
void merge_rows (int row1, int row2)
```
Merges two rows by appending the content of row2 to row1 and deleting row2. 

**Parameters**
- `row1` The index of the first row. 
- `row2` The index of the second row to merge into the first. 

---

```c++
void new_row (stdstring row, int pos)
```
Inserts a new row at the specified position. 

**Parameters**
- `row` The content of the new row. 
- `pos` The position to insert the new row. 

---

```c++
stdstring& operator [] (int row)
```
Access a specific row in the buffer. 

**Parameters**
- `row` The index of the row to access. 

**Returns:**
- A reference to the specified row. 

---

```c++
void push_back (stdstring str)
```
Adds a new row to the end of the buffer. 

**Parameters**
- `str` The content of the new row to add. 

---

```c++
void row_append (int row, stdstring str)
```
Appends a string to the end of a specified row. 

**Parameters**
- `row` The index of the row to append to. 
- `str` The string to append. 

---

```c++
stdstring slice_row (int row, int pos, int pos2)
```
Slices a portion of a specified row, removing it from the row. 

**Parameters**
- `row` The index of the row to slice. 
- `pos` The starting position of the slice. 
- `pos2` The ending position of the slice. 

**Returns:**
- The sliced string that was removed. 

---

```c++
void swap_rows (int row1, int row2)
```
Swaps the contents of two specified rows. 

**Parameters**
- `row1` The index of the first row. 
- `row2` The index of the second row. 

---

## class Command



---

## class Cursor

A class to represent and manipulate the cursor position in a text editor interface.  

---

```c++
int getX ()
```
Gets the current x-coordinate of the cursor. 

**Returns:**
- The x-coordinate of the cursor. 

---

```c++
int getY ()
```
Gets the current y-coordinate of the cursor. 

**Returns:**
- The y-coordinate of the cursor. 

---

```c++
void restore (int span = 0)
```
Restores the cursor position with an optional horizontal span. This method moves the cursor to its current position, adjusted by a specified span. 

**Parameters**
- `span` An optional parameter to adjust the x position. Default is 0. 

---

```c++
void set (int x, int y)
```
Sets the cursor position to specified x and y coordinates. 

**Parameters**
- `x` The new x-coordinate of the cursor. 
- `y` The new y-coordinate of the cursor. 

---

```c++
void setX (int x)
```
Sets the x-coordinate of the cursor. 

**Parameters**
- `x` The new x-coordinate to set. 

---

```c++
void setY (int y)
```
Sets the y-coordinate of the cursor. 

**Parameters**
- `y` The new y-coordinate to set. 

---

## class Ide



---

## class Screen

A singleton class to manage the screen interface for the text editor.  

---

```c++
void draw_rectangle (int y1, int x1, int y2, int x2)
```
Draws a rectangle on the screen. 

**Parameters**
- `y1` The starting y-coordinate of the rectangle. 
- `x1` The starting x-coordinate of the rectangle. 
- `y2` The ending y-coordinate of the rectangle. 
- `x2` The ending x-coordinate of the rectangle. 

---

```c++
int get_height ()
```
Gets the current height of the screen. 

**Returns:**
- The height of the screen in characters. 

---

```c++
int get_width ()
```
Gets the current width of the screen. 

**Returns:**
- The width of the screen in characters. 

---

```c++
static Screen& getScreen ()
```
Gets the singleton instance of the Screen class. 

**Returns:**
- A reference to the single instance of the Screen. 

---

```c++
Screen& operator= (const Screen &)
```
Deleted assignment operator to prevent assignment. 

**Returns:**
- A reference to the current instance. 

---

```c++
void print_multiline_string (int start_y, int start_x, const char * str)
```
Prints a multiline string starting at specified coordinates. 

**Parameters**
- `start_y` The starting y-coordinate for printing. 
- `start_x` The starting x-coordinate for printing. 
- `str` The string to print. 

---

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

