#ifndef DEMOPAGELAYOUT_H
#define DEMOPAGELAYOUT_H

#include "apiresultpanel.h"

#include <functional>
#include <QCursor>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSplitter>
#include <QStyle>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @brief 功能页通用布局: left ops + right result (QSplitter)
 *
 * - round button: no API text, fixed size
 * - API name on right (dark); translation below (gray)
 */
struct DemoPageLayout
{
    static const int kActionBtnWidth = 28;
    static const int kActionBtnHeight = 28;

    QScrollArea *opsScroll;
    QWidget *opsHost;
    QVBoxLayout *opsLayout;
    ApiResultPanel *result;
    QSplitter *splitter;

    static DemoPageLayout build(QWidget *page)
    {
        DemoPageLayout out;
        out.opsHost = new QWidget(page);
        out.opsHost->setObjectName(QStringLiteral("demoOpsHost"));
        out.opsLayout = new QVBoxLayout(out.opsHost);
        out.opsLayout->setAlignment(Qt::AlignTop);
        out.opsLayout->setContentsMargins(0, 0, 4, 0);
        out.opsLayout->setSpacing(12);

        out.opsScroll = new QScrollArea(page);
        out.opsScroll->setObjectName(QStringLiteral("demoOpsScroll"));
        out.opsScroll->setWidgetResizable(true);
        out.opsScroll->setWidget(out.opsHost);
        out.opsScroll->setFrameShape(QFrame::NoFrame);
        out.opsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        out.opsScroll->setMinimumWidth(280);

        out.result = new ApiResultPanel(page);
        out.result->setMinimumWidth(200);

        out.splitter = new QSplitter(Qt::Horizontal, page);
        out.splitter->setObjectName(QStringLiteral("demoPageSplitter"));
        out.splitter->setChildrenCollapsible(false);
        out.splitter->setHandleWidth(6);
        out.splitter->addWidget(out.opsScroll);
        out.splitter->addWidget(out.result);
        out.splitter->setStretchFactor(0, 3);
        out.splitter->setStretchFactor(1, 2);
        out.splitter->setSizes(QList<int>() << 520 << 360);

        QHBoxLayout *root = new QHBoxLayout(page);
        root->setContentsMargins(12, 12, 12, 12);
        root->setSpacing(0);
        root->addWidget(out.splitter);
        return out;
    }

    QWidget *addGroup(const QString &title)
    {
        QFrame *card = new QFrame(opsHost);
        card->setObjectName(QStringLiteral("demoCard"));
        card->setFrameShape(QFrame::NoFrame);

        QVBoxLayout *lay = new QVBoxLayout(card);
        lay->setContentsMargins(14, 12, 14, 12);
        lay->setSpacing(6);

        QLabel *titleLbl = new QLabel(title, card);
        titleLbl->setObjectName(QStringLiteral("demoCardTitle"));
        titleLbl->setWordWrap(true);
        lay->addWidget(titleLbl);

        opsLayout->addWidget(card);
        return card;
    }

    QPushButton *addButton(QWidget *card, const QString &apiText, const QString &desc,
                           const std::function<void()> &fn)
    {
        QVBoxLayout *lay = qobject_cast<QVBoxLayout *>(card->layout());

        QWidget *row = new QWidget(card);
        row->setObjectName(QStringLiteral("demoActionRow"));
        QHBoxLayout *rowLay = new QHBoxLayout(row);
        rowLay->setContentsMargins(0, 0, 0, 0);
        rowLay->setSpacing(10);

        QPushButton *btn = createActionButton(row);
        btn->setToolTip(apiText + QStringLiteral("\n") + desc);

        QWidget *textHost = new QWidget(row);
        textHost->setObjectName(QStringLiteral("demoActionText"));
        QVBoxLayout *textLay = new QVBoxLayout(textHost);
        textLay->setContentsMargins(0, 0, 0, 0);
        textLay->setSpacing(2);

        QLabel *apiLbl = new QLabel(apiText, textHost);
        apiLbl->setObjectName(QStringLiteral("demoActionApi"));
        apiLbl->setWordWrap(true);
        apiLbl->setTextInteractionFlags(Qt::TextSelectableByMouse);

        QLabel *descLbl = new QLabel(desc, textHost);
        descLbl->setObjectName(QStringLiteral("demoActionDesc"));
        descLbl->setWordWrap(true);
        descLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        descLbl->setMinimumWidth(0);

        textLay->addWidget(apiLbl);
        textLay->addWidget(descLbl);
        textHost->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        textHost->setMinimumWidth(0);

        rowLay->addWidget(btn, 0, Qt::AlignLeft | Qt::AlignTop);
        rowLay->addWidget(textHost, 1);
        rowLay->setStretch(0, 0);
        rowLay->setStretch(1, 1);
        row->setMinimumWidth(kActionBtnWidth + 120);

        lay->addWidget(row);
        QObject::connect(btn, &QPushButton::clicked, card, fn);
        return btn;
    }

    static QPushButton *createActionButton(QWidget *parent)
    {
        // No API text on the button — keeps every control the same size
        QPushButton *btn = new QPushButton(parent);
        btn->setObjectName(QStringLiteral("demoActionBtn"));
        btn->setFixedSize(kActionBtnWidth, kActionBtnHeight);
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setProperty("func", QVariant(QStringLiteral("operation")));
        if (btn->style()) {
            btn->style()->unpolish(btn);
            btn->style()->polish(btn);
        }
        return btn;
    }

    void finish()
    {
        opsLayout->addStretch(1);
    }
};

#endif // DEMOPAGELAYOUT_H
