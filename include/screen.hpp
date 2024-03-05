#include "utils.h"

class Screen {
public:
    static Screen& getScreen() {
      static Screen screen_;
      return screen_;
    }
private:

    Screen();

public:
    Screen(const Screen&) = delete;
    Screen& operator=(const Screen&) = delete;

    ~Screen();
    
    void start();

    void update();

    void clear_line(int row);

    void refresh_row(int row);

    void print_row(int row);

    void print_buffer();

    void show_info();
};
