#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "crawler.h"
#include "hash.h"
#include "avlTree.h"
#include "util.h"
#include "map.h"
#include <QLabel>
#include <QDesktopServices>
#include <QUrl>
#include <QScrollArea>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QWidget>
#include <QRegExp>
#include <QResizeEvent>
#include <vector>
#include <sstream>

extern AVLTree tree;	// AVL tree inverse document
extern HashDoc hashDoc;	// hash table inverse document
extern Map map;			// word-wordID map

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dic = new HashTable();	 // dictionary for words division
    dic_ex = new HashTable(); // dictionary for exclusion
    dic->appendFrom("dic.txt");
    dic->appendFrom("dic_special.txt");
    dic_ex->appendFrom("dic_exclusion.txt");

    data_vec.assign(URL_NUM+1, Data(0,"","",""));
    char buffer[1024] = { 0 };
    std::ifstream readFile("data.txt");
    try {
        if (readFile.is_open() == true) {

            while (readFile.getline(buffer, sizeof(char) * 1024)) {
                CharString tempID = buffer;
                readFile.getline(buffer, sizeof(char) * 1024);
                CharString url = buffer;
                readFile.getline(buffer, sizeof(char) * 1024);
                CharString title = buffer;
                bool isExist2 = dic->isExist(title);
                readFile.getline(buffer, sizeof(char) * 1024);
                CharString context = buffer;

                int urlID;
                std::string temp = utf8_to_ansi(&tempID);
                if (temp.substr(0, 1) == "?") {
                    urlID = atoi(temp.substr(1,temp.length()-1).c_str());
                }
                else urlID = atoi(temp.c_str());
                Data data(urlID, url, title, context);


                data_vec[urlID] = data;
            }

            readFile.close();
        }
    }
    catch (bool) {}

    std::vector<CharString> resultStr;
    CharString urlArr[URL_NUM + 1] = { "" };
    for (Data data : data_vec) {
        int urlID = data.urlID;
        CharString url = data.url;
        CharString title = data.title;
        CharString context = data.context;

        urlArr[urlID] = url;
        split_words(dic, dic_ex, &resultStr, &title, urlID);
        split_words(dic, dic_ex, &resultStr, &context, urlID);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

//search button clicked
void MainWindow::on_pushButton_clicked()
{
    CharString search_str = ui->lineEdit->text().toStdString();

    std::vector<CharString>* resultStr = new std::vector<CharString>;
    WordNode* node;
    int result[URL_NUM + 1] = { 0 };
    split_words(dic, dic_ex, resultStr, &search_str, 0);

    for (int j = 0; j < resultStr->size(); j++) {
        node = tree.search(map[(*resultStr)[j].getStr()]);
        if (node) {
            DocNode* n = node->doc_list->getHead();
            while (n) {
                result[n->id] += n->occur_count;
                n = n->next;
            }
        }
    }

    QScrollArea *scroll = ui->scrollArea;
    QWidget *central = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(central);
    scroll->setWidget(central);
    scroll->setWidgetResizable(true);

    for (int j = 1; j < URL_NUM + 1; j++) {
        if (result[j] == 0) continue;

        Data data = data_vec[j];

        QWidget *widget = new QWidget;
        QHBoxLayout *hlayout = new QHBoxLayout(widget);
        QLabel *label = new QLabel(QString::number(data.urlID));
        QLabel *label2 = new QLabel(QString(data.url.getStr()));

        // highlight key words
        QString title = QString(data.title.getStr());
        QString context = QString(data.context.getStr());
        for (int j = 0; j < resultStr->size(); j++) {
            title.replace(QRegExp(QString((*resultStr)[j].getStr())), "<i><span style=\"background-color: #FFFF00\">" + QString((*resultStr)[j].getStr()) + "</span></i>");
            context.replace(QRegExp(QString((*resultStr)[j].getStr())), "<i><span style=\"background-color: #FFFF00\">" + QString((*resultStr)[j].getStr()) + "</span></i>");
        }
        QLabel *label3 = new QLabel(title);
        QLabel *label4 = new QLabel(context);

        label2->setOpenExternalLinks(true);
        hlayout->addWidget(label);
        hlayout->addWidget(label2);
        hlayout->addWidget(label3);
        hlayout->addWidget(label4);
        hlayout->addStretch();
        layout->addWidget(widget);
    }

    layout->addStretch();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    ui->scrollArea->setGeometry(0,100,width(),height() - 100);
}
