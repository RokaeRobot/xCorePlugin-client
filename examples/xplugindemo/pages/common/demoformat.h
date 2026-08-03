#ifndef DEMOFORMAT_H
#define DEMOFORMAT_H

#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>

inline QString demoJsonToString(const QJsonValue &v)
{
    if (v.isUndefined()) {
        return QStringLiteral("<undefined>");
    }
    if (v.isNull()) {
        return QStringLiteral("null");
    }
    if (v.isBool()) {
        return v.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    }
    if (v.isDouble()) {
        return QString::number(v.toDouble());
    }
    if (v.isString()) {
        return v.toString();
    }
    if (v.isArray()) {
        return QString::fromUtf8(QJsonDocument(v.toArray()).toJson(QJsonDocument::Compact));
    }
    if (v.isObject()) {
        return QString::fromUtf8(QJsonDocument(v.toObject()).toJson(QJsonDocument::Compact));
    }
    return QStringLiteral("<unknown>");
}

inline QString demoVariantToString(const QVariant &v)
{
    if (!v.isValid()) {
        return QStringLiteral("<invalid>");
    }
    if (v.type() == QVariant::StringList) {
        return v.toStringList().join(QStringLiteral(", "));
    }
    if (v.canConvert<QJsonValue>()) {
        return demoJsonToString(v.toJsonValue());
    }
    return v.toString();
}

inline QString demoStringListToString(const QStringList &list)
{
    if (list.isEmpty()) {
        return QStringLiteral("<empty>");
    }
    return list.join(QStringLiteral(", "));
}

template<typename T>
inline QString demoVectorToString(const QVector<T> &vec)
{
    QStringList parts;
    for (int i = 0; i < vec.size(); ++i) {
        parts << QVariant(vec.at(i)).toString();
    }
    if (parts.isEmpty()) {
        return QStringLiteral("<empty>");
    }
    return parts.join(QStringLiteral(", "));
}

inline QString demoHashVariantToString(const QHash<QString, QVariant> &hash)
{
    QStringList parts;
    for (auto it = hash.constBegin(); it != hash.constEnd(); ++it) {
        parts << QStringLiteral("%1=%2").arg(it.key(), demoVariantToString(it.value()));
    }
    if (parts.isEmpty()) {
        return QStringLiteral("<empty>");
    }
    return parts.join(QStringLiteral("; "));
}

inline QString demoHashStringListToString(const QHash<QString, QStringList> &hash)
{
    QStringList parts;
    for (auto it = hash.constBegin(); it != hash.constEnd(); ++it) {
        parts << QStringLiteral("%1=[%2]").arg(it.key(), it.value().join(QStringLiteral(",")));
    }
    if (parts.isEmpty()) {
        return QStringLiteral("<empty>");
    }
    return parts.join(QStringLiteral("; "));
}

#endif // DEMOFORMAT_H
