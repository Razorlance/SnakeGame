/********************************************************************************
** Form generated from reading UI file 'snakebot.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SNAKEBOT_H
#define UI_SNAKEBOT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SnakeBot
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QLabel *timerLabel;
    QLabel *player1Label;
    QLabel *player2Label;
    QLabel *player3Label;
    QLabel *player4Label;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *SnakeBot)
    {
        if (SnakeBot->objectName().isEmpty())
            SnakeBot->setObjectName("SnakeBot");
        SnakeBot->setEnabled(true);
        SnakeBot->resize(630, 630);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(SnakeBot->sizePolicy().hasHeightForWidth());
        SnakeBot->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(SnakeBot);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout_4 = new QHBoxLayout(centralwidget);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        frame = new QFrame(centralwidget);
        frame->setObjectName("frame");
        QFont font;
        font.setPointSize(21);
        font.setBold(true);
        frame->setFont(font);
        frame->setFrameShape(QFrame::VLine);
        frame->setFrameShadow(QFrame::Raised);

        horizontalLayout_4->addWidget(frame);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        timerLabel = new QLabel(centralwidget);
        timerLabel->setObjectName("timerLabel");
        QFont font1;
        font1.setFamilies({QString::fromUtf8("Copperplate")});
        font1.setPointSize(21);
        timerLabel->setFont(font1);

        verticalLayout_2->addWidget(timerLabel);

        player1Label = new QLabel(centralwidget);
        player1Label->setObjectName("player1Label");
        QFont font2;
        font2.setPointSize(19);
        font2.setBold(true);
        player1Label->setFont(font2);

        verticalLayout_2->addWidget(player1Label);

        player2Label = new QLabel(centralwidget);
        player2Label->setObjectName("player2Label");
        player2Label->setFont(font2);

        verticalLayout_2->addWidget(player2Label);

        player3Label = new QLabel(centralwidget);
        player3Label->setObjectName("player3Label");
        player3Label->setFont(font2);

        verticalLayout_2->addWidget(player3Label);

        player4Label = new QLabel(centralwidget);
        player4Label->setObjectName("player4Label");
        player4Label->setFont(font2);

        verticalLayout_2->addWidget(player4Label);


        horizontalLayout_4->addLayout(verticalLayout_2);

        SnakeBot->setCentralWidget(centralwidget);
        menubar = new QMenuBar(SnakeBot);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 630, 24));
        SnakeBot->setMenuBar(menubar);
        statusbar = new QStatusBar(SnakeBot);
        statusbar->setObjectName("statusbar");
        SnakeBot->setStatusBar(statusbar);

        retranslateUi(SnakeBot);

        QMetaObject::connectSlotsByName(SnakeBot);
    } // setupUi

    void retranslateUi(QMainWindow *SnakeBot)
    {
        SnakeBot->setWindowTitle(QCoreApplication::translate("SnakeBot", "SnakeBot", nullptr));
        timerLabel->setText(QString());
        player1Label->setText(QString());
        player2Label->setText(QString());
        player3Label->setText(QString());
        player4Label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class SnakeBot: public Ui_SnakeBot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SNAKEBOT_H
