#include "pagehmishell.h"

#include "../common/demopagelayout.h"
#include "../common/demoformat.h"

#include "interface/interfacemanager.h"

PageHmiShell::PageHmiShell(QWidget *parent)
    : QWidget(parent)
{
    DemoPageLayout layout = DemoPageLayout::build(this);
    ApiResultPanel *result = layout.result;

    QWidget *gInfo = layout.addGroup(tr("Version / Path"));
    layout.addButton(gInfo, QStringLiteral("GetPluginVersion()"),
                     tr("Get xplugin runtime version string."), [result]() {
        const QString ver = xplugin::GetPluginVersion();
        result->append(QStringLiteral("GetPluginVersion"), !ver.isEmpty(), ver);
    });
    layout.addButton(gInfo, QStringLiteral("GetPluginDataPath()"),
                     tr("Get plugin local data directory path."), [result]() {
        QString path;
        const int ret = xplugin::GetPluginDataPath(path, PLUGIN_NAME);
        result->append(QStringLiteral("GetPluginDataPath"), ret == 0,
                       QStringLiteral("ret=%1 path=%2").arg(ret).arg(path));
    });

    QWidget *gShell = layout.addGroup(tr("Shell Control"));
    layout.addButton(gShell, QStringLiteral("SwitchRightExpandWidgte(PLUGIN)"),
                     tr("Open/switch this plugin right expand panel."), [result]() {
        const int ret = xplugin::SwitchRightExpandWidgte(PLUGIN_NAME);
        result->append(QStringLiteral("SwitchRightExpandWidgte"), ret == 0,
                       QStringLiteral("ret=%1").arg(ret));
    });
    layout.addButton(gShell, QStringLiteral("SwitchRightExpandWidgte() close"),
                     tr("Close right expand panel (empty args)."), [result]() {
        const int ret = xplugin::SwitchRightExpandWidgte();
        result->append(QStringLiteral("SwitchRightExpandWidgte(empty)"), ret == 0,
                       QStringLiteral("ret=%1").arg(ret));
    });
    layout.addButton(gShell, QStringLiteral("GetRightExpandSideBar()"),
                     tr("Get right expand sidebar widget pointer."), [result]() {
        QWidget *bar = nullptr;
        const int ret = xplugin::GetRightExpandSideBar(&bar);
        result->append(QStringLiteral("GetRightExpandSideBar"), ret == 0 && bar != nullptr,
                       QStringLiteral("ret=%1 ptr=%2").arg(ret).arg(reinterpret_cast<quintptr>(bar)));
    });

    QWidget *gView = layout.addGroup(tr("Fullscreen / Modal"));
    layout.addButton(gView, QStringLiteral("ChangeFullScreen(true)"),
                     tr("Enter plugin fullscreen mode."), [result]() {
        const int ret = xplugin::ChangeFullScreen(PLUGIN_NAME, true);
        result->append(QStringLiteral("ChangeFullScreen(true)"), ret == 0,
                       QStringLiteral("ret=%1").arg(ret));
    });
    layout.addButton(gView, QStringLiteral("ChangeFullScreen(false)"),
                     tr("Exit plugin fullscreen mode."), [result]() {
        const int ret = xplugin::ChangeFullScreen(PLUGIN_NAME, false);
        result->append(QStringLiteral("ChangeFullScreen(false)"), ret == 0,
                       QStringLiteral("ret=%1").arg(ret));
    });
    layout.addButton(gView, QStringLiteral("ChangeModelWindowStatus(true)"),
                     tr("Enable modal window status."), [result]() {
        const int ret = xplugin::ChangeModelWindowStatus(PLUGIN_NAME, true);
        result->append(QStringLiteral("ChangeModelWindowStatus(true)"), ret == 0,
                       QStringLiteral("ret=%1").arg(ret));
    });
    layout.addButton(gView, QStringLiteral("ChangeModelWindowStatus(false)"),
                     tr("Disable modal window status."), [result]() {
        const int ret = xplugin::ChangeModelWindowStatus(PLUGIN_NAME, false);
        result->append(QStringLiteral("ChangeModelWindowStatus(false)"), ret == 0,
                       QStringLiteral("ret=%1").arg(ret));
    });

    QWidget *gPhy = layout.addGroup(tr("Physical Button Status"));
    layout.addButton(gPhy, QStringLiteral("ChangePhyButtonStatus(true)"),
                     tr("Enable physical side buttons for this plugin."), [result]() {
        const int ret = xplugin::ChangePhyButtonStatus(PLUGIN_NAME, PLUGIN_NAME, true);
        result->append(QStringLiteral("ChangePhyButtonStatus(true)"), ret == 0,
                       QStringLiteral("ret=%1").arg(ret));
    });
    layout.addButton(gPhy, QStringLiteral("ChangePhyButtonStatus(false)"),
                     tr("Disable physical side buttons for this plugin."), [result]() {
        const int ret = xplugin::ChangePhyButtonStatus(PLUGIN_NAME, PLUGIN_NAME, false);
        result->append(QStringLiteral("ChangePhyButtonStatus(false)"), ret == 0,
                       QStringLiteral("ret=%1").arg(ret));
    });

    layout.finish();
}
