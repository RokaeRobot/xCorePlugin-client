#ifndef RLCUSTOMCOMMAND_H
#define RLCUSTOMCOMMAND_H

/**
 * @file rlcustomcommand.h
 * @brief 用户自定义 RL 指令的声明式描述、解析与生成
 *
 * 二次插件通过 RlCustomCommandSpec 描述一条指令的参数形态（类型、可选/必选、
 * 赋值前缀等），宿主 RLManager::createData() 据此注册 skeleton / pattern，
 * 并支持 parseCustomCommandLine / buildCustomCommandLine 做行级读写。
 *
 * 典型流程：
 *   1. 插件 afterInit 中构造 RlCustomCommandSpec 并调用 RLManager::createData()
 *   2. 编辑区 skeleton 由 RlCustomCommandBuilder::buildSkeleton 生成
 *   3. 批量修改 / 校验由 buildPattern + parseLine 完成
 */

#include <QHash>
#include <QList>
#include <QString>
#include "xplugin_global.h"

namespace xplugin {

/**
 * @brief 自定义指令单个形参的 UI / 语法类型
 *
 * 与 RL 编辑器 skeleton 占位符一一对应，见 RlCustomCommandBuilder::fieldSkeletonToken。
 */
enum RlCustomFieldKind
{
    RlFieldInteger = 0,   ///< 整数 spinbox，范围见 RlCustomField::intMin/intMax
    RlFieldDouble,        ///< 浮点 dspn
    RlFieldText,          ///< 自由文本 edt
    RlFieldCartPoint,     ///< 笛卡尔点位 robtarget
    RlFieldJointPoint,    ///< 关节点位 jointtarget
    RlFieldSpeed,         ///< 速度 cmb#SPEED
    RlFieldZone,          ///< 转弯区 cmb#ZONE
    RlFieldTool,          ///< 工具 cmb#TOOL
    RlFieldWobj,          ///< 工件 cmb#WOBJ
    RlFieldSwitch         ///< 开关 cmb#On/Off，界面显示 On/Off
};

/**
 * @brief 形参在调用中的出现规则
 */
enum ArgRuleTypeAPI
{
    ARG_MUST_HAVE = 0,    ///< 必选实参，省略则 parse/build 失败
    ARG_DEFAULT           ///< 可选实参，可省略或与 defaultValue 相同时不写
};

/**
 * @brief 单条自定义指令的一个形参描述
 */
struct XPLUGIN_EXPORT RlCustomField
{
    QString key;                          ///< 形参名，用于 values.fields 键及 skeleton 占位
    RlCustomFieldKind kind = RlFieldInteger;
    ArgRuleTypeAPI argRule = ARG_MUST_HAVE;
    QString defaultValue;                 ///< ARG_DEFAULT 时省略实参的默认值（buildLine 比较用）
    int intMin = 0;                       ///< RlFieldInteger 下界
    int intMax = 999;                     ///< RlFieldInteger 上界

    RlCustomField()
    {
    }

    /** @brief 必选/可选形参（无可省略默认值字符串） */
    RlCustomField(const QString &fieldKey,
                  RlCustomFieldKind fieldKind,
                  ArgRuleTypeAPI rule = ARG_MUST_HAVE)
        : key(fieldKey)
        , kind(fieldKind)
        , argRule(rule)
    {
    }

    /** @brief 带 defaultValue 的可选形参 */
    RlCustomField(const QString &fieldKey,
                  RlCustomFieldKind fieldKind,
                  ArgRuleTypeAPI rule,
                  const QString &defaultVal)
        : key(fieldKey)
        , kind(fieldKind)
        , argRule(rule)
        , defaultValue(defaultVal)
    {
    }

    /** @return 是否为可省略形参（ARG_DEFAULT） */
    bool isDefaultArg() const
    {
        return argRule == ARG_DEFAULT;
    }
};

/**
 * @brief 一条自定义 RL 指令的完整规格
 *
 * type 为指令名（大小写不敏感匹配）；fields 顺序即位置实参顺序。
 */
struct XPLUGIN_EXPORT RlCustomCommandSpec
{
    QString type;                         ///< 指令名，如 "MyMove"
    QString describe;                     ///< 指令说明（注册到 RL 描述表）
    QString defaultLine;                  ///< 插入时的默认整行文本（可选）
    bool allowAssignPrefix = true;        ///< 是否允许 "p2 = Cmd(...);" 赋值前缀
    QList<RlCustomField> fields;          ///< 形参列表，顺序与括号内实参一致
};

/**
 * @brief 从一行 RL 文本解析出的实参值，或用于 buildLine 的输入
 */
struct XPLUGIN_EXPORT RlCustomCommandValues
{
    QString assignTarget;                 ///< allowAssignPrefix 为 true 时，记录 "=" 左侧变量名
    QHash<QString, QString> fields;       ///< key -> 实参字符串；可选形参省略时值为空
};

/**
 * @brief skeleton / 正则 pattern / 行解析与拼行的静态工具
 *
 * skeleton 示例：MyCmd($a#spn#0,999#true$,$p1#robtarget##true$);
 * pattern  用于 RL 编辑器校验与批量匹配；含尾部可选参、中间可选参等特殊规则。
 */
class XPLUGIN_EXPORT RlCustomCommandBuilder
{
public:
    /** @brief 由 spec 生成 RL 编辑器 skeleton 整行（含 type(...) 与占位符） */
    static QString buildSkeleton(const RlCustomCommandSpec &spec);

    /** @brief 生成整行匹配用 QRegularExpression 源串（含可选赋值前缀、注释尾） */
    static QString buildPattern(const RlCustomCommandSpec &spec);

    /** @brief 仅匹配 "Type(...);" 语句体，捕获括号内实参列表 */
    static QString buildStatementPattern(const RlCustomCommandSpec &spec);

    /** @brief 解析一行 RL 为 values；忽略 // 后注释 */
    static bool parseLine(const RlCustomCommandSpec &spec, const QString &line, RlCustomCommandValues &values);

    /** @brief 由 values 拼回一行；必选缺失或非法时返回空串 */
    static QString buildLine(const RlCustomCommandSpec &spec, const RlCustomCommandValues &values);

    /** @brief 按逗号拆分实参列表，尊重双引号内逗号 */
    static QStringList tokenizeCallArguments(const QString &argListText);

    /**
     * @brief 将位置实参列表映射到形参名
     * @param formals 形参定义
     * @param actuals 实际 token 列表
     * @param outFields 输出 key -> 值；可选形参未出现时 insert 空串
     */
    static bool resolvePositionalArgs(const QList<RlCustomField> &formals,
                                      const QStringList &actuals,
                                      QHash<QString, QString> &outFields);

private:
    /** @brief 单个形参对应的 skeleton 占位 token */
    static QString fieldSkeletonToken(const RlCustomField &field);
    /** @brief 正则捕获组内部字符类（不含外层括号） */
    static QString fieldCaptureInner(const RlCustomField &field);
    static QString fieldCaptureCore(const RlCustomField &field);
    static QString fieldCapturePattern(const RlCustomField &field);
    /** @brief 尾部连续 ARG_DEFAULT 形参的起始下标 */
    static int trailingOptionalStartIndex(const QList<RlCustomField> &fields);
    /** @brief 是否存在“中间可选、后面仍有必选”的形参 */
    static bool hasMiddleOptional(const QList<RlCustomField> &fields);
    /** @brief 拼 pattern 时单个形参段（首参/逗号/可选捕获策略） */
    static QString fieldPatternSegment(const RlCustomField &field,
                                       bool isFirst,
                                       bool fixedCaptureOptional,
                                       bool optionalCommaPrefix = false);
    /** @brief 拆分 "target = statement" 赋值前缀 */
    static bool splitAssignPrefix(const QString &line, bool allowAssign, QString &assignTarget, QString &statement);

    /** @brief 单个 token 是否符合形参类型约束 */
    static bool tokenMatchesField(const QString &token, const RlCustomField &field);
    /** @brief 回溯：从 formalIdx/argIdx 起能否完成剩余形参与实参对齐 */
    static bool canCompleteAssignment(const QList<RlCustomField> &formals,
                                      const QStringList &actuals,
                                      int formalIdx,
                                      int argIdx);
    /** @brief 去首尾空白；去掉成对双引号 */
    static QString normalizeArgumentToken(const QString &token);
};

/**
 * @brief 全局自定义指令规格表（按 type 索引）
 *
 * RLManager::createData 内部 registerSpec；parse/build 委托给 RlCustomCommandBuilder。
 */
class XPLUGIN_EXPORT RlCustomCommandRegistry
{
public:
    static RlCustomCommandRegistry &instance();

    /** @brief 注册或覆盖一条 spec（type 为空则忽略） */
    void registerSpec(const RlCustomCommandSpec &spec);
    bool hasSpec(const QString &type) const;
    RlCustomCommandSpec specOf(const QString &type) const;

    bool parseLine(const QString &type, const QString &line, RlCustomCommandValues &values) const;
    QString buildLine(const QString &type, const RlCustomCommandValues &values) const;

private:
    QHash<QString, RlCustomCommandSpec> m_specs;
};

}

#endif // RLCUSTOMCOMMAND_H
