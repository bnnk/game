#ifndef MENU_LOGIC_H
#define MENU_LOGIC_H
#include "menu_button.h"

class menu_logic
{
public:
  menu_logic(unsigned int w_width = 1280, unsigned int w_height = 720,
             std::string button1 = "action", std::string button2 = "about",
             std::string button3 = "quit");

  /// Gets the width of the screen
  unsigned int get_w_width() const noexcept { return m_window_width; }

  /// Gets height of the screen
  unsigned int get_height() const noexcept { return m_window_height; }

  /// Gets the vector of buttons
  std::vector<menu_button> &get_m_v_buttons() noexcept { return m_v_buttons; }

  /// Get one button at index i
  menu_button &get_button(unsigned int index);

  /// Sets the position of the buttons
  /// aligned in the center and equally
  /// distant vertically
  void put_buttons_tidy() noexcept;

private:
  unsigned int m_window_width;
  unsigned int m_window_height;
  std::vector<menu_button> m_v_buttons;

  /// mock button to signal if button that does not exist is called
  menu_button m_mock_button =
      menu_button("INVALID INDEX!"
                  " Trying to get a button that does not exist");
};

void test_menu();

#endif // MENU_LOGIC_H
