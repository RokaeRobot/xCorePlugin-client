#include "pagepointarraydemo.h"

#include "../common/demopagelayout.h"
#include "../common/demoformat.h"

#include "interface/interfacemanager.h"

#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

namespace {

/** HMI 二维点位下标：1-based，形如 "[2][3]"（宿主会规范成 "[2][3][0]"） */
QString makeValueIndex(int refPoint, int poseSlot)
{
    return QStringLiteral("[%1][%2]").arg(refPoint).arg(poseSlot);
}

QString dimsTip(const QVector<int> &dims)
{
    if (dims.isEmpty()) {
        return QStringLiteral("(empty)");
    }
    QString tip;
    for (int i = 0; i < dims.size(); ++i) {
        if (i) {
            tip += QLatin1Char(',');
        }
        tip += QString::number(dims.at(i));
    }
    return tip;
}

} // namespace

PagePointArrayDemo::PagePointArrayDemo(QWidget *parent)
    : QWidget(parent)
{
    DemoPageLayout layout = DemoPageLayout::build(this);
    ApiResultPanel *result = layout.result;

    QLineEdit *edPoint = new QLineEdit(QStringLiteral("ref_array"), this);
    QLineEdit *edTool = new QLineEdit(QStringLiteral("tool0"), this);
    QLineEdit *edWobj = new QLineEdit(QStringLiteral("wobj0"), this);

    QSpinBox *spD0 = new QSpinBox(this);
    spD0->setRange(1, 20);
    spD0->setValue(5);
    spD0->setToolTip(tr("First dimension size: number of reference points."));

    QSpinBox *spD1 = new QSpinBox(this);
    spD1->setRange(1, 50);
    spD1->setValue(10);
    spD1->setToolTip(tr("Second dimension size: poses per reference point."));

    QSpinBox *spRef = new QSpinBox(this);
    spRef->setRange(1, 20);
    spRef->setValue(2);
    spRef->setToolTip(tr("Current reference point index (1-based)."));

    QSpinBox *spPose = new QSpinBox(this);
    spPose->setRange(1, 50);
    spPose->setValue(1);
    spPose->setToolTip(tr("Current pose slot under the selected reference point (1-based)."));

    QLabel *lblIndex = new QLabel(this);
    lblIndex->setObjectName(QStringLiteral("demoActionApi"));
    lblIndex->setTextInteractionFlags(Qt::TextSelectableByMouse);

    auto refreshIndexLabel = [lblIndex, spRef, spPose]() {
        lblIndex->setText(tr("Current valueIndex = %1")
                              .arg(makeValueIndex(spRef->value(), spPose->value())));
    };
    refreshIndexLabel();
    connect(spRef, QOverload<int>::of(&QSpinBox::valueChanged), this, refreshIndexLabel);
    connect(spPose, QOverload<int>::of(&QSpinBox::valueChanged), this, refreshIndexLabel);

    auto syncSpinRanges = [spD0, spD1, spRef, spPose]() {
        spRef->setMaximum(spD0->value());
        spPose->setMaximum(spD1->value());
        if (spRef->value() > spRef->maximum()) {
            spRef->setValue(spRef->maximum());
        }
        if (spPose->value() > spPose->maximum()) {
            spPose->setValue(spPose->maximum());
        }
    };
    connect(spD0, QOverload<int>::of(&QSpinBox::valueChanged), this, syncSpinRanges);
    connect(spD1, QOverload<int>::of(&QSpinBox::valueChanged), this, syncSpinRanges);
    syncSpinRanges();

    QWidget *gParam = layout.addGroup(tr("Scenario parameters (customer: [5][10])"));
    {
        QFormLayout *form = new QFormLayout;
        form->addRow(tr("Point name"), edPoint);
        form->addRow(tr("Tool"), edTool);
        form->addRow(tr("Wobj"), edWobj);
        form->addRow(tr("Dim d0 (ref count)"), spD0);
        form->addRow(tr("Dim d1 (poses/ref)"), spD1);
        form->addRow(tr("Reference point #"), spRef);
        form->addRow(tr("Pose slot #"), spPose);
        form->addRow(tr("Resolved index"), lblIndex);
        qobject_cast<QVBoxLayout *>(gParam->layout())->addLayout(form);
    }

    QWidget *gPrep = layout.addGroup(tr("Prepare array cells"));
    layout.addButton(gPrep, QStringLiteral("createPoint(cart)"),
                     tr("Create empty cart shell if missing (host only creates [0][0][0])."),
                     [result, edPoint]() {
        const QString name = edPoint->text().trimmed();
        const bool ok = xplugin::xPluginInterface().createPoint(name, false, false);
        result->append(QStringLiteral("createPoint"), ok, name);
    });
    layout.addButton(gPrep, QStringLiteral("ensurePointElement(all cells)"),
                     tr("Ensure every [i][j] for i=1..d0, j=1..d1 exists (1-based)."),
                     [result, edPoint, spD0, spD1]() {
        const QString name = edPoint->text().trimmed();
        int okCount = 0;
        int failCount = 0;
        QStringList failed;
        for (int i = 1; i <= spD0->value(); ++i) {
            for (int j = 1; j <= spD1->value(); ++j) {
                const QString idx = makeValueIndex(i, j);
                if (xplugin::xPluginInterface().ensurePointElement(name, idx)) {
                    ++okCount;
                } else {
                    ++failCount;
                    if (failed.size() < 8) {
                        failed.append(idx);
                    }
                }
            }
        }
        const QVector<int> dims = xplugin::xPluginInterface().getPointDimensions(name);
        result->append(QStringLiteral("ensurePointElement(all)"),
                       failCount == 0,
                       QStringLiteral("ok=%1 fail=%2 dims=%3 failedSample=%4")
                           .arg(okCount)
                           .arg(failCount)
                           .arg(dimsTip(dims), failed.join(QLatin1Char(','))));
    });
    layout.addButton(gPrep, QStringLiteral("getPointDimensions()"),
                     tr("Read d0/d1/d2 metadata (host may not update d after ensure)."),
                     [result, edPoint]() {
        const QVector<int> dims = xplugin::xPluginInterface().getPointDimensions(edPoint->text());
        result->append(QStringLiteral("getPointDimensions"), !dims.isEmpty(), dimsTip(dims));
    });
    layout.addButton(gPrep, QStringLiteral("getPointValueIndexList()"),
                     tr("List all value keys currently under the point."),
                     [result, edPoint]() {
        const QStringList keys = xplugin::xPluginInterface().getPointValueIndexList(edPoint->text());
        result->append(QStringLiteral("getPointValueIndexList"), true,
                       QStringLiteral("count=%1 | %2")
                           .arg(keys.size())
                           .arg(keys.join(QLatin1Char(','))));
    });

    QWidget *gSave = layout.addGroup(tr("Save poses (problem #1 scenario)"));
    layout.addButton(gSave, QStringLiteral("addCurrentPoint(cart) @ current index"),
                     tr("CORRECT: save TCP into [ref][pose] built from spin boxes."),
                     [result, edPoint, edTool, edWobj, spRef, spPose]() {
        const QString name = edPoint->text().trimmed();
        const QString idx = makeValueIndex(spRef->value(), spPose->value());
        const bool ensured = xplugin::xPluginInterface().ensurePointElement(name, idx);
        const bool ok = xplugin::xPluginInterface().addCurrentPoint(
            name, false, edTool->text(), edWobj->text(), idx);
        result->append(QStringLiteral("addCurrentPoint(cart)"), ok && ensured,
                       QStringLiteral("%1 @ %2 ensure=%3")
                           .arg(name, idx)
                           .arg(ensured ? QStringLiteral("ok") : QStringLiteral("fail")));
    });
    layout.addButton(gSave, QStringLiteral("Save 3 poses along pose slot (auto +1)"),
                     tr("CORRECT batch: for current ref, write pose 1..3 (or from current slot)."),
                     [result, edPoint, edTool, edWobj, spRef, spPose, spD1]() {
        const QString name = edPoint->text().trimmed();
        const int ref = spRef->value();
        int start = spPose->value();
        if (start + 2 > spD1->value()) {
            start = qMax(1, spD1->value() - 2);
            spPose->setValue(start);
        }
        QStringList lines;
        bool allOk = true;
        for (int k = 0; k < 3; ++k) {
            const int pose = start + k;
            const QString idx = makeValueIndex(ref, pose);
            const bool ensured = xplugin::xPluginInterface().ensurePointElement(name, idx);
            const bool ok = xplugin::xPluginInterface().addCurrentPoint(
                name, false, edTool->text(), edWobj->text(), idx);
            allOk = allOk && ensured && ok;
            lines.append(QStringLiteral("%1 -> %2").arg(idx, (ensured && ok) ? QStringLiteral("ok")
                                                                             : QStringLiteral("FAIL")));
        }
        spPose->setValue(qMin(start + 2, spD1->value()));
        const QStringList keys = xplugin::xPluginInterface().getPointValueIndexList(name);
        result->append(QStringLiteral("save 3 poses"), allOk,
                       lines.join(QStringLiteral("; ")) + QStringLiteral(" | keys=") + keys.join(QLatin1Char(',')));
    });
    layout.addButton(gSave, QStringLiteral("WRONG: always write [ref][1] three times"),
                     tr("Reproduces customer bug #1 if caller forgets to bump second index."),
                     [result, edPoint, edTool, edWobj, spRef]() {
        const QString name = edPoint->text().trimmed();
        const int ref = spRef->value();
        const QString idx = makeValueIndex(ref, 1);
        QStringList lines;
        for (int k = 0; k < 3; ++k) {
            const bool ensured = xplugin::xPluginInterface().ensurePointElement(name, idx);
            const bool ok = xplugin::xPluginInterface().addCurrentPoint(
                name, false, edTool->text(), edWobj->text(), idx);
            lines.append(QStringLiteral("#%1 %2 -> %3")
                             .arg(k + 1)
                             .arg(idx, (ensured && ok) ? QStringLiteral("overwrite")
                                                       : QStringLiteral("FAIL")));
        }
        result->append(QStringLiteral("wrong always [ref][1]"), true,
                       tr("Only %1 is updated; [ref][2]/[ref][3] stay empty if never written. | %2")
                           .arg(idx, lines.join(QStringLiteral("; "))));
    });

    QWidget *gNav = layout.addGroup(tr("Increment dimensions (problem #2 scenario)"));
    layout.addButton(gNav, QStringLiteral("Increment reference point (d0 / first dim)"),
                     tr("ref = ref+1, then ensure [ref][1]. Host may fail if d.#0 not updated."),
                     [result, edPoint, spRef, spD0, spPose]() {
        if (spRef->value() >= spD0->value()) {
            result->append(QStringLiteral("inc ref"), false,
                           tr("Already at max ref=%1 (d0=%2). Increase d0 or reset.")
                               .arg(spRef->value())
                               .arg(spD0->value()));
            return;
        }
        spRef->setValue(spRef->value() + 1);
        spPose->setValue(1);
        const QString name = edPoint->text().trimmed();
        const QString idx = makeValueIndex(spRef->value(), spPose->value());
        const bool ok = xplugin::xPluginInterface().ensurePointElement(name, idx);
        const QVector<int> dims = xplugin::xPluginInterface().getPointDimensions(name);
        result->append(QStringLiteral("inc ref + ensure"), ok,
                       QStringLiteral("now %1 ensure=%2 dims=%3")
                           .arg(idx)
                           .arg(ok ? QStringLiteral("ok") : QStringLiteral("FAIL"))
                           .arg(dimsTip(dims)));
    });
    layout.addButton(gNav, QStringLiteral("Increment pose slot (d1 / second dim)"),
                     tr("pose = pose+1 under current ref, then ensure that cell."),
                     [result, edPoint, spRef, spPose, spD1]() {
        if (spPose->value() >= spD1->value()) {
            result->append(QStringLiteral("inc pose"), false,
                           tr("Already at max pose=%1 (d1=%2).")
                               .arg(spPose->value())
                               .arg(spD1->value()));
            return;
        }
        spPose->setValue(spPose->value() + 1);
        const QString name = edPoint->text().trimmed();
        const QString idx = makeValueIndex(spRef->value(), spPose->value());
        const bool ok = xplugin::xPluginInterface().ensurePointElement(name, idx);
        result->append(QStringLiteral("inc pose + ensure"), ok,
                       QStringLiteral("now %1 ensure=%2")
                           .arg(idx, ok ? QStringLiteral("ok") : QStringLiteral("FAIL")));
    });
    layout.addButton(gNav, QStringLiteral("Walk all refs: ensure [i][1]"),
                     tr("For i=1..d0 ensure first pose of each ref (first-dim stress test)."),
                     [result, edPoint, spD0]() {
        const QString name = edPoint->text().trimmed();
        QStringList lines;
        bool allOk = true;
        for (int i = 1; i <= spD0->value(); ++i) {
            const QString idx = makeValueIndex(i, 1);
            const bool ok = xplugin::xPluginInterface().ensurePointElement(name, idx);
            allOk = allOk && ok;
            lines.append(QStringLiteral("%1=%2").arg(idx, ok ? QStringLiteral("ok") : QStringLiteral("FAIL")));
        }
        const QVector<int> dims = xplugin::xPluginInterface().getPointDimensions(name);
        result->append(QStringLiteral("walk refs [i][1]"), allOk,
                       lines.join(QStringLiteral("; ")) + QStringLiteral(" | dims=") + dimsTip(dims));
    });

    QWidget *gRead = layout.addGroup(tr("Inspect cell"));
    layout.addButton(gRead, QStringLiteral("getPointDataByName(current index)"),
                     tr("Read cart xyzabc at current [ref][pose]."),
                     [result, edPoint, spRef, spPose]() {
        const QString idx = makeValueIndex(spRef->value(), spPose->value());
        const QVector<double> data =
            xplugin::xPluginInterface().getPointDataByName(edPoint->text(), idx);
        result->append(QStringLiteral("getPointDataByName"), !data.isEmpty(),
                       QStringLiteral("index=%1 data=%2").arg(idx, demoVectorToString(data)));
    });
    layout.addButton(gRead, QStringLiteral("Snapshot keys containing [ref]"),
                     tr("Filter value keys whose first index equals current ref."),
                     [result, edPoint, spRef]() {
        const int ref = spRef->value();
        const QString prefix = QStringLiteral("[%1][").arg(ref);
        const QStringList keys = xplugin::xPluginInterface().getPointValueIndexList(edPoint->text());
        QStringList hit;
        foreach (const QString &k, keys) {
            if (k.startsWith(prefix)) {
                hit.append(k);
            }
        }
        result->append(QStringLiteral("keys for ref"), true,
                       QStringLiteral("ref=%1 count=%2 | %3")
                           .arg(ref)
                           .arg(hit.size())
                           .arg(hit.join(QLatin1Char(','))));
    });

    layout.finish();
}
