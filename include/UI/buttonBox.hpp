#ifndef APP_BUTTONS_HPP

#define APP_BUTTONS_HPP
#include <UI/common.hpp>
class ButtonBox : public Gtk::Grid {
     private:
          typedef void (MainWindow::*voidFunc)();
          MainWindow *parent;
          voidFunc f1,f2,f3;
          Gtk::Button openBtn,btn2,btn3;
     public:
          ButtonBox (MainWindow *,voidFunc,voidFunc, voidFunc);
          void connectButtons();
};
#endif
