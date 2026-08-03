#ifndef DEMOSHELL_H
#define DEMOSHELL_H

#include <QWidget>

class QListWidget;
class QStackedWidget;

/**
 * @brief 中心页导航壳: nav list + stacked pages
 *
 * Nav titles use tr(); English source, zh via qm.
 */
class DemoShell : public QWidget
{
    Q_OBJECT
public:
    explicit DemoShell(QWidget *parent = nullptr);

private:
    void addPage(const QString &title, QWidget *page);

    QListWidget *m_nav;       ///< nav
    QStackedWidget *m_stack;  ///< pages
};

#endif // DEMOSHELL_H
