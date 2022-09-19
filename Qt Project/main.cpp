#include "sudokusolver.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SudokuSolver w;

    w.show();
    w.setUp();

    return a.exec();
}
