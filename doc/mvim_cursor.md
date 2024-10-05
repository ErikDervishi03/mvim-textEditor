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

###### API documentation generated using [Doxygenmd](https://github.com/d99kris/doxygenmd)

