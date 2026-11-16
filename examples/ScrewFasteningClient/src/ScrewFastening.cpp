/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file ScrewFastening.cpp
 * @brief 客户端插件入口。
 *
 * 创建菜单和设置页面，处理页面样式及程序运行状态变化。
 */

#include "ScrewFastening.h"

#include <QAbstractButton>
#include <QComboBox>
#include <QEvent>
#include <QFrame>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLayout>
#include <QMessageBox>
#include <QPointer>
#include <QPushButton>
#include <QSizePolicy>
#include <QStyledItemDelegate>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>

#include "core/ui/FitStack.h"
#include "core/ui/LabelColumnAligner.h"
#include "core/ui/NavBar.h"
#include "core/controller/RunLock.h"
#include "core/ui/StyleTokens.h"
#include "core/ui/SettingsEditor.h"
#include "core/ui/SettingsTransfer.h"
#include "core/ui/WidgetFactory.h"
#include "event/xplugineventsystem.h"
#include "rl/rlmanager.h"
#include "rl/SetProcessPage.h"
#include "pages/CommandPage.h"
#include "pages/CurvePage.h"
#include "pages/FeedbackPage.h"
#include "pages/PresetPage.h"
#include "pages/RunPage.h"
#include "pages/StatusPage.h"

XPLUGIN_REGISTER(PLUGIN_NAME, ScrewFastening)

namespace {
const char kHeldProperty[] = "screw_held"; // 记录控件是否因程序运行而被禁用的属性名。

// 判断控件是否属于需要限制操作的输入框、下拉框或按钮。
bool isOperableInput(QWidget *widget)
{
    if (qobject_cast<QComboBox *>(widget) != nullptr || qobject_cast<QAbstractButton *>(widget) != nullptr) {
        return true;
    }
    auto *edit = qobject_cast<QLineEdit *>(widget);
    return edit != nullptr && qobject_cast<QComboBox *>(edit->parentWidget()) == nullptr;
}

// 根据程序运行状态禁用操作控件，程序停止后恢复原先可用的控件。
void holdOperableWidgets(QWidget *root, bool locked)
{
    if (root == nullptr) {
        return;
    }

    // 统一处理页面中的操作控件；已经由编辑保存功能管理的配置控件单独判断。
    const QList<QWidget *> widgets = root->findChildren<QWidget *>();
    for (QWidget *widget : widgets) {
        if (!isOperableInput(widget)) {
            continue;
        }

        // 程序运行期间允许切换页面查看状态，导航按钮不属于配置输入。
        if (widget->property(screw::kNavButtonProperty).toBool())
            continue;

        // 配置输入由编辑状态管理；编辑保存按钮保持可用，点击时检查运行状态。
        if (widget->property(screw::kSettingsManagedProperty).toBool() ||
            widget->property(screw::kEditButtonProperty).toBool() ||
            widget->property(screw::kSaveButtonProperty).toBool())
            continue;

        // 只记录本次禁用的控件，程序停止后恢复原本可用的控件，保留原有只读限制。
        if (locked) {
            if (!widget->isEnabled() || widget->property(kHeldProperty).toBool()) {
                continue;
            }
            widget->setProperty(kHeldProperty, true);
            widget->setEnabled(false);
        } else if (widget->property(kHeldProperty).toBool()) {
            widget->setProperty(kHeldProperty, QVariant());
            widget->setEnabled(true);
        }
    }
}

// 判断控制器状态是否表示程序正在运行。
bool programIsRunning(const QString &state)
{
    const QString normalized = state.trimmed().toLower();
    return normalized == QLatin1String("program") || normalized == QLatin1String("programing") ||
           normalized == QLatin1String("programming");
}

} // namespace

xplugin::ScrewFastening::ScrewFastening(QObject *parent) : PluginBase(parent)
{
    setPluginLabel(tr("ScrewFastening"));
    setDefaultGlobalSheet(false);
}

void xplugin::ScrewFastening::init()
{
    auto *page = new QWidget;
    page->setObjectName(QStringLiteral("page_main"));

    auto *rootLayout = new QHBoxLayout(page);

    // 在一级菜单的页面按钮后添加“导出配置”和“导入配置”按钮。
    QList<QPushButton *> navButtons;
    auto *pnl_nav = screw::createNavBar(
        {
            tr("预设参数"),
            tr("运行控制"),
            tr("控制指令"),
            tr("指令反馈"),
            tr("状态读取"),
            tr("扭矩曲线"),
            tr("导出配置"),
            tr("导入配置"),
        },
        style::kPrimaryNavButtonHeight, "pnl_nav", page, &navButtons, true);

    auto *pnl_content = new QWidget(page);
    pnl_content->setObjectName(QStringLiteral("pnl_content"));

    auto *contentLayout = new QVBoxLayout(pnl_content);
    auto *pageStack = new FitStack(pnl_content);
    contentLayout->addWidget(pageStack);

    // 创建六个设置页面，首次进入显示预设参数页。
    m_presetPage = new PresetPage(pageStack);
    pageStack->addWidget(m_presetPage);
    pageStack->addWidget(new RunPage(pageStack));
    pageStack->addWidget(new CommandPage(pageStack));
    pageStack->addWidget(new FeedbackPage(pageStack));
    pageStack->addWidget(new StatusPage(pageStack));
    pageStack->addWidget(new CurvePage(pageStack));

    // 关联菜单按钮与页面切换，并选中第一个页面。
    screw::bindNav(pageStack, navButtons.mid(0, pageStack->count()));
    pageStack->setCurrentIndex(0);

    // 导入和导出配置时打开文件选择窗口，当前页面及导航选中状态保持不变。
    for (int index = pageStack->count(); index < navButtons.size(); ++index) {
        navButtons.at(index)->setAutoExclusive(false);
        navButtons.at(index)->setCheckable(false);
    }
    connect(navButtons.at(6), &QPushButton::clicked, page, [page]() { screw::exportSettings(page); });
    connect(navButtons.at(7), &QPushButton::clicked, page, [page]() { screw::importSettings(page); });

    // 添加菜单与内容区域之间的分隔线，完成主界面的排列。
    auto *line_nav_split = new QLabel(page);
    line_nav_split->setObjectName(QStringLiteral("line_nav_split"));
    line_nav_split->setFrameShape(QFrame::VLine);
    line_nav_split->setFrameShadow(QFrame::Sunken);

    rootLayout->addWidget(pnl_nav);
    rootLayout->addWidget(line_nav_split);
    rootLayout->addWidget(pnl_content, 1);

    // 保存主界面指针，供样式设置、配置恢复和连接确认使用。
    m_centerPage = page;
    screw::bindSettingsReturnReminder(page);

    // 注册 SetProcess 指令编辑页面。
    xplugin::RLManager rlManager;
    rlManager.createGroups(tr("螺丝锁附"), QStringLiteral("screwfastening"));
    const auto createCommandPage = [this]() {
        auto *commandPage = new SetProcessPage;
        m_commandPages.append(commandPage);
        render();
        return commandPage;
    };
    rlManager.createData(
        QStringLiteral("screwfastening"), QStringLiteral("SetProcess"), tr("设置工艺配方参数"),
        createCommandPage, createCommandPage);

    xplugin::CreateCenterWidget(PLUGIN_NAME, page);

    // 连接成功后请求更新插件页面，确认窗口仍由页面绘制后的延后调用显示。
    xplugin::xPluginInterface().connectNetworkStatus(true, QString::fromLatin1(PLUGIN_NAME), this, [this]() {
        m_runSignalActive = false;
        refreshInputLock();
        if (m_centerPage != nullptr && !m_presetAsked) {
            m_centerPage->update();
        }
    });

    /*
     * 仅手动断开连接后允许再次提示预设参数写入。
     * 自动重新连接不重复弹出确认窗口。
     */
    xplugin::xPluginEvent().subscribe(
        xplugin::NetworkDisconnected,
        [this](const xplugin::XPluginEvent &event) {
            m_runSignalActive = false;
            refreshInputLock();
            if (event.property(QStringLiteral("is_expected")).toBool()) {
                m_presetAsked = false;
            }
        },
        QString::fromLatin1(PLUGIN_NAME), this);

    // 程序启停时立即更新输入限制，避免短指令落在两次轮询之间。
    xplugin::xPluginEvent().subscribe(
        xplugin::RCRunStart,
        [this](const xplugin::XPluginEvent &) {
            m_runSignalActive = true;
            refreshInputLock();
        },
        QString::fromLatin1(PLUGIN_NAME), this);
    xplugin::xPluginEvent().subscribe(
        xplugin::RCRunStop,
        [this](const xplugin::XPluginEvent &) {
            m_runSignalActive = false;
            refreshInputLock();
        },
        QString::fromLatin1(PLUGIN_NAME), this);

    // 根据程序运行状态统一限制编辑和设备操作。
    screw::RunLock::instance().subscribe(this, [this](bool locked) { holdOperableWidgets(m_centerPage, locked); });

    /*
     * 定期读取程序运行状态，及时更新页面的操作限制。
     * 启动时先读取一次，避免首次定时检查之前允许操作。
     */
    auto *stateTimer = new QTimer(this);
    stateTimer->setInterval(300);
    connect(stateTimer, &QTimer::timeout, this, &ScrewFastening::refreshInputLock);
    stateTimer->start();
    refreshInputLock();
}

void xplugin::ScrewFastening::refreshInputLock()
{
    auto &interface = xplugin::xPluginInterface();
    if (!interface.getRobotConnected()) {
        // 断线后清除旧连接的启停记录，离线设置仍可在本地保存。
        m_runSignalActive = false;
        screw::RunLock::instance().setLocked(false);
        return;
    }
    // 启动信号到达后不因状态缓存尚未刷新而提前解锁。
    screw::RunLock::instance().setLocked(m_runSignalActive || programIsRunning(interface.getRobotPhyStatus()));
}

void xplugin::ScrewFastening::render()
{
    if (m_centerPage == nullptr) {
        return;
    }

    // 页面容器和文字。
    QFont pageFont = m_centerPage->font();
#ifdef Q_OS_WIN
    pageFont.setFamily(QStringLiteral("Microsoft YaHei"));
#else
    pageFont.setFamily(QStringLiteral("Noto Sans CJK SC"));
#endif
    pageFont.setPixelSize(16);
    const QString pageSheet = QStringLiteral(
        "QWidget#page_main { background-color: #F5F5F5; }"
        "QLabel { color: #202020; background: transparent; }");

    // 参数分组。
    const QString groupSheet = QStringLiteral(
        "QGroupBox {"
        " background-color: #FFFFFF; border: 1px solid #E3E3E3;"
        " border-radius: 10px; margin-top: 20px; padding: 8px;"
        "}"
        "QGroupBox::title {"
        " subcontrol-origin: margin; subcontrol-position: top left;"
        " left: 12px; padding: 0px 4px; color: #202020;"
        "}"
        "QGroupBox[func=\"no_title\"] { border: none; margin-top: 0px; }"
        "QGroupBox#pnl_torque_curve_form { margin-top: 0px; }");

    // 通用操作按钮。
    const QString buttonSheet = QStringLiteral(
        "QPushButton {"
        " background-color: #254E73; color: #FFFFFF;"
        " border: 1px solid transparent; border-radius: 6px;"
        " min-width: 56px; min-height: 40px; padding: 0px 8px;"
        "}"
        "QPushButton:hover { background-color: #2D5D89; }"
        "QPushButton:pressed { background-color: #1C3D5B; }"
        "QPushButton:focus { border-color: #8FB8DC; }"
        "QPushButton:disabled { background-color: #A5B4C2; color: #F1F3F5; }");

    // 导航按钮。
    const QString navigationSheet = QStringLiteral(
        "QPushButton[navButton=\"true\"]:checked {"
        " background-color: %1; color: %2;"
        "}"
        "QPushButton[navButton=\"true\"]:checked:disabled { background-color: #B9787B; }")
                                        .arg(QString::fromLatin1(style::kNavCheckedBackground),
                                             QString::fromLatin1(style::kNavCheckedTextColor));

    // 数值输入框。
    const QString inputSheet = QStringLiteral(
        "QLineEdit {"
        " background-color: #FFFFFF; color: #202020;"
        " border: none; border-bottom: 1px solid #D9D9D9; border-radius: 0px;"
        " padding: 0px 2px; selection-background-color: #254E73; selection-color: #FFFFFF;"
        "}"
        "QLineEdit:focus { border-bottom-color: #254E73; }"
        "QLineEdit:disabled { color: #8C8C8C; border-bottom-color: #D9D9D9; }");

    // 下拉框。
    const QString comboSheet = QStringLiteral(
        "QComboBox {"
        " background-color: #FFFFFF; color: #202020;"
        " border: 1px solid #D8DEE5; border-radius: 4px; padding: 0px 30px 0px 8px;"
        "}"
        "QComboBox:enabled:hover { border-color: #98AFC3; }"
        "QComboBox:focus { border-color: #254E73; }"
        "QComboBox:disabled { background-color: #F3F4F6; color: #8C9299; border-color: #E0E4E8; }"
        "QComboBox::drop-down {"
        " subcontrol-origin: padding; subcontrol-position: top right; width: 28px;"
        " background: transparent; border: none;"
        "}"
        "QComboBox::down-arrow {"
        " image: url(:/ScrewFastening/icons/chevron-down.png); width: 12px; height: 8px;"
        "}"
        "QComboBox::down-arrow:disabled { image: url(:/ScrewFastening/icons/chevron-down-disabled.png); }"
        "QComboBox QLineEdit {"
        " background: transparent; color: #202020; border: none; padding: 0px;"
        " selection-background-color: #254E73; selection-color: #FFFFFF;"
        "}"
        "QComboBox QLineEdit:disabled { color: #8C9299; }"
        "QComboBox QAbstractItemView {"
        " background-color: #FFFFFF; color: #202020; border: 1px solid #D8DEE5;"
        " selection-background-color: #E9F0F7; selection-color: #254E73; outline: none;"
        "}");

    // 放大触屏列表的基础行高；重复 render() 时复用委托，避免高度累积。
    class ComboItemDelegate : public QStyledItemDelegate
    {
    public:
        explicit ComboItemDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
        {
            QSize size = QStyledItemDelegate::sizeHint(option, index);
            size.setHeight(qRound(size.height() * 2.5));
            return size;
        }
    };

    // 递归统一内容容器边距，输入控件和按钮保留各自的内部布局。
    std::function<void(QLayout *)> clearContainerMargins;
    clearContainerMargins = [&clearContainerMargins](QLayout *layout) {
        if (layout == nullptr)
            return;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(6);
        for (int index = 0; index < layout->count(); ++index) {
            QLayoutItem *item = layout->itemAt(index);
            if (QLayout *nested = item->layout()) {
                clearContainerMargins(nested);
            } else if (QWidget *container = item->widget()) {
                if (qobject_cast<QComboBox *>(container) != nullptr ||
                    qobject_cast<QLineEdit *>(container) != nullptr ||
                    qobject_cast<QAbstractButton *>(container) != nullptr)
                    continue;

                // 内容容器允许随文字换行增加高度，并靠上排列，避免吸收页面剩余高度。
                QSizePolicy policy = container->sizePolicy();
                if (policy.verticalPolicy() == QSizePolicy::Maximum) {
                    policy.setVerticalPolicy(QSizePolicy::Preferred);
                    container->setSizePolicy(policy);
                    item->setAlignment(item->alignment() | Qt::AlignTop);
                }
                clearContainerMargins(container->layout());
            }
        }
    };

    // 主界面和指令编辑页。
    QList<QWidget *> pages{m_centerPage.data()};
    for (int index = m_commandPages.size() - 1; index >= 0; --index) {
        if (m_commandPages.at(index).isNull())
            m_commandPages.removeAt(index);
        else
            pages.append(m_commandPages.at(index).data());
    }
    const QString sheet = pageSheet + groupSheet + buttonSheet + navigationSheet;
    const int pageMargin = 15;
    for (QWidget *page : pages) {
        page->setFont(pageFont);
        if (page->styleSheet() != sheet)
            page->setStyleSheet(sheet);
        clearContainerMargins(page->layout());

        // 主界面底部留白。
        if (page == m_centerPage.data() && page->layout() != nullptr)
            page->layout()->setContentsMargins(0, 0, 0, pageMargin);

        // 无二级导航页面的顶部留白。
        const char *singlePages[] = {
            "pnl_command_page", "pnl_feedback_page", "pnl_status_page", "pnl_torque_curve_page",
        };
        for (const char *name : singlePages) {
            QWidget *content = page->findChild<QWidget *>(QString::fromLatin1(name));
            if (content != nullptr && content->layout() != nullptr)
                content->layout()->setContentsMargins(0, pageMargin, 0, 0);
        }

        // 扭矩曲线显示区域。
        if (QWidget *chart = page->findChild<QWidget *>(QStringLiteral("lbl_torque_chart"))) {
            // 图表保持横向伸缩，固定高度避免白色背景和纵轴随页面空白拉长。
            chart->setFixedHeight(435);
            chart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        }

        // 导航栏整体留白。
        if (QWidget *navigation = page->findChild<QWidget *>(QStringLiteral("pnl_nav"))) {
            // 扩大容器宽度以容纳左侧留白，保留原有导航按钮宽度。
            navigation->setFixedWidth(style::kPrimaryNavWidth + pageMargin);
            if (navigation->layout() != nullptr)
                navigation->layout()->setContentsMargins(pageMargin, pageMargin, 0, 0);
        }
        const char *secondaryBars[] = {"pnl_preset_nav", "pnl_run_nav"};
        for (const char *name : secondaryBars) {
            if (QWidget *navigation = page->findChild<QWidget *>(QString::fromLatin1(name))) {
                navigation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
                if (navigation->layout() != nullptr)
                    navigation->layout()->setContentsMargins(0, pageMargin, 0, 0);
            }
        }

        // 导航按钮。
        for (QPushButton *button : page->findChildren<QPushButton *>()) {
            if (!button->property(screw::kNavButtonProperty).toBool())
                continue;

            const QString sizeSheet = QStringLiteral(
                "QPushButton { min-width: 0px; min-height: 0px; padding: 6px; }");
            if (button->styleSheet() != sizeSheet)
                button->setStyleSheet(sizeSheet);

            const int height = button->property(screw::kNavMinHeightProperty).toInt();
            const bool primary = button->parentWidget()->objectName() == QLatin1String("pnl_nav");
            button->setMinimumWidth(0);
            if (primary) {
                button->setFixedHeight(height);
                button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            } else {
                // 同排按钮等宽分配；整排采用最高文字所需的高度，按钮之间保持等高。
                button->setMinimumHeight(height);
                QSizePolicy policy(QSizePolicy::Ignored, QSizePolicy::Minimum);
                policy.setHeightForWidth(true);
                button->setSizePolicy(policy);
            }
        }

        // 编辑保存按钮。
        for (QPushButton *button : page->findChildren<QPushButton *>()) {
            if (!button->property(screw::kSaveButtonProperty).toBool() &&
                !button->property(screw::kEditButtonProperty).toBool())
                continue;

            const QString sizeSheet = QStringLiteral("QPushButton { min-width: %1px; min-height: %2px; }")
                                          .arg(style::kSaveButtonMinWidth)
                                          .arg(style::kSaveButtonMinHeight);
            if (button->styleSheet() != sizeSheet)
                button->setStyleSheet(sizeSheet);
        }

        // 下拉框与数值输入框。
        // 下拉框统一设置外框和内嵌编辑框，数值输入框独立保留底线样式。
        for (QComboBox *combo : page->findChildren<QComboBox *>()) {
            combo->setMinimumWidth(96);
            combo->setMinimumHeight(40);
            combo->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
            if (combo->styleSheet() != comboSheet)
                combo->setStyleSheet(comboSheet);
            // 只安装一次；每次按基础行高计算，重复 render() 不会累积放大。
            if (dynamic_cast<ComboItemDelegate *>(combo->itemDelegate()) == nullptr)
                combo->setItemDelegate(new ComboItemDelegate(combo));
        }
        for (QLineEdit *edit : page->findChildren<QLineEdit *>()) {
            if (qobject_cast<QComboBox *>(edit->parentWidget()) != nullptr)
                continue;
            edit->setMinimumWidth(64);
            edit->setMinimumHeight(40);
            edit->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
            if (edit->styleSheet() != inputSheet)
                edit->setStyleSheet(inputSheet);
        }

        // 表单标签。
        for (QLabel *label : page->findChildren<QLabel *>()) {
            if (!label->objectName().startsWith(QLatin1String("lbl_")))
                continue;
            label->setProperty(kLabelWidthLimitProperty, 180);
            label->setMinimumWidth(0);
            label->setMaximumWidth(180);
            label->setWordWrap(true);
        }

        // 表单各列。
        for (QWidget *row : page->findChildren<QWidget *>()) {
            if (!row->objectName().startsWith(QLatin1String("row_")))
                continue;
            auto *layout = qobject_cast<QHBoxLayout *>(row->layout());
            if (layout == nullptr)
                continue;
            row->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
            for (int index = 0; index < layout->count(); ++index) {
                QWidget *control = layout->itemAt(index)->widget();
                const int stretch = qobject_cast<QComboBox *>(control) != nullptr ? 3
                                    : qobject_cast<QLineEdit *>(control) != nullptr ? 2 : 0;
                layout->setStretch(index, stretch);
            }
        }

        // 标签列宽刷新。
        for (QObject *object : page->findChildren<QObject *>()) {
            if (auto *aligner = dynamic_cast<LabelColumnAligner *>(object))
                aligner->alignColumns();
        }
        page->update();
    }
}

void xplugin::ScrewFastening::idle()
{
    if (m_centerPage == nullptr) {
        return;
    }

    // 初始化寄存器选项，配置由各页面在显示时从文件恢复。
    screw::refreshRegisterCombos(m_centerPage);

    // 页面就绪后监听绘制事件，以便在事件处理结束后触发预设参数写入询问。
    m_centerPage->installEventFilter(this);
    m_centerPage->update();
}

bool xplugin::ScrewFastening::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_centerPage.data() && event->type() == QEvent::Paint && !m_presetAsked &&
        xplugin::xPluginInterface().getRobotConnected()) {
        // 通过零延时定时器，在当前绘制事件处理结束后弹窗询问是否将已保存的预设参数写入锁丝机寄存器。
        QTimer::singleShot(0, this, &ScrewFastening::maybeAskPresetWrite);
    }
    return PluginBase::eventFilter(watched, event);
}

void xplugin::ScrewFastening::maybeAskPresetWrite()
{
    // 延后调用执行前重新检查页面和连接状态，页面关闭或连接断开时不再提示。
    const QPointer<QWidget> page(m_centerPage);
    if (m_presetAsked || page.isNull() || !page->isVisible() || screw::RunLock::instance().isLocked()) {
        return;
    }
    if (!xplugin::xPluginInterface().getRobotConnected()) {
        return;
    }

    // 检查过无可写参数或用户取消也不重复提示；仅手动断开连接会重置此标志。
    m_presetAsked = true;

    // 预设页不存在或没有已保存的可写参数时，不弹窗询问。
    if (!m_presetPage || !m_presetPage->hasSavedPresets()) {
        return;
    }

    const int answer = QMessageBox::question(page->window(), tr("确认"),
                                             tr("已连接到控制器。是否把示教器上次保存的预设参数写入锁丝机?"),
                                             QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
    if (answer != QMessageBox::Ok || page.isNull() || screw::RunLock::instance().isLocked()) {
        return;
    }

    // 确认后再次检查连接状态，连接断开时不执行寄存器写入。
    if (!xplugin::xPluginInterface().getRobotConnected())
        return;

    // 用户确认后读取已保存的预设参数并写入锁丝机，未保存的页面修改不参与写入。
    if (m_presetPage) {
        m_presetPage->writeSavedPresets();
    }
}
