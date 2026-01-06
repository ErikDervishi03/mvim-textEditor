#include "../include/textBuffer.hpp"


textBuffer::textBuffer() : buffer(1, ""), size(1), nonEmptyRowCount(0) 
{
}

std::string& textBuffer::operator [] (int row)
{
  return this->buffer[row];
}

void textBuffer::new_row(std::string row, int pos)
{
  this->buffer.insert(this->buffer.begin() + pos, std::move(row));
  size++;
}

void textBuffer::merge_rows(int row1, int row2)
{
  this->buffer[row1] += std::move(this->buffer[row2]);
  del_row(row2);
}

void textBuffer::del_row(int pos)
{
  if (size == 1)
  {
    this->buffer[0] = std::move(std::string(""));
    return;
  }
  this->buffer.erase(this->buffer.begin() + pos);
  size--;
}

void textBuffer::insert_letter(int row, int pos, char letter)
{
  this->buffer[row].insert(this->buffer[row].begin() + pos, letter);
}


void textBuffer::delete_letter(int row, int pos)
{
  if (this->buffer[row].length() > 0)
  {
    this->buffer[row].erase(this->buffer[row].begin() + pos);
  }
}

void textBuffer::row_append(int row, std::string str)
{
  this->buffer[row] += std::move(str);
}

void textBuffer::push_back(std::string str)
{
  this->buffer.emplace_back(std::move(str));
  size++;
}

void textBuffer::restore()
{
  clear();
  this->buffer.emplace_back("");
  size = 1;
}

void textBuffer::clear()
{
  this->buffer.clear();
  size = 0;
}

bool textBuffer::is_void_row(int row)
{
  return row >= size && this->buffer[row] == "";
}

std::string textBuffer::slice_row(int row, int pos, int pos2)
{
  std::string to_del = this->buffer[row].substr(pos, pos2 - pos);
  this->buffer[row].erase(pos, pos2 - pos);
  return to_del;
}

std::string textBuffer::get_string_row(int row) const
{
  return this->buffer.at(row);
}

int textBuffer::getSize() const
{
  return size;
}

bool textBuffer::is_void() const
{
  // If the this->buffer size is 0, it's considered void
  if (size == 0)
  {
    return true;
  }

  // Check if every row in the this->buffer is empty
  for (const std::string& row : this->buffer)
  {
    if (!row.empty())
    {
      return false;
    }
  }
  return true;    // Return true if all rows are empty
}

void textBuffer::swap_rows(int row1, int row2)
{
  std::swap(this->buffer[row1], this->buffer[row2]);
}

const std::deque<std::string>& textBuffer::get_buffer() const
{
  return this->buffer;
}