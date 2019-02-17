
int main( int argc, char *argv[] ){
     if ( argc < 2 ){
          std::cout << "Input a file!" << std::endl;
          return -1; 
     }
     size_t size;
     char *buff; 
     Debugger debug;
     try{
          buff = Debug::loadFile( argv[1], &size );
          debug.loadProgram(buff,size);
     } catch ( OpenException &e ){
          std::cerr << "Error when opening the file" << std::endl;
          e.display();
     } catch ( InvalidFileError &f ){
          f.display();
     } catch ( MachineException &m ){
          m.display();
          return -1;
     }
     runDebugger( debug );
     delete []buff;
     return 0;
}
