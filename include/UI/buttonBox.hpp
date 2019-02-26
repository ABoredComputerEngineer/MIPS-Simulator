#ifndef APP_BUTTONS_HPP

#define APP_BUTTONS_HPP
#include <UI/common.hpp>
class ButtonBox : public Gtk::Grid {
     private:
          typedef void (MainWindow::*voidFunc)();
          MainWindow *parent;
          voidFunc f1,f2,f3;
          Gtk::Button openBtn,stepBtn,breakBtn,continueBtn,memBtn;
     public:
          ButtonBox (MainWindow *,voidFunc,voidFunc, voidFunc);
          ButtonBox ( MainWindow * );
          void connectButtons();
};
#endif
