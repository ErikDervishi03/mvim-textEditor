#pragma once

#include <string>
#include <vector>

namespace action {
    namespace modify {
      void insert_letter(int letter);
    };

    namespace movement { 
      void new_line(); 
    }; 
};

class Buffer {
private:
  std::vector<std::string> buffer;
  int size;

public:
  Buffer();
  std::string& operator[](int row); 
  void new_row(std::string row, int pos);
  void del_row(int pos);
  void insert_letter(int row, int pos, char letter);
  void merge_rows(int row1, int row2);
  void delete_letter(int row, int pos);
  void row_append(int row, std::string str);
  void push_back(std::string);
  bool is_void_row(int row);
  void clear();
  std::string slice_row(int row, int pos, int pos2);
  std::string get_string_row(int pointed_row) const;
  int get_number_rows() const;
  std::vector<std::string> get_buffer() const;
  void restore();
  bool is_void() const;
  void swap_rows(int row1, int row2);
};
