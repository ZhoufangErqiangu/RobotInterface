#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <windows.h>
#include <QAxBase>
#include <QAxObject>
#include <QFile>
#include <QTextStream>
#include <QVariant>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    //void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;

    QAxObject *FRRJIF;
    QAxObject *DataTable;
    QAxObject *DataNumReg;
    QAxObject *DataPosReg;
    bool FRRJIFConnected;

    void FRRJIFInit();    
    void FRRJIFClose();
};

#endif // MAINWINDOW_H
