#ifndef EXPANDWIDGET_H
#define EXPANDWIDGET_H

#include <QWidget>

class QPushButton;

/**
 * @brief 右侧拓展 demo
 *
 * CreateRightExpandWidgte; clicks report to ApiResultPanel::appendToActive.
 */
class ExpandWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ExpandWidget(QWidget *parent = nullptr);

private:
    void applyStyle();
    QPushButton *addActionBtn(const QString &text, bool primary = false);
    void reportClick(const QString &action, const QString &detail = QString());

private slots:
    void onDemoClicked();
    void onCloseClicked();
    void onActionClicked();
};

#endif // EXPANDWIDGET_H
