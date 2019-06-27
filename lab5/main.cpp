#include <iostream>
#include <QApplication>
#include <QPushButton>

int main(int argc, char** argv) {

    //TODO: aggiungere i `.h` in add_executable in cmakelists

    QApplication app(argc, argv);  // inizializzazione di Qt

    QPushButton b ("Hello QT!"); // radice dell'albero
    b.show();  // mostra l'albero(!) delle viste

    return app.exec();  // ciclo dei messaggi

    return 0;
}