/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file ProcessType.h
 * @brief 定义 SetProcess 工艺类型及其查询接口。
 */

#ifndef PROCESS_TYPE_H
#define PROCESS_TYPE_H

#include <QString>
#include <initializer_list>

namespace xplugin {

/**
 * @brief SetProcess 支持的工艺类型。
 *
 * 有效工艺的枚举值对应写入任务链执行寄存器（通信地址348）的工艺编号。
 * 修改编号时须同步检查控制器的工艺编号映射。
 */
enum ProcessType : int
{
    InvalidProcess = 0, // 无效工艺。
    TorqueSetting = 1,  // 扭矩设置。
    SuctionProcess = 2, // 吸式工艺。
    BlowProcess = 3,    // 吹式工艺。
    ScrewPoint = 4,     // 螺丝点。
    IoOutput = 5,       // IO 输出。
    IoWait = 6          // IO 等待。
};

/** @brief 查询 SetProcess 支持的工艺及其名称，判断工艺是否需要配方号。 */
class ProcessTypeTool final
{
public:
    /** @brief 返回工艺类型总数，用于确定 at() 的下标范围。 */
    static int count();

    /**
     * @brief 按 SetProcess 工艺选项的显示顺序取工艺类型。
     * @param[in] index 下标，有效范围为 0 到 count() - 1。
     * @return 下标有效时返回对应工艺，越界时返回 InvalidProcess。
     */
    static ProcessType at(int index);

    /**
     * @brief 返回 RL 指令和配方同步使用的工艺名称。
     * @return 找不到对应工艺时返回空字符串。
     */
    static QString name(ProcessType type);

    /**
     * @brief 按工艺名称查找对应的工艺类型。
     * @param[in] name 工艺名称，例如 TorqueSetting。区分大小写。
     * @return 匹配的工艺类型。找不到对应工艺时返回 InvalidProcess。
     */
    static ProcessType fromName(const QString &name);

    /**
     * @brief 判断工艺是否需要配方号。
     * @return 仅在工艺已定义且需要配方号时返回 true。
     */
    static bool usesRecipeIndex(ProcessType type);

private:
    ProcessTypeTool() = delete;

    struct Entry;

    static const std::initializer_list<Entry> &entries();

    static const Entry *find(ProcessType type);
};

} // namespace xplugin

#endif // PROCESS_TYPE_H
