#ifndef RLMANAGER_H
#define RLMANAGER_H
/******************************************************************************
 * Copyright (c) 2025 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * Information in this file is the intellectual property of Rokae Technology Co., Ltd,
 * And may contains trade secrets that must be stored and viewed confidentially.
 *
 * @file    rlmanager.h
 * @brief   xplugin 侧 RL 指令注册与管理门面（桥接 xcore::RlManager）
 * @version 1.0.0
 * @author  ROKAE Development Team
 *
 * 二次插件在 afterInit 中通过本类注册自定义指令，典型用法：
 *   RLManager mgr;
 *   mgr.createGroups(tr("视觉"), "vision_group");
 *   mgr.createData("vision_group", spec, insertFactory, propertyFactory);
 *   mgr.registerOffsetSlots(spec.type, {1});           // 点位偏移
 *   mgr.registerBatchModifySlots(spec.type, 2, 3);     // 批量修改 speed/zone
 *****************************************************************************/

#include <QString>
#include <QList>
#include <QObject>
#include <functional>
#include "xplugin_global.h"
#include "rlcustomcommand.h"

namespace xplugin {

class RLContentBase;

/**
 * @brief 用户插件 RL 指令注册入口
 *
 * 封装 xcore::RlManager 的 addCmdKeyword / addSkeleton / addPattern 等调用，
 * 并与 RlCustomCommandRegistry、RlInsSlotRegistry 联动。
 */
class XPLUGIN_EXPORT RLManager : public QObject
{
    Q_OBJECT

public:
    explicit RLManager(QObject *parent = nullptr);

    /**
     * @brief 创建 RL 指令分组（侧栏/面板分组标签）
     * @param lab       分组显示名（可 tr 翻译）
     * @param groupName 分组内部键，createTypeToGroup 时使用
     */
    void createGroups(const QString &lab, const QString &groupName);

    /**
     * @brief 创建简单指令（整行关键字，无参数槽位）
     *
     * 适用于单行语句如 EnableVision;，不支持 skeleton 参数编辑与点位偏移。
     *
     * @param groupName 指令组名
     * @param type      指令关键字
     * @param desc      说明文本
     * @param func1     插入面板内容工厂
     * @param func2     属性面板内容工厂
     */
    void createData(const QString &groupName,
                    const QString &type,
                    const QString &desc,
                    std::function<RLContentBase *()> func1 = [] { return nullptr; },
                    std::function<RLContentBase *()> func2 = [] { return nullptr; });

    /**
     * @brief 创建带 skeleton / pattern 的指令（手动指定正则）
     *
     * 适用于已有 skeleton、pattern 字符串的场景；点位偏移需另行 registerOffsetSlots。
     *
     * @param groupName   指令组名
     * @param type        指令名
     * @param desc        说明
     * @param skeleton    参数键值槽字符串（含 $key#...$ 占位）
     * @param pattern     整行匹配正则
     * @param func1       插入面板
     * @param func2       属性面板
     * @param defaultLine 插入默认行，空则使用 type
     */
    void createData(const QString &groupName,
                    const QString &type,
                    const QString &desc,
                    const QString &skeleton,
                    const QString &pattern,
                    std::function<RLContentBase *()> func1 = [] { return nullptr; },
                    std::function<RLContentBase *()> func2 = [] { return nullptr; },
                    const QString &defaultLine = QString());

    /**
     * @brief 创建声明式自定义指令（推荐）
     *
     * 由 RlCustomCommandSpec 自动生成 skeleton / pattern，并写入 RlCustomCommandRegistry，
     * 支持 parseCustomCommandLine / buildCustomCommandLine 及 skeleton 自动解析偏移/批量槽位。
     *
     * @param groupName      指令组名
     * @param spec           指令结构体（见 rlcustomcommand.h）
     * @param insertWidget   插入面板工厂
     * @param propertyWidget 属性面板工厂
     */
    void createData(const QString &groupName,
                    const RlCustomCommandSpec &spec,
                    std::function<RLContentBase *()> insertWidget = [] { return nullptr; },
                    std::function<RLContentBase *()> propertyWidget = [] { return nullptr; });

    /** @brief 将一条或多条指令 type 挂到指定 group */
    void createTypeToGroup(const QString &groupName, const QString &type);
    void createTypeToGroup(const QString &groupName, const QStringList &typeList);

    /** @brief 注册指令关键字（语法高亮 / 补全） */
    void createTypeKey(const QString &type);

    /**
     * @brief 注册 skeleton 参数槽
     * @note  同时触发 RlInsSlotRegistry 从 skeleton 自动扫描偏移与批量修改槽位
     */
    void createSkeleton(const QString &type, const QString &str);

    /** @brief 注册指令说明（用法提示） */
    void createInsDescribe(const QString &type, const QString &str);

    /** @brief 注册插入时的默认行文本 */
    void createInsDefault(const QString &type, const QString &strDef);

    /** @brief 注册整行校验正则（pattern 键为 c_<type>） */
    void createPattern(const QString &type, const QString &str);

    /** @brief 注册插入面板 widget 工厂 */
    void createInsertWidget(const QString &type,
                            std::function<RLContentBase *()> func = [] { return nullptr; });

    /** @brief 注册属性面板 widget 工厂 */
    void createPropertyWidget(const QString &type,
                              std::function<RLContentBase *()> func = [] { return nullptr; });

    /**
     * @brief 解析一行 RL 为结构化字段
     * @param type   已注册的指令名
     * @param line   单行文本（可含 // 注释）
     * @param values 输出：赋值目标 + 各形参值
     */
    static bool parseCustomCommandLine(const QString &type,
                                       const QString &line,
                                       RlCustomCommandValues &values);

    /**
     * @brief 由结构化字段拼回一行 RL
     * @return 失败时返回空串
     */
    static QString buildCustomCommandLine(const QString &type,
                                          const RlCustomCommandValues &values);

    /**
     * @brief 手动注册指令的 skeleton 点位槽下标（0 起，按括号内实参顺序）
     * @param cartSlots  笛卡尔 robtarget 槽
     * @param jointSlots 关节 jointtarget 槽（可省略）
     */
    void registerOffsetSlots(const QString &type,
                             const QList<int> &cartSlots,
                             const QList<int> &jointSlots = QList<int>());

    /**
     * @brief 手动注册指令参与批量修改的 skeleton 槽下标
     *
     * 对应 xmate「批量修改」中的 speed / zone / tool / wobj；未使用的参数传 -1。
     * createSkeleton 也会根据 #cmb#SPEED 等占位自动扫描，本接口用于覆盖或补充。
     *
     * @param speedIndex speed 实参下标，-1 表示无
     * @param zoneIndex  zone 实参下标
     * @param toolIndex  tool 实参下标
     * @param wobjIndex  wobj 实参下标
     */
    void registerBatchModifySlots(const QString &type,
                                  int speedIndex = -1,
                                  int zoneIndex = -1,
                                  int toolIndex = -1,
                                  int wobjIndex = -1);

private:
    /** @brief 预留：从 skeleton 自动检测偏移槽（当前为空实现） */
    void detectOffsetSlotsFromSkeleton(const QString &type, const QString &skeleton);

    /** @brief xcore 内部 skeleton 键前缀 s_<type> */
    inline static QString SName(const QString &name) { return "s_" + name; }
    /** @brief xcore 内部 pattern 键前缀 c_<type> */
    inline static QString CName(const QString &name) { return "c_" + name; }
    /** @brief 点位名 plain pattern 键前缀 p_<type>（保留） */
    inline static QString PName(const QString &name) { return "p_" + name; }
    /** @brief 保留命名前缀 */
    inline static QString NName(const QString &name) { return "n_" + name; }
};

} // namespace xplugin

#endif // RLMANAGER_H
