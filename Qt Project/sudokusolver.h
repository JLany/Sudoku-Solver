#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <QMainWindow>

class QPushButton;

QT_BEGIN_NAMESPACE
namespace Ui { class SudokuSolver; }
QT_END_NAMESPACE

class SudokuSolver : public QMainWindow
{
    Q_OBJECT

public:
    SudokuSolver(QWidget *parent = nullptr);
    ~SudokuSolver();

    void setUp();
    bool solvePuzzle();
    bool testSolve();

    void generate_sudoku_puzzle();


private slots:

    void on_generate_clicked();

    void on_solve_clicked();

    void on_dificultyCombo_currentTextChanged(const QString &arg1);

    void on_speedCombo_currentIndexChanged(int index);

private:
    Ui::SudokuSolver *ui;

    QPushButton* buttons[9][9];

    bool isTestSolve;
    unsigned int nInitialCells;
    unsigned int solveDelay;
    unsigned long long nOperations;

    // utility functions
    void initializeRawPuzzle();
    void importRawPuzzle();
    bool inRawPuzzle(int, int);

    void stepSudoku(int, int, char);
    bool isSafe(int, int, int);
    bool validSolve();
    void update();
    void resetBoard();
    void holdScreen(int);

    void setDificulty();
    void setSpeed();
};
#endif // SUDOKUSOLVER_H
