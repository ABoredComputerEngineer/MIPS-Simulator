#ifndef APP_MAIN_PANE_HPP

#define APP_MAIN_PANE_HPP

#include <UI/common.hpp>

class SourceView : public ScrolledWindowWithColumns<unsigned int,Glib::ustring >{
     public:
          SourceView ();
          SourceView ( std::vector < std::string > &s );
          long long getLineNumber();
          void updateRow( int row );
          void newSrc( std::vector < std::string > &s );
};

class RegisterView : public ScrolledWindowWithColumns<Glib::ustring,Glib::ustring> {
     public:
          RegisterView ();
          RegisterView ( std::vector < std::string > &values );
          void updateRegisters( RegisterInfo * ); // RegisterInfo is defined in debugMachine.hpp
};

class MainPane : public Gtk::Paned{
     private:
          SourceView source;
          RegisterView registers;
     public:
          MainPane ();
          MainPane ( std::vector < std::string >&code, std::vector < std::string > &values );
          long long getSelectedLineNumber();
          void updateSrc( std::vector < std::string > &code );
          void updateRow (int );
          void update( size_t line, RegisterInfo *inf );
};

#endif
