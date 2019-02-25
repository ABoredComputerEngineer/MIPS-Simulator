#include <UI/mainPane.hpp>
#include <UI/app.hpp>
using std::vector;
using std::string;

#define REGISTER_COUNT 32

enum Dimensions {
     PANE_HEIGHT = 200,
     PANE_WIDTH = 500,
     RIGHT_WIDTH = 700
};
static const std::string registerNames[] = {
    "$zero",
    "$at",
    "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9",
    "$k0", "$k1",
    "$gp", "$sp", "$fp", "$ra",
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
     set_position( RIGHT_WIDTH );
     set_size_request( PANE_WIDTH , PANE_HEIGHT );
     add1( source );
     add2( registers ); 
     show_all_children();
}

MainPane :: MainPane (){
     std::cout << "Calling MainPane Empty constructor" << std::endl;
     set_position( RIGHT_WIDTH );
     set_size_request( PANE_WIDTH , PANE_HEIGHT );

     add1( source );
     add2( registers );
     show_all_children();
}

void MainPane::update( size_t line , RegisterInfo *inf ){
     source.updateRow( line );
     registers.updateRegisters( inf );
}

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
     std::cout << "Calling SourceView Empty constructor" << std::endl;
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


RegisterView :: RegisterView () {
     std::cout << "Calling RegisterView Empty constructor" << std::endl;
     char buff[32];
     set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
     //set_size_request( 300, 200);
     add( treeView );
     refList = Gtk::ListStore::create( entry );
     treeView.set_model( refList );
     
     std::cout << "Empty initializing registers" << std::endl;     
     for ( size_t i = 0; i < REGISTER_COUNT ; i++ ){
          auto row = *( refList->append() );
          snprintf(buff,32,"0x%08x",0 ); 
          row[ entry.col0 ] = registerNames [ i ] ;
          row[ entry.col1 ] = std::string ( buff );
     }

     treeView.append_column( "Register", entry.col0 );
     treeView.append_column( "Value", entry.col1 );
     show_all_children();
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

void RegisterView::updateRegisters( RegisterInfo *regs ){
     Word *reg = reinterpret_cast< Word *>(regs);
     char buff[ 32 ];
     // Get the children row of the treeModel ( refList )
     auto children = refList->children();

     // iterate over the rows and update all the values
     for ( auto iter = children.begin(); iter != children.end(); iter++ ){
          snprintf(buff,32,"0x%08x",*reg);
          Gtk::TreeModel::Row row = *iter;
          row[entry.col1] = std::string ( buff );
          reg++;
     }
}



/*
 * ========================================
 * | LOWER LOG DISPLAY SECTION |
 * ========================================
 */


bool ErrorDisplay :: onDoubleClick(GdkEventButton *event){
     if ( event->type == GDK_2BUTTON_PRESS ){
          Glib::RefPtr < Gtk::TreeSelection > refSelection = treeView.get_selection();
          auto row = *( refSelection->get_selected() );
          int line = row[ entry.col0 ];
          pane->updateRow( line );
     }
     return false;
}
