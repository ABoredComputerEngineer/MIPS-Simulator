#include <UI/main.hpp>

using std::string;
using std::vector;
std::unordered_map < std::string , FileInfo :: Extension > extensionMap;

int main( int argc, char *argv[] ){
     initialize_assembler();
     extensionMap[ Gui::binExt ] = FileInfo::Extension::EXT_BIN;
     extensionMap[ Gui::asmExt ] = FileInfo::Extension::EXT_ASM;
     auto app = Gtk::Application::create( argc, argv, "Example" );
     MainWindow window;
     return app->run( window );
}
