/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SettingsEditor.h
 * @brief 恢复页面配置，保留页面输入，并管理编辑状态。
 */

#ifndef SETTINGS_EDITOR_H
#define SETTINGS_EDITOR_H

#include <functional>

class QPushButton;
class QWidget;

namespace screw {

// 标记始终只读的反馈控件，编辑状态不改变只读限制。
constexpr const char kSettingsReadOnlyProperty[] = "settingsReadOnly";
// 标记由编辑状态统一管理的配置控件。
constexpr const char kSettingsManagedProperty[] = "settingsManaged";

// 配方号用于切换查看，程序停止时可直接选择；不作为参数内容参与修改检查。
constexpr const char kSettingsSelectorProperty[] = "settingsSelector";

// 页面名称用于检查导入参数是否属于对应的配置页面。
constexpr const char kSettingsSectionProperty[] = "settingsSection";

/**
 * @brief 指定页面显示时恢复配置的方法。
 * @param[in] scope 配置页面容器，不能为空。
 * @param[in] restore 从文件读取配置并更新页面的函数。
 * @param[in] manageEditing 是否通过编辑保存按钮管理输入，控制指令页面传入 false。
 */
void bindSettingsRestore(QWidget *scope, const std::function<void()> &restore,
                         bool manageEditing = true);

/** @brief 本地保存成功后结束编辑；单行写入页面同时记录已保存内容。 */
void markSettingsSaved(QWidget *scope);

/** @brief 只记录指定控件范围的保存结果，保留其他控件的未保存修改记录。 */
void markSettingsSaved(QWidget *scope, QWidget *savedControls);

/**
 * @brief 离开页面前检查未保存的修改。
 *
 * 只检查当前可见的配置页面；确认放弃后恢复已保存配置，再允许离开。
 * @return 无待确认修改或确认放弃时返回 true；继续编辑、保存进行中或窗口已销毁时返回 false。
 */
bool confirmSettingsLeave(QWidget *root);

/** @brief 从 settings.json 重新读取全部页面配置，更新页面显示并结束编辑。 */
void reloadSettings(QWidget *root);

/**
 * @brief 设置页面是否处于编辑状态。
 *
 * 程序运行期间不能开启编辑；实时反馈控件始终保持只读。
 * @param[in] scope 需要管理的页面容器，允许为空。
 * @param[in] editing 是否开启编辑。
 */
void setSettingsEditing(QWidget *scope, bool editing);

/**
 * @brief 查询页面是否处于编辑状态。
 * @param[in] scope 待查询的页面容器，允许为空。
 * @return 页面处于编辑状态返回 true；容器为空返回 false。
 */
bool settingsEditing(QWidget *scope);

/**
 * @brief 关联页面与同一位置的编辑、保存按钮。
 *
 * 根据页面编辑状态显示“编辑”或“保存”；按钮销毁后停止更新。
 * @param[in] scope 按钮对应的页面容器，允许为空。
 * @param[in] button 需要更新显示状态的按钮，允许为空。
 * @param[in] cancel 是否属于仅在编辑期间显示的取消按钮。
 */
void bindSettingsEditSaveButton(QWidget *scope, QPushButton *button, bool cancel = false);

/** @brief 询问是否放弃当前页面的编辑，确认后恢复已保存配置。 */
void cancelSettingsEditing(QWidget *scope);

/** @brief 插件从隐藏状态返回时，提醒用户继续完成当前页面的编辑。 */
void bindSettingsReturnReminder(QWidget *root);

} // namespace screw

#endif // SETTINGS_EDITOR_H
