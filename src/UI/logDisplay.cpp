#include <UI/logDisplay.hpp>
#include <UI/app.hpp>
using std::string;
using std::vector;

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

