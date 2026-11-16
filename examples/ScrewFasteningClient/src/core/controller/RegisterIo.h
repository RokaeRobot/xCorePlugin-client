/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file RegisterIo.h
 * @brief 寄存器读写和实时刷新接口，供各页面调用。
 *
 * 使用HMI宿主程序的通信页面已配置的寄存器。
 */

#ifndef REGISTER_IO_H
#define REGISTER_IO_H

#include <functional>

#include <QString>
#include <QStringList>

class QWidget;

class RegisterIo
{
public:
    /** @brief 本次寄存器读取结果。 */
    enum class ReadResult {
        Success,      // 已取得所需寄存器的数值，或当前没有配置数据来源。
        Busy,         // 已有读取请求正在处理。
        Disconnected, // 控制器未连接，或读取期间连接发生变化。
        Failed        // 未取得有效回复。
    };

    /**
     * @brief 按名称写入单个寄存器。
     *
     * 按寄存器类型转换数值，等待写入请求完成后返回。
     * @param[in] name 寄存器名称，不能为空。
     * @param[in] value 待写入的数值文本，不能为空。
     * @return 数值转换及写入均成功返回 true，否则返回 false。
     */
    static bool writeValue(const QString &name, const QString &value);

    /**
     * @brief 按名称读取寄存器数值并更新客户端缓存。
     *
     * 只读取指定寄存器的数组，空名称跳过，重复名称只读取一次。
     * @param[in] names 当前页面需要的寄存器名称；列表为空时不发送请求。
     * @return 本次读取结果，区分成功、已有请求、断线和失败。
     */
    static ReadResult readValues(const QStringList &names);

    /**
     * @brief 获取单个寄存器最近读取的数值。
     * @param[in] name 寄存器名称。
     * @return 最近一次有效读取的第 0 项文本；缓存或该项缺失时返回空字符串。
     */
    static QString currentValue(const QString &name);

    /**
     * @brief 获取所选寄存器的数组元素。
     *
     * 按元素序号读取，缺失元素保留空位置，不读取其他寄存器。
     * @param[in] startName 数据来源寄存器名称，多值数据应配置为数组寄存器。
     * @param[in] count 最多返回的数组元素数，从序号 0 开始，不代表连续寄存器地址数。
     * @return 截至末个已缓存序号的数值列表，长度不超过 count；缺失序号为空字符串。
     *         参数无效或没有缓存时返回空列表。
     */
    static QStringList valuesFrom(const QString &startName, int count);

    /**
     * @brief 为页面设置实时刷新。
     *
     * 页面显示时每秒读取当前来源，逐个请求之间允许处理界面事件，隐藏时停止。
     * @param[in] page 需要刷新的页面，允许为空。
     * @param[in] sources 返回当前页面所需寄存器名称的函数。
     * @param[in] apply 读取成功后的页面更新函数，允许为空。
     * @param[in] clear 进入页面、断线或读取失败时调用的函数。
     */
    static void attachLiveRefresh(QWidget *page, const std::function<QStringList()> &sources,
                                  const std::function<void()> &apply,
                                  const std::function<void()> &clear);

    /**
     * @brief 为需要保留上次显示的页面设置实时刷新。
     *
     * 只刷新可见页面选择的寄存器，由页面自行处理读取结果。
     * @param[in] page 需要刷新的页面，允许为空指针。
     * @param[in] sources 返回当前页面所需寄存器名称的函数。
     * @param[in] onRead 根据读取结果更新显示的函数。
     */
    static void attachLiveRefresh(QWidget *page, const std::function<QStringList()> &sources,
                                  const std::function<void(ReadResult)> &onRead);
};

#endif // REGISTER_IO_H
