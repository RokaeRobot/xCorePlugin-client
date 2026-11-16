/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file PresetPage.h
 * @brief 预设参数页面。
 */

#ifndef PRESET_PAGE_H
#define PRESET_PAGE_H

#include <QWidget>
#include <QVector>

namespace xplugin {

class PresetSectionPage;

class PresetPage : public QWidget
{
    Q_OBJECT
public:
    explicit PresetPage(QWidget *parent = nullptr);

    /** @brief 检查文件中是否存在寄存器名称和设定值均已填写的预设参数。 */
    bool hasSavedPresets() const;

    /**
     * @brief 将已保存的预设参数写入锁丝机。
     *
     * 重新读取客户端本地设置文件，仅写入寄存器名称和数值均已填写的参数行。
     * @return 全部写入成功或没有可写参数返回 true；存在写入失败返回 false。
     */
    bool writeSavedPresets();

private:
    QVector<PresetSectionPage *> m_pages; // 按二级菜单顺序保存的分类页面，由页面容器管理。
};

} // namespace xplugin

#endif // PRESET_PAGE_H
