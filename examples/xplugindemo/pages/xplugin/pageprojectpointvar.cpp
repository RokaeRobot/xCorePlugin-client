#include "pageprojectpointvar.h"

#include "../common/demopagelayout.h"
#include "../common/demoformat.h"

#include "interface/interfacemanager.h"

#include <QFormLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLineEdit>
#include <QSpinBox>

PageProjectPointVar::PageProjectPointVar(QWidget *parent)
    : QWidget(parent)
{
    DemoPageLayout layout = DemoPageLayout::build(this);
    ApiResultPanel *result = layout.result;

    QLineEdit *edPoint = new QLineEdit(QStringLiteral("demo_p1"), this);
    QLineEdit *edValueIndex = new QLineEdit(QStringLiteral("[0][0][0]"), this);
    QLineEdit *edTool = new QLineEdit(QStringLiteral("tool0"), this);
    QLineEdit *edWobj = new QLineEdit(QStringLiteral("wobj0"), this);
    QLineEdit *edVar = new QLineEdit(QStringLiteral("demo_num"), this);
    QLineEdit *edPers = new QLineEdit(QStringLiteral("demo_pers"), this);
    QLineEdit *edProject = new QLineEdit(QStringLiteral("demo_proj"), this);
    QLineEdit *edPointFrom = new QLineEdit(QStringLiteral("demo_p1"), this);
    QLineEdit *edPointTo = new QLineEdit(QStringLiteral("demo_p2"), this);
    QSpinBox *spSpeed = new QSpinBox(this);
    spSpeed->setRange(1, 100);
    spSpeed->setValue(50);

    QWidget *gParam = layout.addGroup(tr("Parameters"));
    {
        QFormLayout *form = new QFormLayout;
        form->addRow(tr("Point name"), edPoint);
        form->addRow(tr("Point value index"), edValueIndex);
        form->addRow(tr("Tool"), edTool);
        form->addRow(tr("Wobj"), edWobj);
        form->addRow(tr("Variable name (non-PERS)"), edVar);
        form->addRow(tr("PERS variable name"), edPers);
        form->addRow(tr("Project name"), edProject);
        form->addRow(tr("Point from"), edPointFrom);
        form->addRow(tr("Point to"), edPointTo);
        form->addRow(tr("Speed"), spSpeed);
        qobject_cast<QVBoxLayout *>(gParam->layout())->addLayout(form);
    }

    QWidget *gTool = layout.addGroup(tr("Tool / Wobj / Speed / Zone"));
    layout.addButton(gTool, QStringLiteral("getCurTool()"), tr("Get current tool name."), [result]() {
        result->append(QStringLiteral("getCurTool"), true, xplugin::xPluginInterface().getCurTool());
    });
    layout.addButton(gTool, QStringLiteral("getCurWobj()"), tr("Get current wobj name."), [result]() {
        result->append(QStringLiteral("getCurWobj"), true, xplugin::xPluginInterface().getCurWobj());
    });
    layout.addButton(gTool, QStringLiteral("getToolList()"), tr("List all tools."), [result]() {
        result->append(QStringLiteral("getToolList"), true,
                       demoStringListToString(xplugin::xPluginInterface().getToolList()));
    });
    layout.addButton(gTool, QStringLiteral("getWobjList()"), tr("List all wobjs."), [result]() {
        result->append(QStringLiteral("getWobjList"), true,
                       demoStringListToString(xplugin::xPluginInterface().getWobjList()));
    });
    layout.addButton(gTool, QStringLiteral("getSpeedList()"), tr("List speed presets."), [result]() {
        result->append(QStringLiteral("getSpeedList"), true,
                       demoStringListToString(xplugin::xPluginInterface().getSpeedList()));
    });
    layout.addButton(gTool, QStringLiteral("getZoneList()"), tr("List zone presets."), [result]() {
        result->append(QStringLiteral("getZoneList"), true,
                       demoStringListToString(xplugin::xPluginInterface().getZoneList()));
    });

    QWidget *gVar = layout.addGroup(tr("Variable (non-PERS)"));
    layout.addButton(gVar, QStringLiteral("getVarDataList()"), tr("List variable names."), [result]() {
        result->append(QStringLiteral("getVarDataList"), true,
                       demoStringListToString(xplugin::xPluginInterface().getVarDataList()));
    });
    layout.addButton(gVar, QStringLiteral("addVarData(num, pers=false)"),
                     tr("Create a non-PERS numeric variable (isPers=false)."), [result, edVar]() {
        const QString name = edVar->text().trimmed();
        const bool ok = xplugin::xPluginInterface().addVarData(name, 1.0, false);
        result->append(QStringLiteral("addVarData(pers=false)"), ok,
                       QStringLiteral("%1 value=1.0").arg(name));
    });
    layout.addButton(gVar, QStringLiteral("updateVarData()"), tr("Update non-PERS variable value to 2.0."), [result, edVar]() {
        const QString name = edVar->text().trimmed();
        const bool ok = xplugin::xPluginInterface().updateVarData(name, 2.0);
        result->append(QStringLiteral("updateVarData"), ok, QStringLiteral("%1 value=2.0").arg(name));
    });
    layout.addButton(gVar, QStringLiteral("getVarObjDataByName()"), tr("Get variable JSON by name."), [result, edVar]() {
        const QString name = edVar->text().trimmed();
        const QJsonObject obj = xplugin::xPluginInterface().getVarObjDataByName(name);
        result->append(QStringLiteral("getVarObjDataByName"), !obj.isEmpty(), demoJsonToString(obj));
    });
    layout.addButton(gVar, QStringLiteral("delVarData()"), tr("Delete variable by name."), [result, edVar]() {
        const QString name = edVar->text().trimmed();
        const bool ok = xplugin::xPluginInterface().delVarData(name);
        result->append(QStringLiteral("delVarData"), ok, name);
    });

    QWidget *gPers = layout.addGroup(tr("PERS variable"));
    layout.addButton(gPers, QStringLiteral("addVarData(num, pers=true)"),
                     tr("Create a PERS numeric variable (isPers=true). Required before PERS APIs."),
                     [result, edPers]() {
        const QString name = edPers->text().trimmed();
        const bool ok = xplugin::xPluginInterface().addVarData(name, 1.0, true);
        result->append(QStringLiteral("addVarData(pers=true)"), ok,
                       QStringLiteral("%1 value=1.0 pers=true").arg(name));
    });
    layout.addButton(gPers, QStringLiteral("registPERSVars()"),
                     tr("Register PERS var name for continuous polling."), [result, edPers]() {
        const QString name = edPers->text().trimmed();
        QVector<QString> names;
        names << name;
        const int ret = xplugin::xPluginInterface().registPERSVars(names);
        result->append(QStringLiteral("registPERSVars"), ret == 0,
                       QStringLiteral("%1 ret=%2").arg(name).arg(ret));
    });
    layout.addButton(gPers, QStringLiteral("getRegistedPERSVars()"),
                     tr("Get current values of the registered PERS variables."), [result]() {
        const QVector<QVariant> vals = xplugin::xPluginInterface().getRegistedPERSVars();
        QStringList parts;
        for (int i = 0; i < vals.size(); ++i) {
            parts << demoVariantToString(vals.at(i));
        }
        const bool ok = !vals.isEmpty();
        result->append(QStringLiteral("getRegistedPERSVars"), ok,
                       ok ? parts.join(QStringLiteral(", ")) : QStringLiteral("<empty> (register PERS first)"));
    });
    layout.addButton(gPers, QStringLiteral("getValuesOfPERSVars()"),
                     tr("Read PERS values by names (one-shot)."), [result, edPers]() {
        const QString name = edPers->text().trimmed();
        QVector<QString> names;
        names << name;
        const QVector<QString> vals = xplugin::xPluginInterface().getValuesOfPERSVars(names);
        const bool ok = !vals.isEmpty() && !vals.first().isEmpty();
        result->append(QStringLiteral("getValuesOfPERSVars"), ok,
                       QStringLiteral("%1 => %2").arg(name, demoVectorToString(vals)));
    });
    layout.addButton(gPers, QStringLiteral("setValuesOfPERSVars()"),
                     tr("Write PERS value to 3.0 by name."), [result, edPers]() {
        const QString name = edPers->text().trimmed();
        QVector<QString> names;
        names << name;
        QVector<QVariant> values;
        values << 3.0;
        const int ret = xplugin::xPluginInterface().setValuesOfPERSVars(names, values);
        result->append(QStringLiteral("setValuesOfPERSVars"), ret == 0,
                       QStringLiteral("%1 value=3.0 ret=%2").arg(name).arg(ret));
    });
    layout.addButton(gPers, QStringLiteral("runPersSmokeTest()"),
                     tr("One-click: ensure PERS var -> regist -> read -> write 3.0 -> read again."),
                     [result, edPers]() {
        const QString name = edPers->text().trimmed();
        if (name.isEmpty()) {
            result->append(QStringLiteral("runPersSmokeTest"), false, QStringLiteral("PERS name is empty"));
            return;
        }

        auto &iface = xplugin::xPluginInterface();
        const QStringList existing = iface.getVarDataList();
        if (existing.contains(name)) {
            const QJsonObject obj = iface.getVarObjDataByName(name);
            const QJsonValue persVal = obj.value(QStringLiteral("pers"));
            const bool isPers = persVal.isBool() ? persVal.toBool()
                                                 : (persVal.toString().compare(QStringLiteral("true"), Qt::CaseInsensitive) == 0);
            if (!isPers) {
                result->append(QStringLiteral("runPersSmokeTest"), false,
                               QStringLiteral("%1 exists but pers!=true; delete it or change PERS name").arg(name));
                return;
            }
            result->append(QStringLiteral("smoke.addVarData"), true,
                           QStringLiteral("%1 already exists as PERS (reuse)").arg(name));
        } else {
            const bool created = iface.addVarData(name, 1.0, true);
            result->append(QStringLiteral("smoke.addVarData(pers=true)"), created, name);
            if (!created) {
                result->append(QStringLiteral("runPersSmokeTest"), false,
                               QStringLiteral("create PERS failed: %1").arg(name));
                return;
            }
        }

        QVector<QString> names;
        names << name;
        const int regRet = iface.registPERSVars(names);
        result->append(QStringLiteral("smoke.registPERSVars"), regRet == 0,
                       QStringLiteral("ret=%1").arg(regRet));

        const QVector<QString> before = iface.getValuesOfPERSVars(names);
        const bool readOk = !before.isEmpty() && !before.first().isEmpty();
        result->append(QStringLiteral("smoke.getValuesOfPERSVars(before)"), readOk,
                       demoVectorToString(before));

        QVector<QVariant> values;
        values << 3.0;
        const int setRet = iface.setValuesOfPERSVars(names, values);
        result->append(QStringLiteral("smoke.setValuesOfPERSVars(3.0)"), setRet == 0,
                       QStringLiteral("ret=%1").arg(setRet));

        const QVector<QString> after = iface.getValuesOfPERSVars(names);
        const bool afterOk = !after.isEmpty() && !after.first().isEmpty();
        result->append(QStringLiteral("smoke.getValuesOfPERSVars(after)"), afterOk,
                       demoVectorToString(after));

        const QVector<QVariant> regVals = iface.getRegistedPERSVars();
        QStringList parts;
        for (int i = 0; i < regVals.size(); ++i) {
            parts << demoVariantToString(regVals.at(i));
        }
        result->append(QStringLiteral("smoke.getRegistedPERSVars"), !regVals.isEmpty(),
                       parts.isEmpty() ? QStringLiteral("<empty>") : parts.join(QStringLiteral(", ")));

        const bool allOk = (regRet == 0) && readOk && (setRet == 0) && afterOk;
        result->append(QStringLiteral("runPersSmokeTest"), allOk,
                       allOk ? QStringLiteral("PASS") : QStringLiteral("FAIL (check steps above)"));
    });
    layout.addButton(gPers, QStringLiteral("delVarData(PERS)"),
                     tr("Delete PERS variable by name."), [result, edPers]() {
        const QString name = edPers->text().trimmed();
        const bool ok = xplugin::xPluginInterface().delVarData(name);
        result->append(QStringLiteral("delVarData(PERS)"), ok, name);
    });

    QWidget *gPoint = layout.addGroup(tr("Point"));
    // ---- 查询（对齐 InterfaceManager 点位 CRUD）----
    layout.addButton(gPoint, QStringLiteral("getPointNameList()"), tr("List point names."), [result]() {
        result->append(QStringLiteral("getPointNameList"), true,
                       demoStringListToString(xplugin::xPluginInterface().getPointNameList()));
    });
    layout.addButton(gPoint, QStringLiteral("getPointObjDataByName()"), tr("Get point JSON by name."), [result, edPoint]() {
        const QJsonObject obj = xplugin::xPluginInterface().getPointObjDataByName(edPoint->text());
        QStringList keys;
        const QJsonObject value = obj.value(QStringLiteral("value")).toObject();
        foreach (const QString &k, value.keys()) {
            if (k.startsWith(QLatin1Char('['))) {
                keys.append(k);
            }
        }
        result->append(QStringLiteral("getPointObjDataByName"), !obj.isEmpty(),
                       QStringLiteral("property=%1 valueKeys=%2 | %3")
                           .arg(obj.value(QStringLiteral("property")).toString(),
                                keys.join(QLatin1Char(',')),
                                demoJsonToString(obj)));
    });
    layout.addButton(gPoint, QStringLiteral("getPointValueIndexList()"),
                     tr("List value indexes of a point array."), [result, edPoint]() {
        const QStringList keys = xplugin::xPluginInterface().getPointValueIndexList(edPoint->text());
        result->append(QStringLiteral("getPointValueIndexList"), true, demoStringListToString(keys));
    });
    layout.addButton(gPoint, QStringLiteral("getPointDimensions()"),
                     tr("Get point array dimensions d0/d1/d2."), [result, edPoint]() {
        const QVector<int> dims = xplugin::xPluginInterface().getPointDimensions(edPoint->text());
        QString tip;
        for (int i = 0; i < dims.size(); ++i) {
            if (i) tip += QLatin1Char(',');
            tip += QString::number(dims.at(i));
        }
        result->append(QStringLiteral("getPointDimensions"), !dims.isEmpty(), tip);
    });
    layout.addButton(gPoint, QStringLiteral("getPointDataByName()"),
                     tr("Get cart xyzabc at value index (cart points only; joint has no pose)."),
                     [result, edPoint, edValueIndex]() {
        const QString name = edPoint->text();
        const QString idx = edValueIndex->text();
        const QVector<double> data = xplugin::xPluginInterface().getPointDataByName(name, idx);
        QString tip = QStringLiteral("index=%1 data=%2").arg(idx, demoVectorToString(data));
        if (data.isEmpty()) {
            const QJsonObject obj = xplugin::xPluginInterface().getPointObjDataByName(name);
            tip += QStringLiteral(" | tip=need cart pose; property=%1")
                       .arg(obj.value(QStringLiteral("property")).toString());
        }
        result->append(QStringLiteral("getPointDataByName"), !data.isEmpty(), tip);
    });
    layout.addButton(gPoint, QStringLiteral("getPointJointDataByName()"),
                     tr("Get joint inner_pos at value index (joint points)."),
                     [result, edPoint, edValueIndex]() {
        const QVector<double> data = xplugin::xPluginInterface().getPointJointDataByName(
            edPoint->text(), edValueIndex->text());
        result->append(QStringLiteral("getPointJointDataByName"), !data.isEmpty(),
                       QStringLiteral("index=%1 data=%2")
                           .arg(edValueIndex->text(), demoVectorToString(data)));
    });
    layout.addButton(gPoint, QStringLiteral("getCurrentPointData()"), tr("Read current TCP pose data."), [result, edTool, edWobj]() {
        const QJsonValue v = xplugin::xPluginInterface().getCurrentPointData(edTool->text(), edWobj->text());
        result->append(QStringLiteral("getCurrentPointData"), !v.isNull() && !v.isUndefined(), demoJsonToString(v));
    });
    layout.addButton(gPoint, QStringLiteral("getPointCoord()"),
                     tr("Read tool/wobj name at value index (cart points)."),
                     [result, edPoint, edValueIndex]() {
        const QJsonObject coord = xplugin::xPluginInterface().getPointCoord(
            edPoint->text(), edValueIndex->text());
        result->append(QStringLiteral("getPointCoord"), !coord.isEmpty(),
                       QStringLiteral("%1 @ %2 | %3")
                           .arg(edPoint->text(), edValueIndex->text(), demoJsonToString(coord)));
    });

    // ---- 创建 / 示教 / 写入 ----
    layout.addButton(gPoint, QStringLiteral("createPoint(cart)"),
                     tr("Create empty cart point shell (isJoint=false, isPers=false)."), [result, edPoint]() {
        const bool ok = xplugin::xPluginInterface().createPoint(edPoint->text(), false, false);
        result->append(QStringLiteral("createPoint(cart)"), ok, edPoint->text());
    });
    layout.addButton(gPoint, QStringLiteral("createPoint(joint)"),
                     tr("Create empty joint point shell (isJoint=true, isPers=false)."), [result, edPoint]() {
        const bool ok = xplugin::xPluginInterface().createPoint(edPoint->text(), true, false);
        result->append(QStringLiteral("createPoint(joint)"), ok, edPoint->text());
    });
    layout.addButton(gPoint, QStringLiteral("createPoint(cart,pers)"),
                     tr("Create empty cart PERS point shell (isPers=true)."), [result, edPoint]() {
        const bool ok = xplugin::xPluginInterface().createPoint(edPoint->text(), false, true);
        result->append(QStringLiteral("createPoint(cart,pers)"), ok, edPoint->text());
    });
    layout.addButton(gPoint, QStringLiteral("ensurePointElement()"),
                     tr("Ensure array element exists at value index."), [result, edPoint, edValueIndex]() {
        const bool ok = xplugin::xPluginInterface().ensurePointElement(edPoint->text(), edValueIndex->text());
        result->append(QStringLiteral("ensurePointElement"), ok,
                       QStringLiteral("%1 @ %2").arg(edPoint->text(), edValueIndex->text()));
    });
    layout.addButton(gPoint, QStringLiteral("addCurrentPoint(joint)"),
                     tr("Save current pose as joint point (multi-dim updates one element)."),
                     [result, edPoint, edTool, edWobj, edValueIndex]() {
        const bool ok = xplugin::xPluginInterface().addCurrentPoint(
            edPoint->text(), true, edTool->text(), edWobj->text(), edValueIndex->text());
        result->append(QStringLiteral("addCurrentPoint(joint)"), ok,
                       QStringLiteral("%1 @ %2").arg(edPoint->text(), edValueIndex->text()));
    });
    layout.addButton(gPoint, QStringLiteral("addCurrentPoint(cart)"),
                     tr("Save current pose as cart point (multi-dim updates one element)."),
                     [result, edPoint, edTool, edWobj, edValueIndex]() {
        const bool ok = xplugin::xPluginInterface().addCurrentPoint(
            edPoint->text(), false, edTool->text(), edWobj->text(), edValueIndex->text());
        result->append(QStringLiteral("addCurrentPoint(cart)"), ok,
                       QStringLiteral("%1 @ %2").arg(edPoint->text(), edValueIndex->text()));
    });
    layout.addButton(gPoint, QStringLiteral("setPointCartData(+1mm X)"),
                     tr("Write cart xyzabc at value index (X+1)."), [result, edPoint, edValueIndex]() {
        const QVector<double> cur = xplugin::xPluginInterface().getPointDataByName(
            edPoint->text(), edValueIndex->text());
        QVector<double> xyz_abc;
        if (cur.size() >= 6) {
            xyz_abc = cur;
            xyz_abc[0] += 1.0;
        } else {
            xyz_abc << 1.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;
        }
        const bool ok = xplugin::xPluginInterface().setPointCartData(
            edPoint->text(), xyz_abc, edValueIndex->text());
        result->append(QStringLiteral("setPointCartData"), ok,
                       QStringLiteral("index=%1 xyzabc=%2")
                           .arg(edValueIndex->text(), demoVectorToString(xyz_abc)));
    });
    layout.addButton(gPoint, QStringLiteral("setPointJointData(zeros)"),
                     tr("Write joint zeros at value index."), [result, edPoint, edValueIndex]() {
        QVector<double> joints;
        joints << 0 << 0 << 0 << 0 << 0 << 0 << 0;
        const bool ok = xplugin::xPluginInterface().setPointJointData(
            edPoint->text(), joints, edValueIndex->text());
        result->append(QStringLiteral("setPointJointData"), ok,
                       QStringLiteral("index=%1").arg(edValueIndex->text()));
    });
    layout.addButton(gPoint, QStringLiteral("setPointElementJson(sample)"),
                     tr("Overwrite one value cell by JSON (advanced)."),
                     [result, edPoint, edValueIndex]() {
        QJsonObject cell;
        QJsonObject coord;
        coord.insert(QStringLiteral("tool"), QStringLiteral("tool0"));
        coord.insert(QStringLiteral("wobj"), QStringLiteral("wobj0"));
        cell.insert(QStringLiteral("coord"), coord);
        QJsonArray inner;
        for (int i = 0; i < 7; ++i) {
            inner.append(i == 0 ? 1.0 : 0.0);
        }
        cell.insert(QStringLiteral("inner_pos"), inner);
        QJsonArray ext;
        for (int i = 0; i < 6; ++i) {
            ext.append(0.0);
        }
        cell.insert(QStringLiteral("extern_pos"), ext);
        const bool ok = xplugin::xPluginInterface().setPointElementJson(
            edPoint->text(), edValueIndex->text(), cell);
        result->append(QStringLiteral("setPointElementJson"), ok,
                       QStringLiteral("%1 @ %2 | %3")
                           .arg(edPoint->text(), edValueIndex->text(), demoJsonToString(cell)));
    });
    layout.addButton(gPoint, QStringLiteral("setPointCoord(tool/wobj)"),
                     tr("Set tool/wobj name at value index (cart points only; joint rejected)."),
                     [result, edPoint, edValueIndex, edTool, edWobj]() {
        const bool ok = xplugin::xPluginInterface().setPointCoord(
            edPoint->text(), edValueIndex->text(), edTool->text(), edWobj->text());
        result->append(QStringLiteral("setPointCoord"), ok,
                       QStringLiteral("%1 @ %2 tool=%3 wobj=%4")
                           .arg(edPoint->text(), edValueIndex->text(), edTool->text(), edWobj->text()));
    });

    // ---- 删除 / 校验 / 运动 ----
    layout.addButton(gPoint, QStringLiteral("removePointElement()"),
                     tr("Remove one array element by value index."), [result, edPoint, edValueIndex]() {
        const bool ok = xplugin::xPluginInterface().removePointElement(edPoint->text(), edValueIndex->text());
        result->append(QStringLiteral("removePointElement"), ok,
                       QStringLiteral("%1 @ %2").arg(edPoint->text(), edValueIndex->text()));
    });
    layout.addButton(gPoint, QStringLiteral("removePoint()"),
                     tr("Delete whole point by name."), [result, edPoint]() {
        const bool ok = xplugin::xPluginInterface().removePoint(edPoint->text());
        result->append(QStringLiteral("removePoint"), ok, edPoint->text());
    });
    layout.addButton(gPoint, QStringLiteral("checkPointMoveTo()"), tr("Check reachability between points."), [result, edPointFrom, edPointTo]() {
        const int ret = xplugin::xPluginInterface().checkPointMoveTo(edPointFrom->text(), edPointTo->text());
        result->append(QStringLiteral("checkPointMoveTo"), true, QStringLiteral("ret=%1").arg(ret));
    });
    layout.addButton(gPoint, QStringLiteral("checkPointSame()"), tr("Check whether two points are same."), [result, edPointFrom, edPointTo]() {
        const int ret = xplugin::xPluginInterface().checkPointSame(edPointFrom->text(), edPointTo->text());
        result->append(QStringLiteral("checkPointSame"), true, QStringLiteral("ret=%1").arg(ret));
    });
    layout.addButton(gPoint, QStringLiteral("moveToStart()"), tr("Move to point (MoveJ sample)."), [result, edPoint, spSpeed]() {
        int err = 0;
        xplugin::xPluginInterface().moveToStart(err, edPoint->text(), spSpeed->value(), true);
        result->append(QStringLiteral("moveToStart"), err == 0, QStringLiteral("errCode=%1").arg(err));
    });
    layout.addButton(gPoint, QStringLiteral("moveToStop()"), tr("Stop moveTo motion."), [result]() {
        xplugin::xPluginInterface().moveToStop();
        result->append(QStringLiteral("moveToStop"), true, QString());
    });

    QWidget *gProj = layout.addGroup(tr("Project"));
    layout.addButton(gProj, QStringLiteral("getProjectList()"),
                     tr("List workspace projects and show current project."), [result]() {
        const QStringList projects = xplugin::xPluginInterface().getProjectList();
        const QString cur = xplugin::xPluginInterface().getCurProject();
        result->append(QStringLiteral("getProjectList"), true,
                       QStringLiteral("current=%1 | list=%2")
                           .arg(cur.isEmpty() ? QStringLiteral("(empty)") : cur,
                                demoStringListToString(projects)));
    });
    layout.addButton(gProj, QStringLiteral("createProject(isUsed=true)"),
                     tr("Create project and switch to it (isUsed=true)."), [result, edProject]() {
        const QString name = edProject->text().trimmed();
        const int ret = xplugin::xPluginInterface().createProject(name, QStringLiteral("demo"), true);
        QString tip = QStringLiteral("ret=%1").arg(ret);
        if (ret == -1) tip += QStringLiteral(" (invalid name)");
        else if (ret == -2) tip += QStringLiteral(" (already exists)");
        else if (ret == -3) tip += QStringLiteral(" (mkpath failed)");
        else if (ret == 0) tip += QStringLiteral(" current=%1").arg(xplugin::xPluginInterface().getCurProject());
        result->append(QStringLiteral("createProject"), ret == 0, tip);
    });
    layout.addButton(gProj, QStringLiteral("changeProject()"),
                     tr("Switch active project by name."), [result, edProject]() {
        const QString name = edProject->text().trimmed();
        const int ret = xplugin::xPluginInterface().changeProject(name);
        QString tip = QStringLiteral("ret=%1").arg(ret);
        if (ret == -1) tip += QStringLiteral(" (invalid name)");
        else if (ret == -2) tip += QStringLiteral(" (not found)");
        else if (ret == 0) tip += QStringLiteral(" current=%1").arg(xplugin::xPluginInterface().getCurProject());
        result->append(QStringLiteral("changeProject"), ret == 0, tip);
    });
    layout.addButton(gProj, QStringLiteral("removeProject()"),
                     tr("Delete project directory by name."), [result, edProject]() {
        const QString name = edProject->text().trimmed();
        const int ret = xplugin::xPluginInterface().removeProject(name);
        QString tip = QStringLiteral("ret=%1").arg(ret);
        if (ret == -1) tip += QStringLiteral(" (invalid name)");
        else if (ret == -2) tip += QStringLiteral(" (not found)");
        else if (ret == -3) tip += QStringLiteral(" (remove failed)");
        result->append(QStringLiteral("removeProject"), ret == 0, tip);
    });

    layout.finish();
}
