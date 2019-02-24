#include <iostream>
#include <fstream>
#include <gtkmm.h>
#include <vector>
#include <string>
#include <unordered_map>
using std::string;
using std::vector;


struct FileInfo {
     enum Extension{
          EXT_BIN,
          EXT_ASM,
     };
     std::string filePath;
     Extension ext;
     FileInfo ();
     FileInfo ( const std::string &s );
     void addFile( const std::string & );
};
namespace Gui {
     const std::string binExt(".bin");
     const std::string asmExt(".asm");
};
std::unordered_map < std::string , FileInfo :: Extension > extensionMap;

struct RowInfo {
     long long line;
     string str;
};

std::string getExtension( const std::string &s ){
     std::string extension;
     for ( auto iter = s.end(); *iter != '.'; iter-- ){
          extension += *iter; 
     }
     return string ( extension.rbegin(), extension.rend() );
}

FileInfo :: FileInfo ( const std::string &s ):filePath(s){
}

FileInfo :: FileInfo (){}

void FileInfo :: addFile( const std::string &s ){
     filePath = s;
     std::string extension = getExtension( s );
     if ( extensionMap.count( extension ) ){ // the extension exists in our supported list
          ext = extensionMap[ extension ];          
     }
}

class MainWindow;
/*
 * ===========================================
 * | BUTTON GRIDS |
 * ===========================================
 */
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

ButtonBox :: ButtonBox(MainWindow *m, voidFunc a, voidFunc b, voidFunc c):\
          parent(m),\
          f1(a),\
          f2(b),\
          f3(c),\
          openBtn("Open File"),\
          btn2("Button 2"),\
          btn3("Button 3")\
{
#define BUTTON_DIM 75 
#define BUTTON_SPACING 20
     openBtn.set_size_request(BUTTON_DIM,BUTTON_DIM);
     btn2.set_size_request(BUTTON_DIM,BUTTON_DIM);
     btn3.set_size_request(BUTTON_DIM,BUTTON_DIM);
     attach(openBtn,1,1,1,1);
     attach(btn2,3,1,1,1);
     attach(btn3,5,1,1,1);
     set_column_spacing(BUTTON_SPACING);
#undef BUTTON_DIM
#undef BUTTON_SPACING
}


/*
 * ========================================
 * | TEXT PANES |
 * ========================================
 */

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

class SourceView : public ScrolledWindowWithColumns<unsigned int,Glib::ustring >{
     public:
          SourceView ();
          SourceView ( vector < string > &s );
          long long getLineNumber();
          void updateRow( int row );
          void newSrc( vector < string > &s );
};

void SourceView :: updateRow ( int row ){
     auto children = refList->children();
     auto iter = children.begin();
     for ( int i = 0; i < row-1; i++ ){
          iter++;
     }
     Gtk::TreePath path ( iter );
     treeView.scroll_to_row( path );
     auto refSelection = treeView.get_selection();
     refSelection->select( *iter );
}

long long SourceView ::getLineNumber() {
     Glib :: RefPtr < Gtk :: TreeSelection > refTreeSelection = treeView.get_selection();
     Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
     auto row = *iter;
     return row[ entry.col0];
}

SourceView :: SourceView () {
     set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
     //set_size_request( 300, 200);
     add( treeView );
     refList = Gtk::ListStore::create( entry );
     treeView.set_model( refList );
     treeView.append_column( "Line", entry.col0 );
     treeView.append_column( "Text", entry.col1 );
}

SourceView :: SourceView ( vector < string > &code ){
     set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
     add( treeView ); 
     refList = Gtk::ListStore::create( entry );
     treeView.set_model( refList );

     // Add Data to the List
     for ( size_t i = 0; i < code.size(); i++ ){
          auto row = *( refList->append() );
          row[ entry.col0 ] = ( i + 1 );
          row[ entry.col1 ] = code[i];
     }

     treeView.append_column( "Line", entry.col0);
     treeView.append_column( "Text", entry.col1);
     show_all_children();
}

void SourceView :: newSrc ( vector < string > &code ){
     refList->clear();
     treeView.set_model( refList );
     for ( size_t i = 0; i < code.size(); i++ ){
          auto row = *( refList->append() );
          row[ entry.col0 ] = ( i + 1 );
          row[ entry.col1 ] = code[i];
     }
}

class RegisterView : public ScrolledWindowWithColumns<Glib::ustring,Glib::ustring> {
     public:
          RegisterView ();
          RegisterView ( vector < string > &values );
};

RegisterView :: RegisterView () {
     set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
     //set_size_request( 300, 200);
     add( treeView );
     refList = Gtk::ListStore::create( entry );
     treeView.set_model( refList );
     treeView.append_column( "Register", entry.col0 );
     treeView.append_column( "Text", entry.col1 );
}


RegisterView :: RegisterView ( vector < string > &values ){
     set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
     add( treeView );
     refList = Gtk::ListStore::create( entry );
     treeView.set_model( refList );

     for ( size_t i = 0; i < values.size(); i++ ){
          auto row = *( refList->append() );
          row[ entry.col1 ] = values[i];
          row[ entry.col0 ] = "Register Name";
     }
     treeView.append_column( "Register" , entry.col0);
     treeView.append_column( "Values", entry.col1);
     show_all_children();
}

class MainPane : public Gtk::Paned{
     private:
          SourceView source;
          RegisterView registers;
     public:
          MainPane ( vector < string >&code, vector < string > &values );
          long long getSelectedLineNumber();
          void updateSrc( vector < string > &code );
          void updateRow (int );
};

void MainPane :: updateRow ( int x ){
     source.updateRow( x );
}

long long MainPane :: getSelectedLineNumber(){
     return source.getLineNumber();
}
void MainPane :: updateSrc( vector < string > &code ){
     source.newSrc( code );
}

MainPane :: MainPane ( vector < string > &code, vector < string > &values ):\
          source( code ),\
          registers( values )\
{
     set_position( 700 );
     set_size_request( 500, 200 );
     add1( source );
     add2( registers ); 
     show_all_children();
}

/*
 * ========================================
 * | LOWER LOG DISPLAY SECTION |
 * ========================================
 */
struct ErrorInfo{
     long long line;
     string errText;
};

class ErrorDisplay : \
          public ScrolledWindowWithColumns<long long,Glib::ustring>
{
     private:
          MainPane *pane;
     public:
          ErrorDisplay ();
          ErrorDisplay ( MainPane *);
          ErrorDisplay ( vector < ErrorInfo > &s );
          void updateBuffer( const vector < ErrorInfo > &s );
          void makeStructure();
          bool onDoubleClick(GdkEventButton *event);
};


bool ErrorDisplay :: onDoubleClick(GdkEventButton *event){
     if ( event->type == GDK_2BUTTON_PRESS ){
          Glib::RefPtr < Gtk::TreeSelection > refSelection = treeView.get_selection();
          auto row = *( refSelection->get_selected() );
          int line = row[ entry.col0 ];
          pane->updateRow( line );
     }
     return false;
}


ErrorDisplay :: ErrorDisplay (MainPane *p): pane(p){
     makeStructure();
}


ErrorDisplay :: ErrorDisplay ():pane(nullptr){
     makeStructure();
}

ErrorDisplay :: ErrorDisplay( vector < ErrorInfo > &s ):pane(nullptr){
     set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
     //set_size_request( 200, 200 );

     add( treeView );
     refList = Gtk:: ListStore :: create( entry );
     treeView.set_model( refList );

     for ( size_t i = 0; i < s.size(); i++ ){
          auto row = *( refList->append() );
          row[ entry.col0 ] = s[i].line;
          row[ entry.col1 ] = s[i].errText;
     }

     treeView.append_column( "Line", entry.col0 );
     treeView.append_column( "Program Text", entry.col1 );
     show_all_children();
}

void ErrorDisplay :: updateBuffer ( const vector <ErrorInfo > &s ){
     // clears the what is currently in the error buffer
     // and add the item to the buffer
     refList->clear(); 
     for ( size_t i = 0; i < s.size(); i++ ){
          auto row = * (refList->append() );
          row[ entry.col0 ] = s[i].line;
          row[ entry.col1 ] = s[i].errText;
     }
     show_all_children();
}



class MemoryDisplay : public Gtk::ScrolledWindow {
     private:
          Glib::RefPtr < Gtk :: TextBuffer >refBuffer1,refBuffer2;
          Gtk::TextView textView;
     public:
          MemoryDisplay ();
          MemoryDisplay ( const char *buff, size_t size );
          void updateBuffer( const char *start, const char *end );
};

MemoryDisplay :: MemoryDisplay(){
     set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
     //set_size_request( 200, 200 );
     
     add( textView );
     textView.set_editable( false );
     refBuffer1 = Gtk::TextBuffer::create();
     refBuffer2 = Gtk::TextBuffer::create();
     refBuffer1->set_text( "" );
     textView.set_buffer( refBuffer1 );

     show_all_children();     
}


MemoryDisplay :: MemoryDisplay( const char *buff, size_t size ){
     set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
     //set_size_request( 200, 200 );
     
     add( textView );
     refBuffer1 = Gtk::TextBuffer::create();
     refBuffer2 = Gtk::TextBuffer::create();
     refBuffer1->set_text( buff, buff + size );
     textView.set_buffer( refBuffer1 );

     show_all_children();     
}

void MemoryDisplay :: updateBuffer( const char *start, const char *end ){
     refBuffer1->set_text( start, end );
     textView.set_buffer( refBuffer1 );
}

class LogDisplay : public Gtk::Notebook {
     private:
          MainPane *pane;
          const char *memBuff;
          size_t size;
          ErrorDisplay errDisplay;
          MemoryDisplay memDisplay;
     public:
          LogDisplay ();
          LogDisplay (MainPane *);
          LogDisplay( const char *buff, size_t buffSize );
          void addToErrBuff( const vector < ErrorInfo > &errs );
          void addToMemBuff( const char *buff, size_t buffSize ); 
};

LogDisplay :: LogDisplay ( MainPane *p ):errDisplay( p ){
     set_size_request( 1000,200 );
     append_page( errDisplay );
     append_page( memDisplay );
     show_all_children();
}

LogDisplay :: LogDisplay ( ){
     //set_size_request( 200,200 );
     append_page( errDisplay );
     append_page( memDisplay );
     show_all_children();
}

void LogDisplay :: addToErrBuff( const vector < ErrorInfo > &errs ){
     errDisplay.updateBuffer( errs );
}

void LogDisplay :: addToMemBuff ( const char *buff, size_t buffSize ){
     memDisplay.updateBuffer( buff, buff + buffSize );
}

/*
 * ========================================
 * | THE MAIN WINDOW CONTAINER |
 * ========================================
 */
class MainWindow : public Gtk::Window {
     private:
          FileInfo currentFile;
          Gtk::Box box;
          Gtk::Button openBtn,button2,button3;
          char *srcBuff; // holds the source code
          size_t srcSize;
          size_t binSize;
          char *binBuff; // holds the binary generated
          vector < string > srcCode;
          vector < string > values;
          ButtonBox menu;
          MainPane mainPane;
          LogDisplay logs;
     public:
          MainWindow ();
          void addErrorData( const vector < ErrorInfo >&s );
          void memoryData( const char *start, size_t size );
          void loadAsm( std::ifstream & );
          void loadBin( std::ifstream & );
          void onBtnOpen();
          void onButton2();
          void onButton3();
          void loadFile(); // loads the file denoted by the currentFile

};


void MainWindow :: loadBin(std::ifstream &inFile){
}

void MainWindow :: loadAsm(std::ifstream &inFile){
     srcBuff = new char[ srcSize + 1 ];
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
//     if ( currentFile.ext == FileInfo :: Extension :: EXT_ASM ){
          srcSize = fsize;
          loadAsm(inFile);
 //    }
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
          box(Gtk::ORIENTATION_VERTICAL),\
          srcCode {"This","is an ", "Example","1","2","3","4","5","76","sadf","asdf" },\
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

void ButtonBox :: connectButtons(){
     openBtn.signal_clicked().connect( sigc::mem_fun( *parent,f1 ) ); 
     btn2.signal_clicked().connect( sigc::mem_fun( *parent,f1 ) ); 
     btn3.signal_clicked().connect( sigc::mem_fun( *parent,f1 ) ); 
}

void ErrorDisplay :: makeStructure(){
     set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
     //set_size_request( 200, 300 );
     add( treeView );
     refList = Gtk::ListStore::create( entry );
     treeView.set_model( refList );
     treeView.append_column( "Line", entry.col0 );
     treeView.append_column( "Error Text", entry.col1 );

     // Add double click handlers
     treeView.signal_button_press_event().connect( sigc::mem_fun( *this, &ErrorDisplay:: onDoubleClick ));

}

int main( int argc, char *argv[] ){
     extensionMap[ Gui::binExt ] = FileInfo::Extension::EXT_BIN;
     extensionMap[ Gui::asmExt ] = FileInfo::Extension::EXT_ASM;
     auto app = Gtk::Application::create( argc, argv, "Example" );
     MainWindow window;
     vector < ErrorInfo > inf = {
          { 1, "Fuck"},
          { 3, "This"},
     };
     string s("Fuuuuck");
     window.addErrorData( inf );
     window.memoryData( s.c_str(), s.size());
     return app->run( window );
}
