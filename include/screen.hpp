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

    void restore();

    int get_width();

    int get_height();

    void draw_rectangle(int y1, int x1, int y2, int x2);

    void update();

    void print_command();

    void print_buffer();

    void print_multiline_string(int start_y, int start_x, const char *str);

};
