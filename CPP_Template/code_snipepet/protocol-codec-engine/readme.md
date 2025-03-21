# ProtocolCodec

用于帧协议编解码工具

## 使用方法

### 1. 定义协议格式

```cpp
ProtocolCodecEngine codecEngine;
//定义协议帧格式
codecEngine.frameDeclare("H(5AFF)S2CV(CRC16)E(FE)");
//定义校验使用的标志符
codecEngine.setVerifyFlags("SC");
//设置数据类型编码字节数，默认2字节
codecEngine.setTypeEncodeByteSize(3);
```
|标志符|说明|示例|
|:--:|:--|:--|
|H|header/帧头|5AFF|
|S|size/内容字节数大小|2|
|C|content/内容（隐式包含数据类型）||
|V|verify/校验类型|CRC16、SUM8、SUM16|
|E|end/帧尾|FE|

### 2. 注册数据类型

目前有三种内容编码方式，分别对应`JsonCodec`、`SerializeCodec`、`BytesCodec`:  

- json编码器：`JsonCodec<T>`

```cpp
struct DataType1 {
    enum {
        Type = 1 //必须设置，用于区分数据类型
    };

    int data = -1;

    //json数据解码
    static DataType1 fromJson(const QJsonDocument& doc) {
        DataType1 data;
        data.data = doc.object().value("d").toInt();
        return data;
    }

    //json数据编码
    QJsonDocument toJson() const {
        QJsonObject obj;
        obj.insert("d", data);
        return QJsonDocument(obj);
    }
};

//注册DataType1
class TestClass {
public:
    void registerType() {
        //定义协议帧格式
        codecEngine.frameDeclare("H(5AFF)S2CV(CRC16)E(FE)");
        //定义校验使用的标志符
        codecEngine.setVerifyFlags("SC");
        //注册类型
        codecEngine.registerType<JsonCodec<DataType1>>(this, &TestClass::dataType1Callback);
        //不使用解码器
        codecEngine.registerType<DataType1, JsonCodec>();
    }

    void dataType1Callback(const DataType1& data) {
        qDebug() << "data1 callback:" << data.data;
    }

private:
    ProtocolCodecEngine codecEngine;
}
```

- 序列化编码器：`SerializeCodec<T>`

```cpp
struct DataType2 {
    enum {
        Type = 2 //必须设置，用于区分数据类型
    };

    QString data;
};

//数据序列化
inline QDataStream& operator<<(QDataStream& out, const DataType2& data) {
    out << data.data;
    return out;
}

//数据反序列化
inline QDataStream& operator>>(QDataStream& in, DataType2& data) {
    in >> data.data;
    return in;
}

//注册DataType2
class TestClass {
public:
    void registerType() {
        //定义协议帧格式
        codecEngine.frameDeclare("H(5AFF)S2CV(CRC16)E(FE)");
        //定义校验使用的标志符
        codecEngine.setVerifyFlags("SC");
        //注册类型
        codecEngine.registerType<SerializeCodec<DataType2>>(this, &TestClass::dataType2Callback);
        //不使用解码器
        codecEngine.registerType<DataType2, SerializeCodec>();
    }

    void dataType2Callback(const DataType2& data) {
        qDebug() << "data2 callback:" << data.data;
    }

private:
    ProtocolCodecEngine codecEngine;
}
```

- 字节数组编码器：`BytesCodec`

```cpp
struct DataType3 {
    enum {
        Type = 3 //必须设置，用于区分数据类型
    };

    int data = 0;

    //字节数组转对象成员
    static DataType3 fromBytes(const QByteArray& content) {
        DataType3 data;
        memcpy(&data.data, content.data(), 4);
        return data;
    }

    //复制对象数据到字节数组
    QByteArray toBytes() const {
        char bytes[4];
        memcpy(bytes, (void*)&data, 4);
        return QByteArray(bytes, 4);
    }
};

//注册DataType3
class TestClass {
public:
    void registerType() {
        //定义协议帧格式
        codecEngine.frameDeclare("H(5AFF)S2CV(CRC16)E(FE)");
        //定义校验使用的标志符
        codecEngine.setVerifyFlags("SC");
        //注册类型
        codecEngine.registerType<BytesCodec<DataType2>>(this, &TestClass::dataType2Callback);
        //不使用解码器
        codecEngine.registerType<DataType2, BytesCodec>();
    }

    void dataType3Callback(const DataType3& data) {
        qDebug() << "data3 callback:" << data.data;
    }

private:
    ProtocolCodecEngine codecEngine;
}
```

### 3. 数据解析

将QByteArray数据传递给`ProtocolCodecEngine`即可，会自动回调到`registerType`时的回调函数。一般数据来源于串口、socket等等：

```cpp
codecEngine.appendBuffer(socket->readAll());
```

### 4. 数据编码

编码返回完整帧数据，传递对象即可：

```cpp
DataType1 data1{ 10 };
socket->write(codecEngine.encode(data1));

DataType2 data2{ "abcde" };
socket->write(codecEngine.encode(data2));
```

### 5. 空消息内容编解码

对于没有消息内容的指令编解码，使用其`registerType`重载函数：

```cpp
class TestClass {
public:
    void registerType() {
        //定义协议帧格式
        codecEngine.frameDeclare("H(5AFF)S2CV(CRC16)E(FE)");
        //定义校验使用的标志符
        codecEngine.setVerifyFlags("SC");
        //注册类型
        codecEngine.registerType<0x03>(this, &TestClass::dataType3Callback);
        //编码
        auto bytes = codecEngine.encode<0x03>();
    }

    void dataType3Callback() {
        qDebug() << "data3 callback!";
    }

private:
    ProtocolCodecEngine codecEngine;
}
```

### 6. 数据压缩编码器

如果在可预期的内容传输中字节数过多，可以使用数据压缩编码器`CompressCodec`，该编码器需要配合其他编码器一起使用：

```cpp
struct DataType4 {
    enum {
        Type = 4
    };

    QByteArray data;

    static DataType4 fromBytes(const QByteArray& content) {
        DataType4 data;
        data.data = content;
        return data;
    }

    QByteArray toBytes() const {
        return data;
    }
};

class TestClass {
public:
    void registerType() {
        //定义协议帧格式
        codecEngine.frameDeclare("H(5AFF)S4CV(CRC16)E(FE)");
        //定义校验使用的标志符
        codecEngine.setVerifyFlags("SC");
        //注册类型
        codecEngine.registerType<CompressCodec<DataType4, BytesCodec>>(this, &TestClass::dataType4Callback);
        //仅编码
        codecEngine.registerCompressEncoder<DataType4, BytesCodec>();
    }

    void dataType4Callback(const DataType4& data) {
        qDebug() << "data4 callback size:" << data.data.size();
    }

private:
    ProtocolCodecEngine codecEngine;
}
```

### 7. 自定义编解码器

继承`ProtocolTypeCodec<T>`编解码基类可以实现自己的编解码逻辑：

```cpp
template<typename T>
class MyCodec : public ProtocolTypeCodec<T> {
public:
    // 解码器按照帧协议格式解码成功会回调到该函数
    T decode(const QByteArray &content) override {
        // decode content bytes to type 'T'
    }
    
    // 编码器编码对象内容时会调用该函数
    QByteArray encode(const T &data) override {
        // encode type 'T' data to bytes
    }
};

```

## 未解决的问题

- 解析缓存无限制：当一直接收垃圾数据时，会在`decoder`内部一直缓存下来，直到解析到一个完整帧才会删除之前的