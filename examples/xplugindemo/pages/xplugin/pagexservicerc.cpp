#include "pagexservicerc.h"

#include "../common/demopagelayout.h"
#include "../common/demoformat.h"

#include "interface/interfacemanager.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QTimer>

PageXServiceRc::PageXServiceRc(QWidget *parent)
    : QWidget(parent)
{
    DemoPageLayout layout = DemoPageLayout::build(this);
    ApiResultPanel *result = layout.result;

    QLineEdit *edKey = new QLineEdit(QStringLiteral("user_log.language_type"), this);
    QLineEdit *edSubKey = new QLineEdit(QStringLiteral("language"), this);
    QLineEdit *edNode = new QLineEdit(QStringLiteral("project.setting"), this);
    QLineEdit *edCustomKey = new QLineEdit(QStringLiteral("ping"), this);
    QLineEdit *edCustomVal = new QLineEdit(QStringLiteral("1"), this);
    QLineEdit *edRcModule = new QLineEdit(QStringLiteral("robot"), this);
    QLineEdit *edRcCmd = new QLineEdit(QStringLiteral("body.get_DH_params"), this);
    QLineEdit *edRcFull = new QLineEdit(QStringLiteral("robot.frame.get_current"), this);
    QLineEdit *edServiceCb = new QLineEdit(QStringLiteral("project.tasks_pointer"), this);

    QWidget *gParam = layout.addGroup(tr("Parameters"));
    {
        QFormLayout *form = new QFormLayout;
        form->addRow(tr("XService key"), edKey);
        form->addRow(tr("subKey"), edSubKey);
        form->addRow(tr("Node key"), edNode);
        form->addRow(tr("Custom key"), edCustomKey);
        form->addRow(tr("Custom val"), edCustomVal);
        form->addRow(tr("RC module"), edRcModule);
        form->addRow(tr("RC command"), edRcCmd);
        form->addRow(tr("RC full"), edRcFull);
        form->addRow(tr("Callback serviceKey"), edServiceCb);
        qobject_cast<QVBoxLayout *>(gParam->layout())->addLayout(form);
    }

    QWidget *gXs = layout.addGroup(tr("XService / Node"));
    layout.addButton(gXs, QStringLiteral("getNodeData()"), tr("Get node data by key."), [result, edNode]() {
        result->append(QStringLiteral("getNodeData"), true,
                       demoJsonToString(xplugin::xPluginInterface().getNodeData(edNode->text())));
    });
    layout.addButton(gXs, QStringLiteral("getXServiceData(key)"), tr("Get XService data by key."), [result, edKey]() {
        result->append(QStringLiteral("getXServiceData"), true,
                       demoJsonToString(xplugin::xPluginInterface().getXServiceData(edKey->text())));
    });
    layout.addButton(gXs, QStringLiteral("getXServiceData(key,sub)"), tr("Get XService data by key and subKey."), [result, edKey, edSubKey]() {
        result->append(QStringLiteral("getXServiceData(sub)"), true,
                       demoJsonToString(xplugin::xPluginInterface().getXServiceData(edKey->text(), edSubKey->text())));
    });
    layout.addButton(gXs, QStringLiteral("setXServiceData(project.setting)"), tr("Set XService data (sample)."), [result]() {
        const bool ok = xplugin::xPluginInterface().setXServiceData(
            QStringLiteral("project.setting"), QStringLiteral("max_manual_speed"), 100);
        result->append(QStringLiteral("setXServiceData"), ok, QStringLiteral("max_manual_speed=100"));
    });
    layout.addButton(gXs, QStringLiteral("commandXServiceData(change_language)"), tr("Async command to XService."), [result]() {
        QJsonObject obj;
        obj.insert(QStringLiteral("target_language"), QStringLiteral("cn"));
        const bool ok = xplugin::xPluginInterface().commandXServiceData(
            QStringLiteral("user_log.change_language"), obj);
        result->append(QStringLiteral("commandXServiceData"), ok, QStringLiteral("user_log.change_language"));
        QTimer::singleShot(3000, result, [result]() {
            result->appendInfo(QStringLiteral("after 3s language=%1")
                                   .arg(demoJsonToString(
                                       xplugin::xPluginInterface().getXServiceData(
                                           QStringLiteral("user_log.language_type"),
                                           QStringLiteral("language")))));
        });
    });
    layout.addButton(gXs, QStringLiteral("execCommandXServiceData(factory)"), tr("Sync exec XService command."), [result]() {
        result->append(QStringLiteral("execCommandXServiceData"), true,
                       demoJsonToString(xplugin::xPluginInterface().execCommandXServiceData(
                           QStringLiteral("factory.get_flex_status"), QString())));
    });

    QWidget *gCb = layout.addGroup(tr("XService Callback"));
    layout.addButton(gCb, QStringLiteral("connectXServiceData()"), tr("Subscribe XService data callback."), [result, edServiceCb]() {
        const bool ok = xplugin::xPluginInterface().connectXServiceData(
            PLUGIN_NAME, edServiceCb->text(),
            [result](const QVariant &data) {
                result->appendInfo(QStringLiteral("XService callback: %1").arg(demoVariantToString(data)));
            });
        result->append(QStringLiteral("connectXServiceData"), ok, edServiceCb->text());
    });
    layout.addButton(gCb, QStringLiteral("disconnectXServiceData()"), tr("Unsubscribe XService data callback."), [result, edServiceCb]() {
        const bool ok = xplugin::xPluginInterface().disconnectXServiceData(PLUGIN_NAME, edServiceCb->text());
        result->append(QStringLiteral("disconnectXServiceData"), ok, edServiceCb->text());
    });

    QWidget *gCustom = layout.addGroup(tr("commandCustomData"));
    layout.addButton(gCustom, QStringLiteral("commandCustomData(key,val)"), tr("Send custom plugin key/value."), [result, edCustomKey, edCustomVal]() {
        const QJsonArray arr = xplugin::xPluginInterface().commandCustomData(
            nullptr, PLUGIN_NAME, edCustomKey->text(), edCustomVal->text());
        result->append(QStringLiteral("commandCustomData(key,val)"), true, demoJsonToString(arr));
    });
    layout.addButton(gCustom, QStringLiteral("commandCustomData(json)"), tr("Send custom plugin JSON object."), [result, edCustomKey, edCustomVal]() {
        QJsonObject json;
        json.insert(edCustomKey->text(), edCustomVal->text());
        const QJsonArray arr = xplugin::xPluginInterface().commandCustomData(nullptr, PLUGIN_NAME, json);
        result->append(QStringLiteral("commandCustomData(json)"), true, demoJsonToString(arr));
    });

    QWidget *gRc = layout.addGroup(tr("RC Request"));
    layout.addButton(gRc, QStringLiteral("rcRequest()"), tr("Async RC request by module/command."), [result, edRcModule, edRcCmd]() {
        const bool ok = xplugin::xPluginInterface().rcRequest(edRcModule->text(), edRcCmd->text(), QJsonValue());
        result->append(QStringLiteral("rcRequest"), ok,
                       QStringLiteral("%1.%2").arg(edRcModule->text(), edRcCmd->text()));
    });
    layout.addButton(gRc, QStringLiteral("rcRequestFullCommand()"), tr("Async RC request by full command."), [result, edRcFull]() {
        const bool ok = xplugin::xPluginInterface().rcRequestFullCommand(edRcFull->text());
        result->append(QStringLiteral("rcRequestFullCommand"), ok, edRcFull->text());
    });
    layout.addButton(gRc, QStringLiteral("rcExecRequest()"), tr("Sync RC exec by module/command."), [result, edRcModule, edRcCmd]() {
        result->append(QStringLiteral("rcExecRequest"), true,
                       demoJsonToString(xplugin::xPluginInterface().rcExecRequest(
                           edRcModule->text(), edRcCmd->text(), QJsonValue())));
    });
    layout.addButton(gRc, QStringLiteral("rcExecRequestFullCommand()"), tr("Sync RC exec by full command."), [result, edRcFull]() {
        result->append(QStringLiteral("rcExecRequestFullCommand"), true,
                       demoJsonToString(xplugin::xPluginInterface().rcExecRequestFullCommand(edRcFull->text())));
    });

    layout.finish();
}
