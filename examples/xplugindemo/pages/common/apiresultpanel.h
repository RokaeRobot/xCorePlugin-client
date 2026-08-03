#ifndef APIRESULTPANEL_H
#define APIRESULTPANEL_H

#include <QWidget>
#include <QString>

class QPlainTextEdit;
class QPushButton;
class QShowEvent;

/**
 * @brief 执行结果 panel
 *
 * append()/appendInfo(); active panel for ExpandWidget via appendToActive().
 */
class ApiResultPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ApiResultPanel(QWidget *parent = nullptr);
    ~ApiResultPanel() override;

    void clear();
    void append(const QString &api, bool ok, const QString &detail);
    void appendInfo(const QString &msg);

    static ApiResultPanel *active();
    static void appendToActive(const QString &api, bool ok, const QString &detail);
    static void appendInfoToActive(const QString &msg);

protected:
    void showEvent(QShowEvent *event) override;

private:
    void setAsActive();

    QPlainTextEdit *m_edit;
    QPushButton *m_btnClear;

    static ApiResultPanel *s_active;
};

#endif // APIRESULTPANEL_H
