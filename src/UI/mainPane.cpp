#include <UI/mainPane.hpp>
#include <UI/app.hpp>
using std::vector;
using std::string;
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
