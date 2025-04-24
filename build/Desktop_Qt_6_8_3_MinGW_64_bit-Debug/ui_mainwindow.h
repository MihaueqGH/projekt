/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *timeLabel;
    QPushButton *showAll;
    QLabel *mainInformation;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *allLayout;
    QVBoxLayout *mainLayout;
    QPushButton *showOffline;
    QPushButton *about;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1300, 803);
        MainWindow->setCursor(QCursor(Qt::CursorShape::ArrowCursor));
        MainWindow->setStyleSheet(QString::fromUtf8("backgound-color:black"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        timeLabel = new QLabel(centralwidget);
        timeLabel->setObjectName("timeLabel");
        timeLabel->setGeometry(QRect(850, 60, 441, 41));
        QFont font;
        font.setFamilies({QString::fromUtf8("Verdana")});
        font.setPointSize(15);
        timeLabel->setFont(font);
        timeLabel->setStyleSheet(QString::fromUtf8("background-color:rgba(0,0,0,100)"));
        timeLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        showAll = new QPushButton(centralwidget);
        showAll->setObjectName("showAll");
        showAll->setGeometry(QRect(10, 60, 251, 41));
        QFont font1;
        font1.setPointSize(12);
        font1.setKerning(true);
        showAll->setFont(font1);
        showAll->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        showAll->setStyleSheet(QString::fromUtf8(""));
        showAll->setAutoDefault(false);
        mainInformation = new QLabel(centralwidget);
        mainInformation->setObjectName("mainInformation");
        mainInformation->setGeometry(QRect(0, 0, 1301, 51));
        mainInformation->setFont(font);
        mainInformation->setStyleSheet(QString::fromUtf8("background-color:rgba(0,0,0,100)"));
        mainInformation->setAlignment(Qt::AlignmentFlag::AlignCenter);
        horizontalLayoutWidget = new QWidget(centralwidget);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(10, 110, 1281, 681));
        allLayout = new QHBoxLayout(horizontalLayoutWidget);
        allLayout->setObjectName("allLayout");
        allLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout = new QVBoxLayout();
        mainLayout->setObjectName("mainLayout");

        allLayout->addLayout(mainLayout);

        showOffline = new QPushButton(centralwidget);
        showOffline->setObjectName("showOffline");
        showOffline->setGeometry(QRect(290, 60, 251, 41));
        showOffline->setFont(font1);
        showOffline->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        showOffline->setStyleSheet(QString::fromUtf8(""));
        showOffline->setAutoDefault(false);
        about = new QPushButton(centralwidget);
        about->setObjectName("about");
        about->setGeometry(QRect(570, 60, 251, 41));
        about->setFont(font1);
        about->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        about->setStyleSheet(QString::fromUtf8(""));
        about->setAutoDefault(false);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        timeLabel->setText(QString());
        showAll->setText(QCoreApplication::translate("MainWindow", "Dost\304\231pne stacje online", nullptr));
        mainInformation->setText(QCoreApplication::translate("MainWindow", "Dane pogodowe - Projekt", nullptr));
        showOffline->setText(QCoreApplication::translate("MainWindow", "Stacje zapisane na urz\304\205dzeniu", nullptr));
        about->setText(QCoreApplication::translate("MainWindow", "O aplikacji", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
