#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    void setupUi();
    QString getOsReleaseValue(const QString &key) const;
    QString getKernelVersion() const;
    QString getCpuInfo() const;
    QString getMemoryInfo() const;

    QLabel *m_logo;
    QLabel *m_osName;
    QLabel *m_versionLabel;
    QLabel *m_kernelLabel;
    QLabel *m_archLabel;
    QLabel *m_cpuLabel;
    QLabel *m_memoryLabel;
    QPushButton *m_okButton;
};
