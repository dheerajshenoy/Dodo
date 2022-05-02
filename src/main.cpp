#include "Dodo.hpp"


int main(int argc,  char **argv) {
    QApplication a(argc, argv);
	dodo = new Dodo(argc, argv);
    dodo->show();
    a.exec();
}
