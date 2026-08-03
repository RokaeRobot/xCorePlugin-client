#ifndef DEMOSTYLE_H
#define DEMOSTYLE_H

#include <QFile>
#include <QString>
#include <QStyle>
#include <QWidget>

/**
 * @brief Load xMate-like QSS bundled in :/xplugindemo/render/demo_xmate.qss
 */
inline QString demoLoadXmateStyleSheet()
{
    QFile file(QStringLiteral(":/xplugindemo/render/demo_xmate.qss"));
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

inline void demoApplyXmateStyle(QWidget *root)
{
    if (!root) {
        return;
    }
    const QString qss = demoLoadXmateStyleSheet();
    if (qss.isEmpty()) {
        return;
    }
    root->setStyleSheet(qss);
    root->style()->unpolish(root);
    root->style()->polish(root);
    const QList<QWidget *> kids = root->findChildren<QWidget *>();
    for (int i = 0; i < kids.size(); ++i) {
        QWidget *w = kids.at(i);
        w->style()->unpolish(w);
        w->style()->polish(w);
    }
}

#endif // DEMOSTYLE_H
