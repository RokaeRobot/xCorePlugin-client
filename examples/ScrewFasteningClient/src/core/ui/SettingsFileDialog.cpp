/**
 * Copyright (c) 2026 ROKAE (Beijing) Technology Co., Ltd. All Rights Reserved.
 *
 * @file SettingsFileDialog.cpp
 * @brief 在选定的存储设备内选择配置文件。
 */

#include "SettingsFileDialog.h"
#include <QComboBox>
#include <QCoreApplication>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPointer>
#include <QProcess>
#include <QPushButton>
#include <QRegExp>
#include <QScreen>
#include <QSettings>
#include <QStorageInfo>
#include <QTimer>
#include <QVBoxLayout>
#include <QWindow>

namespace screw {
namespace {

// 路径必须位于指定目录内，名称相似的相邻目录不属于指定目录。
bool insideDirectory(const QString &path, const QString &root)
{
    return !path.isEmpty() && !root.isEmpty() &&
           (path == root || path.startsWith(root + QLatin1Char('/')));
}

bool sameMountedDevice(const SettingsFileSelection &selection, bool importing)
{
    for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {
        if (storage.rootPath() == selection.mountPath && storage.device() == selection.device &&
            storage.isValid() && storage.isReady())
            return importing || !storage.isReadOnly();
    }
    return false;
}

#ifndef Q_OS_WIN
// 直接读取设备列表，避免依赖命令解释器和文本筛选管道。
bool removableUsbPresent()
{
    QProcess process;
    process.start(QStringLiteral("lsblk"),
                  {QStringLiteral("--nodeps"), QStringLiteral("--noheadings"),
                   QStringLiteral("--output"), QStringLiteral("TRAN,RM")});
    if (!process.waitForStarted(1000) || !process.waitForFinished(2000)) {
        process.kill();
        process.waitForFinished(500);
        return false;
    }
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0)
        return false;
    for (const QString &line : QString::fromLocal8Bit(process.readAllStandardOutput()).split(QLatin1Char('\n'))) {
        const QStringList columns = line.simplified().split(QLatin1Char(' '));
        if (columns.size() == 2 && columns.at(0) == QLatin1String("usb") && columns.at(1) == QLatin1String("1"))
            return true;
    }
    return false;
}

// 部署目录仅筛选已就绪挂载点，不逐一校验其与 USB 设备的对应关系；普通目录不作为设备。
QList<QStorageInfo> availableVolumes()
{
    const QString configPath = QDir(QCoreApplication::applicationDirPath()).filePath(
        QStringLiteral("xplugins/client/") + QString::fromLatin1(PLUGIN_NAME) + QStringLiteral("/storage.ini"));
    const QSettings deployment(configPath, QSettings::IniFormat);
    const QStringList defaults{QStringLiteral("/media/vantron"), QStringLiteral("/media/nodka")};
    const QStringList searchRoots = deployment.value(QStringLiteral("storage/searchRoots"), defaults).toStringList();
    QList<QStorageInfo> result;
    for (const QStorageInfo &storage : QStorageInfo::mountedVolumes()) {
        if (!storage.isValid() || !storage.isReady())
            continue;
        const QString mount = QFileInfo(storage.rootPath()).canonicalFilePath();
        for (const QString &root : searchRoots) {
            if (insideDirectory(mount, QFileInfo(root.trimmed()).canonicalFilePath())) {
                result.append(storage);
                break;
            }
        }
    }
    return result;
}

class SettingsStorageDialog : public QDialog
{
public:
    SettingsStorageDialog(QWidget *parent, bool importing, const QList<QStorageInfo> &volumes)
        : QDialog(parent), m_importing(importing), m_volumes(volumes)
    {
        setWindowTitle(importing ? tr("导入配置") : tr("导出配置"));
        setWindowFlag(Qt::WindowContextHelpButtonHint, false);
        setFont(parent->font());
        setStyleSheet(QStringLiteral("QPushButton { min-height: 40px; min-width: 80px; }"
                                    "QComboBox, QLineEdit { min-height: 36px; }"
                                    "QListWidget::item { min-height: 40px; }"));
        auto *layout = new QVBoxLayout(this);
        auto *location = new QHBoxLayout;
        location->addWidget(new QLabel(tr("U 盘："), this));
        m_volume = new QComboBox(this);
        m_volume->setObjectName(QStringLiteral("settings_storage_volume"));
        for (const QStorageInfo &storage : volumes)
            m_volume->addItem(QStringLiteral("%1 (%2)%3").arg(storage.displayName(), storage.rootPath(),
                storage.isReadOnly() ? tr(" [只读]") : QString()));
        location->addWidget(m_volume, 1);
        m_back = new QPushButton(tr("返回上一级"), this);
        location->addWidget(m_back);
        auto *refresh = new QPushButton(tr("刷新"), this);
        location->addWidget(refresh);
        layout->addLayout(location);
        m_location = new QLabel(this);
        m_location->setWordWrap(true);
        m_location->setTextFormat(Qt::PlainText);
        layout->addWidget(m_location);
        m_files = new QListWidget(this);
        m_files->setObjectName(QStringLiteral("settings_storage_files"));
        layout->addWidget(m_files, 1);
        auto *nameRow = new QHBoxLayout;
        nameRow->addWidget(new QLabel(tr("文件名："), this));
        m_name = new QLineEdit(this);
        m_name->setObjectName(QStringLiteral("settings_storage_filename"));
        m_name->setReadOnly(importing);
        m_name->setMaxLength(200);
        if (!importing)
            m_name->setText(QStringLiteral("settings.json"));
        nameRow->addWidget(m_name, 1);
        layout->addLayout(nameRow);
        auto *buttons = new QHBoxLayout;
        buttons->addStretch();
        auto *cancel = new QPushButton(tr("取消"), this);
        m_confirm = new QPushButton(importing ? tr("导入") : tr("导出"), this);
        m_confirm->setObjectName(QStringLiteral("settings_storage_confirm"));
        buttons->addWidget(cancel);
        buttons->addWidget(m_confirm);
        layout->addLayout(buttons);
        connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
        connect(m_confirm, &QPushButton::clicked, this, [this]() { confirm(); });
        connect(m_back, &QPushButton::clicked, this, [this]() {
            QDir directory(m_directory);
            if (directory.cdUp())
                showDirectory(directory.absolutePath());
        });
        connect(refresh, &QPushButton::clicked, this, [this]() { showDirectory(m_directory); });
        connect(m_volume, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, [this](int index) { selectVolume(index); });
        connect(m_files, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
            if (m_importing)
                m_name->setText(item->data(Qt::UserRole + 1).toBool() ? QString() : item->text());
        });
        connect(m_files, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
            if (item->data(Qt::UserRole + 1).toBool())
                showDirectory(item->data(Qt::UserRole).toString());
            else if (m_importing) {
                m_name->setText(item->text());
                confirm();
            }
        });
        m_timer.setInterval(1000);
        connect(&m_timer, &QTimer::timeout, this, [this]() { checkStorage(); });
        selectVolume(0);
        m_timer.start();
        // 示教器使用 Qt 5.12，通过 QWindow 获取屏幕，避免调用 Qt 5.14 才提供的 QWidget::screen()。
        QScreen *screen = nullptr;
        if (parent->window() != nullptr && parent->window()->windowHandle() != nullptr)
            screen = parent->window()->windowHandle()->screen();
        if (screen == nullptr)
            screen = QGuiApplication::primaryScreen();
        const QSize available = screen != nullptr ? screen->availableGeometry().size() - QSize(32, 32)
                                                   : QSize(820, 540);
        resize(QSize(820, 540).boundedTo(available));
    }

    SettingsFileSelection selection() const { return m_selection; }

private:
    void selectVolume(int index)
    {
        const QStorageInfo &storage = m_volumes.at(index);
        m_selection = {QString(), storage.rootPath(), storage.device()};
        m_confirm->setEnabled(m_importing || !storage.isReadOnly());
        showDirectory(storage.rootPath());
    }

    bool checkStorage()
    {
        if (sameMountedDevice(m_selection, true))
            return true;
        m_timer.stop();
        m_confirm->setEnabled(false);
        QMessageBox::warning(this, tr("U 盘不可用"), tr("U 盘已移除或挂载位置已变化，请重新选择。"));
        reject();
        return false;
    }

    void showDirectory(const QString &path)
    {
        if (!checkStorage())
            return;
        const QString actual = QFileInfo(path).canonicalFilePath();
        const QString root = QFileInfo(m_selection.mountPath).canonicalFilePath();
        if (!insideDirectory(actual, root) || !QFileInfo(actual).isDir()) {
            QMessageBox::warning(this, tr("目录不可用"), tr("只能访问选定 U 盘内的目录。"));
            return;
        }
        m_directory = actual;
        m_location->setText(tr("当前位置：%1").arg(QDir(root).relativeFilePath(actual)));
        m_back->setEnabled(actual != root);
        m_files->clear();
        if (m_importing)
            m_name->clear();
        const QDir directory(actual);
        const auto entries = directory.entryInfoList(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot,
                                                     QDir::DirsFirst | QDir::Name | QDir::IgnoreCase);
        for (const QFileInfo &entry : entries) {
            // 文件窗口不显示符号链接，避免通过链接访问选定设备以外的文件。
            if (entry.isSymLink() || (!entry.isDir() && entry.suffix().compare(QLatin1String("json"), Qt::CaseInsensitive) != 0))
                continue;
            auto *item = new QListWidgetItem(entry.fileName(), m_files);
            item->setData(Qt::UserRole, entry.absoluteFilePath());
            item->setData(Qt::UserRole + 1, entry.isDir());
            if (entry.isDir())
                item->setText(entry.fileName() + QLatin1Char('/'));
        }
    }

    void confirm()
    {
        if (!checkStorage())
            return;
        QString name = m_name->text().trimmed();
        if (name.isEmpty() || name == QLatin1String(".") || name.contains(QStringLiteral("..")) ||
            name.contains(QLatin1Char('/')) || name.contains(QLatin1Char('\\'))) {
            QMessageBox::warning(this, tr("文件名不正确"), tr("请填写文件名，文件名不能包含目录路径或连续两个句点。"));
            return;
        }
        if (!name.endsWith(QLatin1String(".json"), Qt::CaseInsensitive))
            name += QStringLiteral(".json");
        m_selection.path = QDir(m_directory).filePath(name);
        if (!settingsFileAvailable(m_selection, m_importing)) {
            QMessageBox::warning(this, tr("文件不可用"), tr("配置文件不存在、保存位置不可写或文件路径超出了选定 U 盘。"));
            return;
        }
        if (!m_importing && QFileInfo::exists(m_selection.path)) {
            if (QMessageBox::question(this, tr("覆盖配置"), tr("同名配置文件已经存在，是否覆盖？"),
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
                return;
        }
        if (settingsFileAvailable(m_selection, m_importing))
            accept();
        else
            QMessageBox::warning(this, tr("文件不可用"), tr("存储设备或文件路径已变化，请重新选择。"));
    }

    bool m_importing;
    QList<QStorageInfo> m_volumes; // 打开窗口时找到的挂载设备，拔出后需要重新打开窗口。
    SettingsFileSelection m_selection;
    QString m_directory; // 当前正在浏览的目录，始终位于选定设备内。
    QComboBox *m_volume;
    QListWidget *m_files;
    QLabel *m_location;
    QLineEdit *m_name;
    QPushButton *m_back;
    QPushButton *m_confirm;
    QTimer m_timer;
};
#endif
} // namespace

bool settingsFileAvailable(const SettingsFileSelection &selection, bool importing)
{
    if (selection.path.isEmpty())
        return false;
    const QFileInfo target(selection.path);
    if (selection.mountPath.isEmpty())
        return importing ? target.isFile() && target.isReadable() : QFileInfo(target.absolutePath()).isWritable();
    if (!sameMountedDevice(selection, importing) || target.isSymLink())
        return false;
    const QString root = QFileInfo(selection.mountPath).canonicalFilePath();
    const QString parent = QFileInfo(target.absolutePath()).canonicalFilePath();
    if (!insideDirectory(parent, root))
        return false;
    if (importing)
        return target.isFile() && target.isReadable() && insideDirectory(target.canonicalFilePath(), root);
    return QFileInfo(parent).isWritable() && (!target.exists() || (target.isFile() && target.isWritable()));
}

SettingsFileSelection selectSettingsFile(QWidget *parent, bool importing)
{
    if (parent == nullptr)
        return {};
#ifdef Q_OS_WIN
    QFileDialog dialog(parent, importing ? QObject::tr("导入配置") : QObject::tr("导出配置"));
    dialog.setNameFilter(QObject::tr("配置文件 (*.json)"));
    dialog.setDefaultSuffix(QStringLiteral("json"));
    dialog.setFileMode(importing ? QFileDialog::ExistingFile : QFileDialog::AnyFile);
    dialog.setAcceptMode(importing ? QFileDialog::AcceptOpen : QFileDialog::AcceptSave);
    if (!importing)
        dialog.selectFile(QStringLiteral("settings.json"));
    if (dialog.exec() == QDialog::Accepted && !dialog.selectedFiles().isEmpty())
        return {dialog.selectedFiles().first(), QString(), QByteArray()};
#else
    if (!removableUsbPresent()) {
        QMessageBox::information(parent, QObject::tr("U 盘不可用"),
                                 QObject::tr("未检测到可移除的 USB 存储设备，请插入 U 盘。"));
        return {};
    }
    const auto volumes = availableVolumes();
    if (volumes.isEmpty()) {
        QMessageBox::information(parent, QObject::tr("U 盘不可用"),
                                 QObject::tr("U 盘尚未挂载到可用位置，请等待设备就绪后重试。"));
        return {};
    }
    SettingsStorageDialog dialog(parent, importing, volumes);
    if (dialog.exec() == QDialog::Accepted)
        return dialog.selection();
#endif
    return {};
}

} // namespace screw
