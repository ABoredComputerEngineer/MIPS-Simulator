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
          assert( 0 );
          ext = BIN_FILE;
     }
//     std::cout << "Ext " << std::endl;
//     std::cout << ext << std::endl;;
}

FileInfo :: FileInfo () :ext(0){}

void FileInfo :: addFile( const std::string &s ){
     filePath = s;
     std::string extension = getExtension( s );
     //std::cout << extension << std::endl;
     if ( extension == ".asm" ){
          //std::cout << "Loading Asm file " << std::endl;
          ext = ASM_FILE;
     } else if ( extension == ".bin" ){
          ext = BIN_FILE;
          //std::cout << "Loading bin file" << std::endl;
     }
     genFileDirectory();
}

void FileInfo :: genFileDirectory(){
     char delim = '/';
     auto iter = filePath.end();
     for ( ; iter != filePath.begin() && *iter != delim; iter-- );
     assert( iter != filePath.begin() );
     fileDirectory = std::string ( filePath.begin(), iter );
     iter++;
     auto nameBegin = iter;
     for ( ; iter != filePath.end() && *iter != '.' ; iter++ );
     fileName = std::string ( nameBegin, iter );
     //std::cout << fileDirectory << std::endl;
     //std::cout << fileName << std::endl;
}


/*
 * ========================================
 * | THE MAIN WINDOW CONTAINER |
 * ========================================
 */

static size_t fileSize( std::ifstream &inFile){
     inFile.seekg( 0, std::ios::end );
     size_t x = inFile.tellg();
     inFile.seekg( 0, std::ios::beg );
     return x;
}

void MainWindow :: loadBin(std::ifstream &inFile, size_t fsize){
     if ( fsize > binSize ){
          if ( binBuff ){ delete []binBuff ; }
          binSize = fsize;
          binBuff = new char[ binSize + 1 ];
     }
     inFile.read( binBuff, binSize );
     //std::cout << "Loading program in the machine " << std::endl;
     try{
          debug.loadProgram( binBuff, binSize );
     } catch ( MachineException &m ){
          m.display();
          return ;
     }
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
     //std::cout << "Print vector " << std::endl;
     while ( getline( inFile , s ) ){
          srcCode.push_back( s );
          //std::cout << s << std::endl ;
          snprintf(str,s.length()  + 2, "%s\n", s.c_str() );
          str  += ( s.length() + 1  );
     }
     //std::cout<< "Print Buffer" << std::endl;
     //std::cout << srcBuff<< std::endl;
}


void MainWindow :: loadFile (){
     //std::cout << currentFile.filePath << std::endl;
     std::ifstream inFile( currentFile.filePath, std::ios::binary );
     inFile.seekg( 0, std::ios::end );
     size_t fsize = inFile.tellg();
     inFile.seekg( 0, std::ios::beg ); 
     std::string s = getExtension( currentFile.filePath );
     
     if ( strcmp(s.c_str(),".asm" ) == 0 ){
          executable = false;
          loadAsm(inFile, fsize);
     } else if ( strcmp( s.c_str(),".bin" ) == 0 ){
          executable = true;
          //std::cout << "Loading bin file" << std::endl;
          loadBin( inFile, fsize );
          // load the asm source file
          auto src = debug.getSrcPath();
          //std::cout << src << std::endl;
          std::ifstream asmFile( src, std::ios::binary | std::ios::in );
          size_t size = fileSize( asmFile );
          loadAsm( asmFile, size );
          asmFile.close();
     }
     inFile.close();
     mainPane.updateSrc( srcCode );
     if ( executable ){
     auto x = debug.getLineNumber();
     //std::cout << "Line Number " << x << std::endl;
     mainPane.update( x, nullptr );
     } else {
          mainPane.update( 1, nullptr );
     }
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
     } else if ( result != Gtk::RESPONSE_CANCEL ){
          std::cerr << "What the fuck happened?" << std::endl;
     }
}

void MainWindow :: onBtnStep (){
     //std::cout << executable << std::endl;
     if ( executable ){
          debug.singleStep();
          size_t line = debug.getLineNumber();
          auto registers = debug.getRegisters();
          mainPane.update( line, &registers );
          char *x = debug.getMem( memBuff , memStart,byteCount );
          //std::cout << x << std::endl;
          logs.addToMemBuff( x, memBuff.len );
          //assert( buff.len == strlen( x ) );
          memBuff.clearBuff();
     }
}

void MainWindow :: onBtnBreak(){
     //std::cout << "Breaking" << std::endl;
     long long line = mainPane.getSelectedLineNumber();
     debug.setBreakPoint( line );
}

void MainWindow :: onBtnContinue(){
     debug.continueExecution();
     size_t line = debug.getLineNumber();
     auto registers = debug.getRegisters();
     mainPane.update( line, &registers );
     char *x = debug.getMem( memBuff , memStart ,byteCount );
     logs.addToMemBuff( x, memBuff.len );
     memBuff.clearBuff();
}

void MainWindow :: onBtnMemory (){
     InputDialog dialog(&memStart,&byteCount);
     dialog.set_transient_for( *this );
     dialog.run();
     if ( byteCount > 0 ){
          char *x = debug.getMem( memBuff , memStart ,byteCount );
          //std::cout << x << std::endl;
          logs.addToMemBuff( x, memBuff.len );
//          assert( memBuff.len == strlen( x ) );
          memBuff.clearBuff();
     }
}


void MainWindow :: onBtnAssemble(){
     //std::cout << "Assembling the file " << std::endl;
     labelMap.clear();
     if ( !srcBuff ){
          // std::cout << "Open a file first!" << std::endl;
          return;
     }
     Generator gen(srcBuff, currentFile.filePath.c_str(), true);
     //std::cout << "Parsing the file " << std::endl;
     bool success = gen.parseFile();
     // std::cout << "Parse Success " << std::endl;
     if ( !success  ){
          logs.addToErrBuff( errorList );
          return;
     }
     gen.encode();
     if ( !gen.isGenSuccess() ){
          logs.addToErrBuff( errorList );
          return;
     }
     std::string outFile( currentFile.fileDirectory + "/" + currentFile.fileName + ".bin" );
     try {
         gen.generateFile(outFile.c_str());
     } catch ( Assembler::FileException &f ){
         std:: cerr << f.getError() << std::endl;
         return ;
     }
     // if we successfully generate the executable load the bin file
     std::ifstream binFile( outFile, std::ios::binary|std::ios::in );
     auto size = fileSize( binFile );
     executable = true;
     loadBin( binFile, size ) ;
     auto x = debug.getLineNumber();
     //std::cout << "Line Number " << x << std::endl;
     mainPane.update( x, nullptr );
}
void MainWindow :: onBtnReset (){
     debug.resetExecution();
     size_t line = debug.getLineNumber();
     auto registers = debug.getRegisters();
     mainPane.update( line, &registers );
     char *x = debug.getMem( memBuff , memStart,byteCount );
     //std::cout << x << std::endl;
     logs.addToMemBuff( x, memBuff.len );
     //assert( buff.len == strlen( x ) );
     memBuff.clearBuff();
}

MainWindow :: MainWindow ():\
          srcBuff( nullptr ),\
          srcSize(0),\
          binBuff( nullptr ),\
          binSize(0),\
          executable( false ),\
          box(Gtk::ORIENTATION_VERTICAL),\
          menu( this ),\
          logs( &mainPane ),\
          memStart( 0x4fc ),\
          byteCount( 0x5f0 )\
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


InputDialog :: InputDialog(size_t *a, size_t *b ):label1("Start address"),label2("Number of Bytes"),s1(a),s2(b),btn("Ok"){
     entry1.set_max_length( 50 );
     entry2.set_max_length( 50 );
     Gtk::Box *box = get_vbox();
     box->pack_start( label1 );
     box->pack_start( entry1 );
     box->pack_start( label2 );
     box->pack_start( entry2 );
     box->pack_start( btn ); 
     btn.signal_clicked().connect( sigc::mem_fun( *this, &InputDialog::onBtnClick ) );
     show_all_children();
}

void InputDialog::onBtnClick(){
     std::string text2( entry1.get_text() + " " + entry2.get_text() );
     std::istringstream stream(text2);
     stream.unsetf( std::ios::dec );
     stream.unsetf( std::ios::oct );
     stream.unsetf( std::ios::hex );
     size_t a, b;
     stream >> a ;
     stream >> b;
     if ( !stream ){
          a = 0; b = 0;
     } else {
          *s1 = a; *s2 = b;
     }
     hide();
}

#undef ASM_FILE
#undef BIN_FILE


