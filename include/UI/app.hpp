#ifndef APP_MAIN_UI_HPP

#define APP_MAIN_UI_HPP
#include <UI/common.hpp>
#include <UI/buttonBox.hpp>
#include <UI/mainPane.hpp>
#include <UI/logDisplay.hpp>
class MainWindow : public Gtk::Window {
     private:
          FileInfo currentFile;
          Gtk::Box box;
          Gtk::Button openBtn,button2,button3;
          char *srcBuff; // holds the source code
          size_t srcSize;
          char *binBuff; // holds the binary generated
          size_t binSize;
          std::vector < std::string > srcCode;
          std::vector < std::string > values;
          ButtonBox menu;
          MainPane mainPane;
          LogDisplay logs;
     public:
          MainWindow ();
          virtual ~MainWindow();
          void addErrorData( const std::vector < ErrorInfo >&s );
          void memoryData( const char *start, size_t size );
          void loadAsm( std::ifstream &, size_t fsize );
          void loadBin( std::ifstream &, size_t fsize );
          void onBtnOpen();
          void onButton2();
          void onButton3();
          void loadFile(); // loads the file denoted by the currentFile

};
#endif
