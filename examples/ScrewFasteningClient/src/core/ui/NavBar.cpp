/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file NavBar.cpp
 * @brief 一级菜单和二级菜单的创建及页面切换。
 *
 * 一级菜单和二级菜单共用按钮排列及选中规则。
 */

#include "NavBar.h"

#include <QBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPointer>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

#include "SettingsEditor.h"
#include "FitStack.h"
#include "WidgetFactory.h"
#include "WrappingButton.h"

namespace screw {

QWidget *createNavBar(const QStringList &items, int buttonMinHeight, const char *barObjectName,
                      QWidget *parent, QList<QPushButton *> *buttons, bool vertical)
{
    auto *bar = new QWidget(parent);
    bar->setObjectName(QString::fromLatin1(barObjectName));

    // 横向菜单使用可换行按钮并等分宽度；纵向菜单保留固定按钮高度。
    QBoxLayout *layout = vertical ? static_cast<QBoxLayout *>(new QVBoxLayout(bar))
                                  : static_cast<QBoxLayout *>(createNavRow(bar));
    for (const QString &text : items) {
        QPushButton *button = vertical ? new QPushButton(text, bar) : new WrappingButton(text, bar);

        // 记录按钮的目标尺寸和选中状态，插件在 render() 中统一设置最终样式。
        button->setCheckable(true);
        button->setAutoExclusive(true);
        button->setProperty(kNavButtonProperty, true);
        button->setProperty(kNavMinHeightProperty, buttonMinHeight);
        layout->addWidget(button, vertical ? 0 : 1);
        if (buttons != Q_NULLPTR) {
            buttons->append(button);
        }
    }

    // 首次显示时选中第一个菜单按钮，与默认页面保持一致。
    if (QLayoutItem *firstItem = layout->itemAt(0)) {
        if (QPushButton *firstButton = qobject_cast<QPushButton *>(firstItem->widget())) {
            firstButton->setChecked(true);
        }
    }

    if (vertical)
        layout->addStretch();
    return bar;
}

void bindNav(FitStack *stack, const QList<QPushButton *> &buttons)
{
    // 切换前确认未保存的修改，取消切换时恢复导航按钮的选中状态。
    for (int index = 0; index < buttons.size(); ++index) {
        QObject::connect(buttons.at(index), &QPushButton::clicked, stack, [stack, buttons, index]() {
            if (stack->currentIndex() == index)
                return;

            const QPointer<FitStack> stackGuard(stack);
            const bool leave = confirmSettingsLeave(stack->currentWidget());
            if (!stackGuard)
                return;
            if (!leave) {
                for (int buttonIndex = 0; buttonIndex < buttons.size(); ++buttonIndex)
                    buttons.at(buttonIndex)->setChecked(buttonIndex == stack->currentIndex());
                return;
            }
            stack->setCurrentIndex(index);
        });
    }
}

} // namespace screw
