#ifndef APP_COMMON_HPP

#define APP_COMMON_HPP
#include <assembler/gen.hpp>
#include <Debugger/debug.hpp>
#include <iostream>
#include <fstream>
#include <gtkmm.h>
#include <vector>
#include <string>
#include <unordered_map>

/*
 * These classes are all forward declaration as a place holder.
 * Their definitions can be found in their respective header files.
 */
class MainWindow;
class ButtonBox;
class MainPane;
class LogDisplay;
template < class T1, class T2 >
class ScrolledWindowWithColumns : public Gtk::ScrolledWindow {
     public:
          class ScrolledWindowColumn : public Gtk::TreeModel::ColumnRecord {
               public:
                    Gtk::TreeModelColumn < T1 > col0;
                    Gtk::TreeModelColumn < T2 > col1;
                    ScrolledWindowColumn () { add( col0 ); add(col1); }
          };
          ScrolledWindowColumn entry;
     protected:
          Glib::RefPtr < Gtk :: ListStore > refList;
          Gtk::TreeView treeView;
     public:
          ScrolledWindowWithColumns (){
          }
};
/*
 * ===========================================
 * | BUTTON GRIDS |
 * ===========================================
 */

struct FileInfo {
     enum Extension{
          EXT_BIN,
          EXT_ASM,
     };
     std::string filePath;
     std::string fileDirectory;
     std::string fileName;
//     Extension ext;
     int ext;
     FileInfo ();
     FileInfo ( const std::string &s );
     void addFile( const std::string & );
     void genFileDirectory();
};

namespace Gui {
     const std::string binExt(".bin");
     const std::string asmExt(".asm");
};
extern std::unordered_map < std::string , FileInfo :: Extension > extensionMap;

#endif

