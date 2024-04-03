#include "buffer.hpp"

Buffer::Buffer() {
  buffer.reserve(10000);
  buffer.emplace_back("");
  size = 1;
}

void Buffer::new_row(std::string row, int pos) {
  buffer.insert(buffer.begin() + pos, std::move(row));
  size++;
}

void Buffer::merge_rows(int row1, int row2) {
  buffer[row1] += std::move(buffer[row2]);
  del_row(row2);
}

void Buffer::del_row(int pos) {
  if(size == 1){
    buffer[0]=std::move(std::string(""));
    return;
  }
  buffer.erase(buffer.begin() + pos);
  size--;
}

void Buffer::insert_letter(int row, int pos, char letter) {
  buffer[row].insert(buffer[row].begin() + pos, letter);
}

void Buffer::delete_letter(int row, int pos) {
  if(buffer[row].length() > 0)
    buffer[row].erase(buffer[row].begin() + pos);
}

void Buffer::row_append(int row, std::string str) {
  buffer[row] += std::move(str);
}

void Buffer::push_back(std::string str){
  buffer.emplace_back(std::move(str));
  size++;
}

void Buffer::restore(){
  clear();
  buffer.reserve(10000);
  buffer.emplace_back("");
  size = 1;
}

void Buffer::clear(){
  buffer.clear();
  size = 0;
}

bool Buffer::is_void_row(int row){
  return row >= size && buffer[row] == "";
}

std::string Buffer::slice_row(int row, int pos, int pos2) {
  std::string to_del = buffer[row].substr(pos, pos2);
  buffer[row].erase(pos, pos2);
  return to_del;
}

std::string Buffer::get_string_row(int row) const {
  return buffer.at(row);
}

int Buffer::get_number_rows() const {
  return size;
}

std::vector<std::string> Buffer::get_buffer() const {
  return buffer;
}

  
