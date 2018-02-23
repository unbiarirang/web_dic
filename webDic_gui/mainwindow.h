#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QResizeEvent>
#include <vector>
#include <QUrl>
#include "charString.h"
#include "hash.h"

namespace Ui {
class MainWindow;
}

typedef struct Data {
    int urlID;
    CharString url;
    CharString title;
    CharString context;

    Data(int urlID, CharString urlStr, CharString title, CharString contextStr) {
        this->urlID = urlID;
        this->url = urlStr;
        this->title = title;
        this->context = contextStr;
    }
} Data;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    std::vector<Data> data_vec;

    HashTable *dic;
    HashTable *dic_ex;

    void resizeEvent(QResizeEvent *event);
};

#endif // MAINWINDOW_H
