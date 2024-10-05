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

