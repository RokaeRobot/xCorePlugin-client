#include "pageioregister.h"

#include "../common/demopagelayout.h"
#include "../common/demoformat.h"

#include "interface/interfacemanager.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>

PageIoRegister::PageIoRegister(QWidget *parent)
    : QWidget(parent)
{
    DemoPageLayout layout = DemoPageLayout::build(this);
    ApiResultPanel *result = layout.result;

    QLineEdit *edIoName = new QLineEdit(QStringLiteral("DO0"), this);
    QComboBox *cbType = new QComboBox(this);
    cbType->addItems(QStringList() << QStringLiteral("DI") << QStringLiteral("DO")
                                   << QStringLiteral("GI") << QStringLiteral("GO")
                                   << QStringLiteral("AI") << QStringLiteral("AO"));
    cbType->setCurrentText(QStringLiteral("DO"));

    QWidget *gParam = layout.addGroup(tr("Parameters"));
    {
        QFormLayout *form = new QFormLayout;
        form->addRow(tr("IO name"), edIoName);
        form->addRow(tr("Type"), cbType);
        qobject_cast<QVBoxLayout *>(gParam->layout())->addLayout(form);
    }

    QWidget *gIo = layout.addGroup(tr("IO"));
    layout.addButton(gIo, QStringLiteral("getIOHash()"), tr("Get all IO names and values."), [result]() {
        result->append(QStringLiteral("getIOHash"), true,
                       demoHashVariantToString(xplugin::xPluginInterface().getIOHash()));
    });
    layout.addButton(gIo, QStringLiteral("getIOByType(QString)"), tr("List IO names by type string."), [result, cbType]() {
        result->append(QStringLiteral("getIOByType(QString)"), true,
                       demoStringListToString(xplugin::xPluginInterface().getIOByType(cbType->currentText())));
    });
    layout.addButton(gIo, QStringLiteral("getIOByType(int=1 DO)"), tr("List IO names by type int."), [result]() {
        result->append(QStringLiteral("getIOByType(int)"), true,
                       demoStringListToString(xplugin::xPluginInterface().getIOByType(1)));
    });
    layout.addButton(gIo, QStringLiteral("getIOValue()"), tr("Read one IO value."), [result, edIoName]() {
        const bool v = xplugin::xPluginInterface().getIOValue(edIoName->text());
        result->append(QStringLiteral("getIOValue"), true,
                       QStringLiteral("%1=%2").arg(edIoName->text()).arg(v));
    });
    layout.addButton(gIo, QStringLiteral("setIOValue(true)"), tr("Set IO value to true."), [result, edIoName, cbType]() {
        const bool ok = xplugin::xPluginInterface().setIOValue(edIoName->text(), cbType->currentText(), true);
        result->append(QStringLiteral("setIOValue(true)"), ok, edIoName->text());
    });
    layout.addButton(gIo, QStringLiteral("setIOValue(false)"), tr("Set IO value to false."), [result, edIoName, cbType]() {
        const bool ok = xplugin::xPluginInterface().setIOValue(edIoName->text(), cbType->currentText(), false);
        result->append(QStringLiteral("setIOValue(false)"), ok, edIoName->text());
    });

    QWidget *gReg = layout.addGroup(tr("Register"));
    layout.addButton(gReg, QStringLiteral("getRegisterHash()"), tr("Get register name map."), [result]() {
        result->append(QStringLiteral("getRegisterHash"), true,
                       demoHashStringListToString(xplugin::xPluginInterface().getRegisterHash()));
    });
    layout.addButton(gReg, QStringLiteral("getRegisterValue()"), tr("Get all register values."), [result]() {
        result->append(QStringLiteral("getRegisterValue"), true,
                       demoJsonToString(xplugin::xPluginInterface().getRegisterValue()));
    });

    layout.finish();
}
