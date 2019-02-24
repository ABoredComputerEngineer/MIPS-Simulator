#include <UI/app.hpp>

using std::string;
using std::vector;

struct RowInfo {
     long long line;
     string str;
};

#define ASM_FILE 0
#define BIN_FILE 1

static std::string getExtension( const std::string &s ){
     std::string extension;
     for ( auto iter = s.end(); *iter != '.'; iter-- ){
          extension += *iter; 
     }
     extension += ".";
     return string ( extension.rbegin(), extension.rend() );
}

FileInfo :: FileInfo ( const std::string &s ):filePath(s){
     std::string extension = getExtension( s );
     std::cout << extension << std::endl;
     if ( extension == ".asm" ){
          ext = ASM_FILE;
     } else if ( extension == ".bin" ){
          ext = BIN_FILE;
     }
     std::cout << "Ext " << std::endl;
     std::cout << ext << std::endl;;
}

FileInfo :: FileInfo () :ext(0){}

void FileInfo :: addFile( const std::string &s ){
     filePath = s;
     std::string extension = getExtension( s );
     std::cout << extension << std::endl;
     if ( extension == ".asm" ){
          ext = ASM_FILE;
     } else if ( extension == ".bin" ){
          ext = BIN_FILE;
     }
     std::cout << "Ext " << std::endl;
     std::cout << ext << std::endl;;
}


/*
 * ========================================
 * | THE MAIN WINDOW CONTAINER |
 * ========================================
 */


void MainWindow :: loadBin(std::ifstream &inFile, size_t fsize){
}

void MainWindow :: loadAsm(std::ifstream &inFile, size_t fsize){
     if ( fsize > srcSize ){
          if ( srcBuff ){
               delete []srcBuff;
          }
          srcSize = fsize;
          srcBuff = new char[ srcSize + 1 ];
     } 
     char *str = srcBuff;
     std::string s;
     std::cout << "Print vector " << std::endl;
     while ( getline( inFile , s ) ){
          srcCode.push_back( s );
          std::cout << s << std::endl ;
          snprintf(str,s.length()  + 2, "%s\n", s.c_str() );
          str  += ( s.length() + 1  );
     }
     std::cout<< "Print Buffer" << std::endl;
     std::cout << srcBuff<< std::endl;
}
void MainWindow :: loadFile (){
     std::cout << currentFile.filePath << std::endl;
     std::ifstream inFile( currentFile.filePath, std::ios::binary );
     inFile.seekg( 0, std::ios::end );
     size_t fsize = inFile.tellg();
     inFile.seekg( 0, std::ios::beg ); 
     if ( currentFile.ext == ASM_FILE){
          loadAsm(inFile, fsize);
     }
     inFile.close();
}

void MainWindow :: onBtnOpen( ){
     Gtk::FileChooserDialog  dialog( "Choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN );

     dialog.set_transient_for( *this );
     
     dialog.add_button("_Cancel",Gtk::RESPONSE_CANCEL );
     dialog.add_button("_Open", Gtk::RESPONSE_OK );

     auto filterAsm = Gtk::FileFilter::create();
     filterAsm->set_name(".asm files");
     filterAsm->add_pattern( "*.asm");
     dialog.add_filter( filterAsm );

     auto binFiles= Gtk::FileFilter::create();
     binFiles->set_name(".bin files");
     binFiles->add_pattern( "*.bin");
     dialog.add_filter( binFiles );
     
     int result = dialog.run();
     
     if ( result == Gtk::RESPONSE_OK ){
          srcCode.clear();
          currentFile.addFile( dialog.get_filename() );
          loadFile();
          mainPane.updateSrc( srcCode );
     } else if ( result != Gtk::RESPONSE_CANCEL ){
          std::cerr << "What the fuck happened?" << std::endl;
     }
}
void MainWindow :: onButton2( ){
     long long rowInfo = mainPane.getSelectedLineNumber();
     std::cout << rowInfo << std::endl;
}
void MainWindow :: onButton3( ){
     long long rowInfo = mainPane.getSelectedLineNumber();
     std::cout << rowInfo << std::endl;
}

MainWindow :: MainWindow ():\
          srcBuff( nullptr ),\
          srcSize(0),\
          binBuff( nullptr ),\
          binSize(0),\
          box(Gtk::ORIENTATION_VERTICAL),\
          values { "Fuck","This","Shit" },\
          menu( this, &MainWindow :: onBtnOpen, &MainWindow::onButton2, &MainWindow::onButton3 ),\
          mainPane( srcCode, values ),\
          logs( &mainPane )\
{
     // Set window sizes and such
     set_title("Main Window");
     set_default_size(1000,600);
     set_border_width( 30 );

     add( box ); // Add the main box which contains all the widgets
     // Set box structures
     box.set_spacing( 25 );
     box.set_homogeneous(false);
     box.pack_start( menu, Gtk::PACK_SHRINK );
     box.pack_start( mainPane, Gtk::PACK_SHRINK);
     box.set_homogeneous(false);
     box.set_spacing( 75 );
     box.pack_start( logs, Gtk::PACK_SHRINK );

     box.show_all_children();

     // Connect Buttons of the menu
     menu.connectButtons();
     show_all_children();
}

void MainWindow::addErrorData( const vector < ErrorInfo > &s ){
     logs.addToErrBuff( s );
}

void MainWindow :: memoryData( const char *start, size_t size ){
     logs.addToMemBuff(start,size);
}

MainWindow :: ~MainWindow (){
     if ( srcBuff ){
          delete []srcBuff;
     }
     if ( binBuff ){
          delete []binBuff;
     }
}

#undef ASM_FILE
#undef BIN_FILE


