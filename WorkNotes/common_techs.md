### uuid
```cpp

#include <QCoreApplication>
#include <QUuid>
#include <QDateTime>
#include <QCryptographicHash>
#include <QDebug>

QUuid generateCustomUuid(const QString &value) {
    // 获取当前时间戳
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();

    // 将时间戳和输入值组合成一个字符串
    QString combined = QString::number(timestamp) + value;

    // 使用 SHA-256 哈希函数将组合值转换为哈希值
    QByteArray hash = QCryptographicHash::hash(combined.toUtf8(), QCryptographicHash::Sha256);

    // 取前 16 个字节（128 位）作为 UUID 的内容
    QByteArray uuidBytes = hash.left(16);

    // 将字节数组转换为 QUuid 对象
    QUuid uuid = QUuid::fromRfc4122(uuidBytes);
    return uuid;
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QString value = "example_value";
    QUuid uuid = generateCustomUuid(value);

    qDebug() << "Generated custom UUID:" << uuid.toString();

    return a.exec();
}

```

