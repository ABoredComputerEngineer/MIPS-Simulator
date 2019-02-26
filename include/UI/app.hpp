#ifndef APP_MAIN_UI_HPP

#define APP_MAIN_UI_HPP
#include <sstream>
#include <assembler/gen.hpp>
#include <Debugger/debug.hpp>
#include <UI/common.hpp>
#include <UI/buttonBox.hpp>
#include <UI/mainPane.hpp>
#include <UI/logDisplay.hpp>

extern std::vector < ErrorInfo > errorList; // defined in common.cpp of assembler
extern strToIndexMap labelMap;  //defined in parse.cpp of assembler
class InputDialog : public Gtk::Dialog {
     public:
          Gtk::Label label1, label2;
          Gtk::Entry entry1;
          Gtk::Entry entry2;
          Gtk::Button btn;
          size_t *s1,*s2;
          InputDialog(size_t *a,size_t *b);
          void onBtnClick();
};

class MainWindow : public Gtk::Window {
     private:
          FileInfo currentFile;
          Gtk::Box box;
          Gtk::Button openBtn,button2,button3;
          char *srcBuff; // holds the source code
          size_t srcSize;
          char *binBuff; // holds the binary generated
          size_t binSize;
          Debugger debug;
          bool executable; // is true if we load the bin file or run the assembler in the asm file
          std::vector < std::string > srcCode;
          std::vector < std::string > values;
          AppendBuffer memBuff;
          size_t memStart, byteCount;
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
          void onBtnStep();
          void onBtnBreak();
          void onBtnContinue();
          void onBtnMemory();
          void onBtnAssemble();
          void loadFile(); // loads the file denoted by the currentFile

};
#endif
