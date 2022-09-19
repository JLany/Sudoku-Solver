#include "sudokusolver.h"
#include "ui_sudokusolver.h"
#include <QPushButton>
#include <QGridLayout>
#include <QButtonGroup>
#include <QString>
#include <QThread>
#include <QTime>
#include <random>
#include <ctime>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>


int rawBoard[9][9]{};

int board[9][9]{ {0,0,0,0,5,6,0,9,0},
                 {0,5,0,4,0,0,0,0,0},
                 {0,6,0,3,8,0,0,4,5},
                 {0,0,6,0,0,0,0,0,0},
                 {0,0,0,0,0,0,0,2,0},
                 {3,0,0,0,1,0,8,0,0},
                 {0,0,0,0,4,0,7,0,8},
                 {8,0,0,7,0,3,2,6,0},
                 {0,0,7,8,0,5,0,0,4} };


SudokuSolver::SudokuSolver(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SudokuSolver)
    , isTestSolve{ false }
    , nInitialCells{ 30 } // default value
    , solveDelay{ 30 } // default value
    , nOperations{ 0 }
{
    ui->setupUi(this);

    ui->solve->setEnabled(false);
}

SudokuSolver::~SudokuSolver()
{
    delete ui;
}

void SudokuSolver::setUp() {
    int i, j;
    for (i = 0; i < 9; ++i) {
        for (j = 0; j < 9; ++j) {
            buttons[i][j] = new QPushButton();
            buttons[i][j]->setMinimumSize(40, 40);
            buttons[i][j]->setMaximumSize(40, 40);
            ui->boardLayout->addWidget(buttons[i][j], i, j);
        }
    }

    setFixedSize(size());
}

bool SudokuSolver::solvePuzzle() {
    isTestSolve = false;

    initializeRawPuzzle();
    stepSudoku(0, 0, '+');
    if (validSolve())
        return true;

    return false;
}

bool SudokuSolver::testSolve() {
    isTestSolve = true;

    initializeRawPuzzle();
    stepSudoku(0, 0, '+');
    if (validSolve()) {
        importRawPuzzle();
        return true;
//        ui->counterLbl->setText("Pssed");
    }

//    ui->counterLbl->setText("Failed");

    return false;
}

void SudokuSolver::generate_sudoku_puzzle() {
    static std::default_random_engine engine{ static_cast<unsigned int>(time(0)) };
    static std::uniform_int_distribution<unsigned int> randomValue{ 1, 9 };
    static std::uniform_int_distribution<unsigned int> randomIndex{ 0, 8 };

    resetBoard(); // make sure board is empty

    for (unsigned int i{ 0 }; i < nInitialCells; ++i) {
        int row = randomIndex(engine);
        int col = randomIndex(engine);

        if (board[row][col] != 0) { // retry iteration
            // --i;
            continue;
        }

        int value = randomValue(engine);

        int counter = 0;
        while (!isSafe(value, row, col)) {
            value = randomValue(engine);
            ++counter;
            if (counter > 9)
                break;
        }
        if (counter == 9) { // retry iteration
            // --i;
            continue;
        }

        board[row][col] = value;
    }

    update();
    holdScreen(30);

    if (!testSolve())
        return generate_sudoku_puzzle();


}

bool SudokuSolver::isSafe(int newMove, int row, int col) {
    for (int i = 0; i < 9; ++i)
        if (newMove == board[row][i] && i != col) {
            return false;
        }

    for (int i = 0; i < 9; ++i)
        if (newMove == board[i][col] && i != row) {
            return false;
        }

    for (int i = (row / 3) * 3; i < (row / 3) * 3 + 3; ++i)
        for (int j = (col / 3) * 3; j < (col / 3) * 3 + 3; ++j)
            if (newMove == board[i][j] && !(i == row && j == col)) {
                return false;
            }

    return true;
}

bool SudokuSolver::validSolve() {
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col)
            if (!isSafe(board[row][col], row, col) || board[row][col] == 0)
                return false;
    }

    return true;
}

void SudokuSolver::stepSudoku(int newRw, int newCol, char sign) {
    if (!validSolve()) {
        if (newCol == 9) {
            newCol = 0;
            ++newRw;
        }
        if (newCol < 0) {
            --newRw;
            newCol = 0;
        }
        if (newRw < 0)
            return;
        if (newRw == 9) {
            return;
        }
    }
    else {
        return;
    }

    if (sign == '-') {
        if (inRawPuzzle(newRw, newCol)) {
            return stepSudoku(newRw, newCol - 1, '-');
        }
    }
    else if (sign == '+')
        if (inRawPuzzle(newRw, newCol)) {
            return stepSudoku(newRw, newCol + 1, '+');
        }

    if (board[newRw][newCol] != 0
            && isSafe(board[newRw][newCol], newRw, newCol)) {
        if (!validSolve())
            stepSudoku(newRw, newCol + 1, '+');
    }
    else {
        for (int number = 1; number <= 9; ++number) {
            if (isSafe(number, newRw, newCol)) {
                board[newRw][newCol] = number;

                if (!isTestSolve && solveDelay != 0) {
                    ++nOperations;
                    holdScreen(solveDelay);
                    update();
                }

                if (!validSolve())
                    stepSudoku(newRw, newCol + 1, '+');
                if (validSolve()) {
                    return;
                }
            }
        }

        if (!validSolve()) {
            board[newRw][newCol] = 0;
            if (!isTestSolve) {
                ++nOperations;
                holdScreen(solveDelay);
                update();
            }
        }

        return;
    }

    if (!validSolve())
        return stepSudoku(newRw, newCol - 1, '-');
}

void SudokuSolver::resetBoard() {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            board[i][j] = 0;
}

bool SudokuSolver::inRawPuzzle(int row, int col) {
    if (rawBoard[row][col] != 0)
        return true;
    else
        return false;
}

void SudokuSolver::initializeRawPuzzle() {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            rawBoard[i][j] = board[i][j];
}

void SudokuSolver::importRawPuzzle() {
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            board[i][j] = rawBoard[i][j];
}

void SudokuSolver::update() {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            if (board[i][j] == 0) {
                buttons[i][j]->setText(QString(""));
                continue;
            }

            buttons[i][j]->setText(QString::number(board[i][j]));
        }
    }

    if (nOperations != 0)
        ui->counterLbl->setText(QString::number(nOperations));
}

void SudokuSolver::holdScreen(int dura) {
    QTime dieTime = QTime::currentTime().addMSecs( dura );
    while( QTime::currentTime() < dieTime )
       {
           QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
       }
}

void SudokuSolver::setDificulty() {
    QString txt = ui->dificultyCombo->currentText();

    if (txt == "Easy")
        nInitialCells = 60;
    if (txt == "Normal")
        nInitialCells = 50;
    if (txt == "Hard")
        nInitialCells = 40;
    if (txt == "Extreme (Default)")
        nInitialCells = 30;
    if (txt == "Imposible")
        nInitialCells = 15;
    if (txt == "ARE YOU SURE?")
        nInitialCells = 7;
}

void SudokuSolver::setSpeed() {
    int index = ui->speedCombo->currentIndex();

    switch(index) {
    case 1:
        solveDelay = 30;
        break;
    case 2:
        solveDelay = 20;
        break;
    case 3:
        solveDelay = 10;
        break;
    case 4:
        solveDelay = 5;
        break;
    case 5:
        solveDelay = 1;
        break;
    default:
        break;
    }
}


// ******************************** slots ********************************

void SudokuSolver::on_generate_clicked()
{
    ui->generate->setEnabled(false);
    ui->solve->setEnabled(false);

    nOperations = 0;
    ui->counterLbl->setText(QString(""));

    setDificulty();
    generate_sudoku_puzzle();

    ui->solve->setEnabled(true);
    ui->generate->setEnabled(true);
}


void SudokuSolver::on_solve_clicked()
{
    ui->solve->setEnabled(false);
    ui->generate->setEnabled(false);

    setSpeed();
    solvePuzzle();

    ui->generate->setEnabled(true);
    ui->solve->setEnabled(true);
}


void SudokuSolver::on_dificultyCombo_currentTextChanged(const QString &arg1)
{
    setDificulty();
}


void SudokuSolver::on_speedCombo_currentIndexChanged(int index)
{
    setSpeed();
}

