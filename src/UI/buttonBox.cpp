#include <UI/buttonBox.hpp>
#include <UI/app.hpp>
ButtonBox :: ButtonBox(MainWindow *m):\
          parent(m),\
          openBtn("Open File"),\
          assembleBtn("Assemble"),\
          stepBtn("Step"),\
          breakBtn("Break"),\
          continueBtn("Continue"),\
          memBtn("Memory")\
{
#define BUTTON_DIM 75 
#define BUTTON_SPACING 20
     openBtn.set_size_request(BUTTON_DIM,BUTTON_DIM);
     stepBtn.set_size_request(BUTTON_DIM,BUTTON_DIM);
     breakBtn.set_size_request(BUTTON_DIM,BUTTON_DIM);
     attach(openBtn,1,1,1,1);
     attach(assembleBtn,3,1,1,1);
     attach(stepBtn,5,1,1,1);
     attach(breakBtn,7,1,1,1);
     attach(continueBtn,9,1,1,1);
     attach(memBtn,11,1,1,1);
     set_column_spacing(BUTTON_SPACING);
#undef BUTTON_DIM
#undef BUTTON_SPACING
}

void ButtonBox :: connectButtons(){
     openBtn.signal_clicked().connect( sigc::mem_fun( *parent, &MainWindow::onBtnOpen) ); 
     assembleBtn.signal_clicked().connect( sigc::mem_fun( *parent, &MainWindow::onBtnAssemble) ); 
     stepBtn.signal_clicked().connect( sigc::mem_fun( *parent, &MainWindow::onBtnStep) ); 
     breakBtn.signal_clicked().connect( sigc::mem_fun( *parent,&MainWindow::onBtnBreak) ); 
     continueBtn.signal_clicked().connect( sigc::mem_fun( *parent,&MainWindow::onBtnContinue) ); 
     memBtn.signal_clicked().connect( sigc::mem_fun( *parent, &MainWindow::onBtnMemory) );
}
