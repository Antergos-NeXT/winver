#include "mainwindow.h"

#include <KLocalizedString>
#include <KAboutData>

#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QProcess>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

QString MainWindow::getOsReleaseValue(const QString &key) const
{
    QFile file(QStringLiteral("/etc/os-release"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QTextStream in(&file);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.startsWith(key + QLatin1Char('='))) {
            QString val = line.mid(key.length() + 1);
            if (val.startsWith(QLatin1Char('"')))
                val = val.mid(1, val.length() - 2);
            return val;
        }
    }
    return {};
}

QString MainWindow::getKernelVersion() const
{
    QFile f(QStringLiteral("/proc/sys/kernel/version"));
    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString::fromUtf8(f.readAll()).trimmed();

    QFile f2(QStringLiteral("/proc/sys/kernel/osrelease"));
    if (f2.open(QIODevice::ReadOnly | QIODevice::Text))
        return QString::fromUtf8(f2.readAll()).trimmed();

    return {};
}

QString MainWindow::getCpuInfo() const
{
    QFile f(QStringLiteral("/proc/cpuinfo"));
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QString model;
    int count = 0;
    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.startsWith(QStringLiteral("model name"))) {
            if (model.isEmpty())
                model = line.section(QLatin1Char(':'), 1).trimmed();
            ++count;
        }
    }
    if (model.isEmpty())
        return {};
    return i18n("%1 (%2 %3)", model, count, count == 1 ? i18n("core") : i18n("cores"));
}

QString MainWindow::getMemoryInfo() const
{
    QFile f(QStringLiteral("/proc/meminfo"));
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.startsWith(QStringLiteral("MemTotal"))) {
            const QStringList parts = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                bool ok = false;
                const double kb = parts.at(1).toDouble(&ok);
                if (ok) {
                    if (kb > 1024.0 * 1024.0)
                        return i18n("%1 GB").arg(kb / (1024.0 * 1024.0), 0, 'f', 2);
                    else
                        return i18n("%1 MB").arg(kb / 1024.0, 0, 'f', 0);
                }
            }
        }
    }
    return {};
}

void MainWindow::setupUi()
{
    setWindowTitle(i18nc("@title:window", "About Antergos NeXT"));
    setFixedSize(520, 400);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 16);
    mainLayout->setSpacing(12);

    // Logo
    m_logo = new QLabel(this);
    const QString logoPath = QStringLiteral(DATA_DIR "/antergos-logo.png");
    QPixmap logo(logoPath);
    if (logo.isNull()) {
        QPixmap fallback(QStringLiteral(":/antergos-logo.png"));
        if (!fallback.isNull())
            logo = fallback;
    }
    if (!logo.isNull()) {
        m_logo->setPixmap(logo.scaledToWidth(300, Qt::SmoothTransformation));
    } else {
        m_logo->setText(QStringLiteral("Antergos NeXT"));
        QFont f = m_logo->font();
        f.setPointSize(20);
        f.setBold(true);
        m_logo->setFont(f);
    }
    m_logo->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_logo);

    // OS name
    QString osName = getOsReleaseValue(QStringLiteral("NAME"));
    if (osName.isEmpty())
        osName = QStringLiteral("Antergos NeXT");
    m_osName = new QLabel(this);
    m_osName->setText(osName);
    QFont of = m_osName->font();
    of.setPointSize(14);
    of.setBold(true);
    m_osName->setFont(of);
    m_osName->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_osName);

    // Separator
    auto *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(sep);

    // Info grid
    auto *infoLayout = new QVBoxLayout;
    infoLayout->setSpacing(6);

    const QString version = getOsReleaseValue(QStringLiteral("VERSION_ID"));
    const QString pretty = getOsReleaseValue(QStringLiteral("PRETTY_NAME"));

    m_versionLabel = new QLabel(this);
    m_versionLabel->setText(i18n("Version: %1", pretty.isEmpty() ? version : pretty));
    infoLayout->addWidget(m_versionLabel);

    m_kernelLabel = new QLabel(this);
    m_kernelLabel->setText(i18n("Kernel: %1", getKernelVersion()));
    infoLayout->addWidget(m_kernelLabel);

    QString arch = getOsReleaseValue(QStringLiteral("COREOS_ARCH"));
    if (arch.isEmpty()) {
#ifdef Q_PROCESSOR_X86_64
        arch = QStringLiteral("x86_64");
#elif defined(Q_PROCESSOR_AARCH64)
        arch = QStringLiteral("aarch64");
#else
        arch = QStringLiteral("unknown");
#endif
    }
    m_archLabel = new QLabel(this);
    m_archLabel->setText(i18n("Architecture: %1", arch));
    infoLayout->addWidget(m_archLabel);

    m_cpuLabel = new QLabel(this);
    m_cpuLabel->setText(i18n("Processor: %1", getCpuInfo()));
    infoLayout->addWidget(m_cpuLabel);

    m_memoryLabel = new QLabel(this);
    m_memoryLabel->setText(i18n("Memory: %1", getMemoryInfo()));
    infoLayout->addWidget(m_memoryLabel);

    mainLayout->addLayout(infoLayout);
    mainLayout->addStretch();

    // OK button
    auto *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    m_okButton = new QPushButton(i18nc("@action:button", "&OK"), this);
    m_okButton->setFixedWidth(100);
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(m_okButton);
    mainLayout->addLayout(buttonLayout);

    KAboutData aboutData(
        QStringLiteral("winver"),
        i18n("About Antergos NeXT"),
        QStringLiteral(WINVER_VERSION),
        i18n("Antergos NeXT system information dialog"),
        KAboutLicense::GPL_V3,
        i18n("© 2026 Antergos NeXT")
    );
    aboutData.addAuthor(i18n("Antergos NeXT Team"));
    KAboutData::setApplicationData(aboutData);
}
