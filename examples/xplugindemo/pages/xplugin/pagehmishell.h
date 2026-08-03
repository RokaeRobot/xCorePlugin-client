#ifndef PAGEHMISHELL_H
#define PAGEHMISHELL_H

#include <QWidget>

/**
 * @brief 功能页: UI Layer
 *
 * 壳层 API
 * Layout: DemoPageLayout. UI strings: English tr() + ts.
 */
class PageHmiShell : public QWidget
{
    Q_OBJECT
public:
    explicit PageHmiShell(QWidget *parent = nullptr);
};

#endif // PAGEHMISHELL_H
