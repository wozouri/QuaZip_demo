// ==========================================
// QuaZip 简单示例项目
// ==========================================

// main.cpp
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

// QuaZip v1.5 头文件
#include <quazip.h>
#include <quazipfile.h>
#include <quazipdir.h>
#include <quazipfileinfo.h>

class QuaZipDemo : public QWidget
{
    Q_OBJECT

public:
    QuaZipDemo(QWidget *parent = nullptr) : QWidget(parent)
    {
        setupUI();
        connectSignals();
    }

private slots:
    void createZipFile()
    {
        QString zipPath = QFileDialog::getSaveFileName(this,
                                                       "保存ZIP文件", "", "ZIP Files (*.zip)");

        if (zipPath.isEmpty()) return;

        // 创建ZIP文件
        QuaZip zip(zipPath);
        if (!zip.open(QuaZip::mdCreate)) {
            QMessageBox::warning(this, "错误", "无法创建ZIP文件");
            return;
        }

        // 添加几个测试文件
        addFileToZip(zip, "test1.txt", "这是第一个测试文件的内容\nHello QuaZip!");
        addFileToZip(zip, "test2.txt", "这是第二个测试文件\n包含中文内容");
        addFileToZip(zip, "folder/test3.txt", "this file in child fold ");

        zip.close();

        logOutput->append(QString("✅ success to creat zip file: %1").arg(zipPath));
        logOutput->append(QString("📁 have 3 files"));
    }

    void extractZipFile()
    {
        QString zipPath = QFileDialog::getOpenFileName(this,
                                                       "选择ZIP文件", "", "ZIP Files (*.zip)");

        if (zipPath.isEmpty()) return;

        QString extractDir = QFileDialog::getExistingDirectory(this, "选择解压目录");
        if (extractDir.isEmpty()) return;

        // 打开ZIP文件
        QuaZip zip(zipPath);
        if (!zip.open(QuaZip::mdUnzip)) {
            QMessageBox::warning(this, "错误", "无法打开ZIP文件");
            return;
        }

        logOutput->append(QString("📂 正在解压: %1").arg(zipPath));
        logOutput->append(QString("📁 解压到: %1").arg(extractDir));

        // 解压所有文件
        QuaZipFile file(&zip);
        for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
            QString fileName = zip.getCurrentFileName();
            QString filePath = extractDir + "/" + fileName;

            // 创建目录
            QDir().mkpath(QFileInfo(filePath).absolutePath());

            // 解压文件
            if (!file.open(QIODevice::ReadOnly)) {
                logOutput->append(QString("❌ 无法打开文件: %1").arg(fileName));
                continue;
            }

            QFile outFile(filePath);
            if (!outFile.open(QIODevice::WriteOnly)) {
                logOutput->append(QString("❌ 无法创建文件: %1").arg(filePath));
                file.close();
                continue;
            }

            outFile.write(file.readAll());
            outFile.close();
            file.close();

            logOutput->append(QString("✅ 解压文件: %1").arg(fileName));
        }

        zip.close();
        logOutput->append("🎉 解压完成!");
    }

    void listZipContents()
    {
        QString zipPath = QFileDialog::getOpenFileName(this,
                                                       "选择ZIP文件", "", "ZIP Files (*.zip)");

        if (zipPath.isEmpty()) return;

        QuaZip zip(zipPath);
        if (!zip.open(QuaZip::mdUnzip)) {
            QMessageBox::warning(this, "错误", "无法打开ZIP文件");
            return;
        }

        logOutput->append(QString("📋 ZIP文件内容: %1").arg(zipPath));
        logOutput->append(QString(50, '='));

        // 列出所有文件
        QuaZipFileInfo64 info;
        for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
            if (zip.getCurrentFileInfo(&info)) {
                QString size = QString::number(info.uncompressedSize);
                QString compSize = QString::number(info.compressedSize);
                QString ratio = QString::number(
                    100.0 * (1.0 - (double)info.compressedSize / info.uncompressedSize), 'f', 1);

                logOutput->append(QString("📄 %1").arg(info.name));
                logOutput->append(QString("   大小: %1 字节 (压缩后: %2 字节, 压缩率: %3%)")
                                      .arg(size).arg(compSize).arg(ratio));
                logOutput->append(QString("   修改时间: %1")
                                      .arg(info.dateTime.toString("yyyy-MM-dd hh:mm:ss")));
                logOutput->append("");
            }
        }

        zip.close();
        logOutput->append(QString(50, '='));
        logOutput->append("📊 列表完成");
    }

    void clearLog()
    {
        logOutput->clear();
        logOutput->append("QuaZip v1.5 演示程序");
        logOutput->append("支持创建、解压和查看ZIP文件");
        logOutput->append(QString(40, '='));
    }

private:
    void setupUI()
    {
        setWindowTitle("QuaZip v1.5 演示程序");
        setMinimumSize(600, 400);

        QVBoxLayout *layout = new QVBoxLayout(this);

        // 按钮
        createZipBtn = new QPushButton("创建 ZIP 文件", this);
        extractZipBtn = new QPushButton("解压 ZIP 文件", this);
        listContentsBtn = new QPushButton("查看 ZIP 内容", this);
        clearLogBtn = new QPushButton("清空日志", this);

        // 日志输出
        logOutput = new QTextEdit(this);
        logOutput->setReadOnly(true);
        logOutput->setFont(QFont("Consolas", 9));

        // 布局
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(createZipBtn);
        buttonLayout->addWidget(extractZipBtn);
        buttonLayout->addWidget(listContentsBtn);
        buttonLayout->addWidget(clearLogBtn);

        layout->addLayout(buttonLayout);
        layout->addWidget(logOutput);

        // 初始化日志
        clearLog();
    }

    void connectSignals()
    {
        connect(createZipBtn, &QPushButton::clicked, this, &QuaZipDemo::createZipFile);
        connect(extractZipBtn, &QPushButton::clicked, this, &QuaZipDemo::extractZipFile);
        connect(listContentsBtn, &QPushButton::clicked, this, &QuaZipDemo::listZipContents);
        connect(clearLogBtn, &QPushButton::clicked, this, &QuaZipDemo::clearLog);
    }

    void addFileToZip(QuaZip& zip, const QString& fileName, const QString& content)
    {
        QuaZipFile file(&zip);
        if (!file.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName))) {
            logOutput->append(QString("❌ 无法在ZIP中创建文件: %1").arg(fileName));
            return;
        }

        file.write(content.toUtf8());
        file.close();

        logOutput->append(QString("✅ 添加文件: %1 (%2 字节)").arg(fileName).arg(content.toUtf8().size()));
    }

private:
    QPushButton *createZipBtn;
    QPushButton *extractZipBtn;
    QPushButton *listContentsBtn;
    QPushButton *clearLogBtn;
    QTextEdit *logOutput;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 测试 QuaZip 是否可用
    qDebug() << "QuaZip 测试程序启动";

    QuaZipDemo demo;
    demo.show();

    return app.exec();
}

#include "main.moc"
