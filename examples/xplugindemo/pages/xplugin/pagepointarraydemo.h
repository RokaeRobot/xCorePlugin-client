#ifndef PAGEPOINTARRAYDEMO_H
#define PAGEPOINTARRAYDEMO_H

#include <QWidget>

/**
 * @brief Demo page: 2D point array as [refPoint][poseSlot]
 *
 * Mirrors customer scenario: array like [5][10] (5 reference points,
 * 10 poses each). Shows correct valueIndex usage (1-based HMI style)
 * and a "wrong pattern" button that always writes [ref][1].
 *
 * Layout: DemoPageLayout. UI strings: English tr() + ts.
 */
class PagePointArrayDemo : public QWidget
{
    Q_OBJECT
public:
    explicit PagePointArrayDemo(QWidget *parent = nullptr);
};

#endif // PAGEPOINTARRAYDEMO_H
