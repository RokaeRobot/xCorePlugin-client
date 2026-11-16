/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file FormPageBuilder.cpp
 * @brief 通用参数页面的创建。
 *
 * 根据参数分组创建表单和操作按钮，供多个设置页面共用。
 */

#include "FormPageBuilder.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSizePolicy>
#include <QStringList>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

#include "EditSaveControls.h"
#include "LabelColumnAligner.h"
#include "FitStack.h"
#include "StyleTokens.h"
#include "WidgetFactory.h"

namespace screw {

QHash<QString, ParamRowWidgets> buildFormContent(QWidget *page, const char *formName,
                                                 const QVector<FormGroup> &groups, bool withSaveButton,
                                                 bool editReadOnly)
{
    QHash<QString, ParamRowWidgets> rows;
    auto *pageLayout = new QVBoxLayout(page);

    // 内容区域保持所需高度，按钮与表单容器底部对齐；剩余空间留在页面下方。
    auto *body = new QWidget(page);
    body->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    auto *bodyLayout = new QHBoxLayout(body);

    auto *form = new QWidget(body);
    form->setObjectName(QString::fromLatin1(formName));
    form->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    auto *formLayout = new QVBoxLayout(form);

    // 收集全部参数名称，按同一列宽排列，避免各行输入区域左右参差。
    QStringList labelTexts;
    for (const FormGroup &group : groups) {
        for (const FormRow &row : group.rows) {
            labelTexts << row.text;
        }
    }
    const int labelWidth = labelColumnWidth(form, labelTexts);

    // 表单显示后按最终字体统一标签宽度。
    form->installEventFilter(new LabelColumnAligner(form));

    for (const FormGroup &group : groups) {
        auto *groupBox = new QGroupBox(group.title, form);
        if (group.key != Q_NULLPTR) {
            groupBox->setObjectName(QStringLiteral("grp_") + QString::fromLatin1(group.key));
        } else {
            groupBox->setObjectName(QStringLiteral("grp_content"));
        }
        if (group.title.isEmpty()) {
            groupBox->setProperty("func", "no_title");
        }

        auto *groupLayout = new QVBoxLayout(groupBox);
        formLayout->addWidget(groupBox);

        for (const FormRow &row : group.rows) {
            const auto widgets = createParamRow(groupBox, row.key, row.text, labelWidth, editReadOnly);
            groupLayout->addWidget(widgets.widget);
            rows.insert(QString::fromLatin1(row.key), widgets);
        }
    }

    bodyLayout->addWidget(form, 1);

    // 需要保存的页面添加编辑保存按钮，按钮作用范围限定为当前页面。
    if (withSaveButton) {
        bodyLayout->addWidget(createEditSaveControls(body, page), 0, Qt::AlignRight | Qt::AlignBottom);
    }

    pageLayout->addWidget(body);
    pageLayout->addStretch();
    return rows;
}

QWidget *createFormPage(FitStack *stack, const char *pageName, const char *formName,
                        const QVector<FormGroup> &groups)
{
    auto *page = new QWidget(stack);
    page->setObjectName(QString::fromLatin1(pageName));
    buildFormContent(page, formName, groups, false);
    stack->addWidget(page);
    return page;
}

QWidget *createPlaceholderPage(FitStack *stack, const char *pageName, const char *hintName)
{
    auto *page = new QWidget(stack);
    page->setObjectName(QString::fromLatin1(pageName));

    auto *layout = new QVBoxLayout(page);
    auto *hint = new QLabel(page);
    hint->setObjectName(QString::fromLatin1(hintName));
    hint->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    layout->addWidget(hint);

    stack->addWidget(page);
    return page;
}

} // namespace screw
