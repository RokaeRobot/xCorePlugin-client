/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file LabelColumnAligner.h
 * @brief 统一各参数表单的标签列宽度。
 */

#ifndef LABEL_COLUMN_ALIGNER_H
#define LABEL_COLUMN_ALIGNER_H

#include <QEvent>
#include <QObject>

class QWidget;

// 标签列的宽度上限由 render() 设置。
constexpr const char kLabelWidthLimitProperty[] = "labelWidthLimit";

class LabelColumnAligner : public QObject
{
public:
    /**
     * @brief 创建由 form 管理的标签列对齐器；调用方还需将其安装为 form 的事件过滤器。
     * @param[in] form 待对齐的表单指针，同时作为父对象，不能为空。
     */
    explicit LabelColumnAligner(QWidget *form);

    /**
     * @brief 按字体测量值、可用宽度和样式上限统一 lbl_ 标签列宽。
     *
     * 长文字的换行属性由页面样式设置。
     */
    void alignColumns();

protected:
    /**
     * @brief 在表单完成样式初始化、显示、调整尺寸或更换字体时重新对齐。
     * @param[in] watched 被监视的对象指针。
     * @param[in] event 事件指针。
     * @return 是否停止事件继续传递。
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QWidget *m_form; // 需要统一标签列宽度的表单指针。
};

#endif // LABEL_COLUMN_ALIGNER_H
