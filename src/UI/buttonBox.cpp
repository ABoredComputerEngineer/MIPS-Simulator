#include <UI/buttonBox.hpp>
#include <UI/app.hpp>
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

void ButtonBox :: connectButtons(){
     openBtn.signal_clicked().connect( sigc::mem_fun( *parent,f1 ) ); 
     btn2.signal_clicked().connect( sigc::mem_fun( *parent,f1 ) ); 
     btn3.signal_clicked().connect( sigc::mem_fun( *parent,f1 ) ); 
}
