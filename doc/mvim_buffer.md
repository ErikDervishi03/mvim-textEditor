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

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

