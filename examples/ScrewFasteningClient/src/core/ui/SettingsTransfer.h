/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SettingsTransfer.h
 * @brief 插件配置的导入和导出。
 */

#ifndef SETTINGS_TRANSFER_H
#define SETTINGS_TRANSFER_H

class QWidget;

namespace screw {

/**
 * @brief 导出已保存的插件配置。
 *
 * 选择保存位置，将 settings.json 中已保存的完整配置写入用户选择的文件。
 * 页面中尚未保存的修改不参与导出。
 * @param[in] root 插件主界面指针，不能为空指针。
 */
void exportSettings(QWidget *root);

/**
 * @brief 导入插件配置。
 *
 * 读取用户选择的配置文件，检查文件格式和寄存器配置，确认替换后保存配置并刷新页面。
 * 程序运行期间不允许导入；本地保存后立即尝试同步完整配方集合。
 * 控制器同步失败时保留本地配置并提示，由自动同步继续重试。
 * @param[in] root 插件主界面指针，不能为空指针。
 */
void importSettings(QWidget *root);

} // namespace screw

#endif // SETTINGS_TRANSFER_H
