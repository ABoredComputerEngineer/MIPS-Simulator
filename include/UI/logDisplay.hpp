#ifndef APP_LOG_DISPLAY_HPP

#define APP_LOG_DISPLAY_HPP
#include <UI/common.hpp>
class ErrorDisplay : \
          public ScrolledWindowWithColumns<long long,Glib::ustring>
{
     private:
          MainPane *pane;
     public:
          ErrorDisplay ();
          ErrorDisplay ( MainPane *);
          ErrorDisplay ( std::vector < ErrorInfo > &s );
          void updateBuffer( const std::vector < ErrorInfo > &s );
          void makeStructure();
          bool onDoubleClick(GdkEventButton *event);
};

class MemoryDisplay : public Gtk::ScrolledWindow {
     private:
          Glib::RefPtr < Gtk :: TextBuffer >refBuffer1,refBuffer2;
          Gtk::TextView textView;
     public:
          MemoryDisplay ();
          MemoryDisplay ( const char *buff, size_t size );
          void updateBuffer( const char *start, const char *end );
};


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
          void addToErrBuff( const std::vector < ErrorInfo > &errs );
          void addToMemBuff( const char *buff, size_t buffSize ); 
};

#endif
