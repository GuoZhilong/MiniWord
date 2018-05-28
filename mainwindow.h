#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMessageBox>
#include <QMainWindow>
#include <dialogopen.h>
#include <d_replace.h>
#include <d_new.h>
#include <d_save.h>
#include <d_find.h>
#include <datastruct.h>
#include <QMouseEvent>
#include <QTimer>
#include <QTime>
#include <QLabel>
#include <QScrollArea>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_actionOpen_triggered();
    void receivefilename(QString filename);
    void receiveNewfilename(QString filename);
    void receiveSavefilename(QString filename);
    void on_actionreplace_triggered();
    void f_replace(QString s1,QString s2);
    void f_find(QString s);
    void graphic_datastruct();
  //  void wheelEvent(QWheelEvent *event);
    void on_actionexit_triggered();
    void on_actionNew_triggered();
    void on_actionSave_triggered();
//
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void on_actioninstruction_triggered();
    void highlightChunk(Cursor cursor1,Cursor crusor2);
    void on_actionfind_triggered();

private:
    Ui::MainWindow *ui;
    Text* t;//主数据结构的指针
    QLabel **label;
    QScrollArea * scrollArea;
    QWidget * screen;
    QLabel * cursor;
    int labelnum;
    int state;
    bool pressflag;
    QLabel * cursor_2;
protected:
  //  void mouseMoveEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
