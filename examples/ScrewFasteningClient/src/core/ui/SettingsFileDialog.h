/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SettingsFileDialog.h
 * @brief 配置文件的选择窗口。
 */
#ifndef SETTINGS_FILE_DIALOG_H
#define SETTINGS_FILE_DIALOG_H

#include <QByteArray>
#include <QString>
class QWidget;

namespace screw {

/** @brief 文件路径和选中时的存储设备信息，用于读写前再次检查。 */
struct SettingsFileSelection
{
    QString path;       // 待导入或导出的文件绝对路径。
    QString mountPath;  // 选中的存储设备挂载目录；Windows 文件窗口不限制挂载目录。
    QByteArray device;  // 选中时的设备名称，用于检查挂载目录是否仍对应原设备。
};

/**
 * @brief 选择导入或导出文件；取消、父窗口为空或无可用设备时返回空路径。
 * @param[in] parent 文件窗口所属的控件，不能为空。
 * @param[in] importing true 使用已有文件，false 选择导出目标并确认覆盖。
 * @note Windows 使用系统窗口；Linux 将浏览范围限制为选定设备的挂载目录。
 */
SettingsFileSelection selectSettingsFile(QWidget *parent, bool importing);

/** @brief 读写前检查设备和文件路径；导出时同时检查存储设备是否只读。 */
bool settingsFileAvailable(const SettingsFileSelection &selection, bool importing);

} // namespace screw
#endif
