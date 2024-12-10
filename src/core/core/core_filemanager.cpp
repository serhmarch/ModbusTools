#include "core_filemanager.h"

mbCoreFileManager::Strings::Strings() :
    dirname_mbtools  (QStringLiteral(".mbtools")),
    dirname_project  (QStringLiteral("project" )),
    dirname_temporary(QStringLiteral("tmp"))
{
}

const mbCoreFileManager::Strings &mbCoreFileManager::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreFileManager::mbCoreFileManager(mbCore *core, QObject *parent) : QObject{parent},
    m_core (core)
{
    checkAndCreateHiddenFolder();
}

bool mbCoreFileManager::getOrCreateHiddenFile(const QString &fileName, QFile &file, QIODevice::OpenMode mode)
{
    return false;
}

bool mbCoreFileManager::createTemporaryFile(const QString &fileName, QFile &file, QIODevice::OpenMode mode)
{
    QString sFileName = m_hiddenTemporaryDir.filePath(fileName);
    file.setFileName(sFileName);
    return file.open(mode);
}

bool mbCoreFileManager::checkAndCreateHiddenFolder()
{
    const Strings &s = Strings::instance();
    QDir workDir = QDir::home();
    if (!workDir.exists())
        return false;
    if (!cdOrCreate(s.dirname_mbtools, workDir))
        return false;
    if (!cdOrCreate(mbCore::globalCore()->applicationName(), workDir))
        return false;
    m_hiddenProjectDir = workDir;
    m_hiddenTemporaryDir = workDir;
    if (!cdOrCreate(s.dirname_project, m_hiddenProjectDir))
        return false;
    if (!cdOrCreate(s.dirname_temporary, m_hiddenTemporaryDir))
        return false;
    processHiddenProjectFolder();
    processHiddenTemporaryFolder();
    return true;
}

bool mbCoreFileManager::cdOrCreate(const QString &dirName, QDir &dir)
{
    if (dir.cd(dirName))
        return true;
    if (dir.mkdir(dirName))
    {
        if (dir.cd(dirName))
            return true;
    }
    return false;
}

bool mbCoreFileManager::processHiddenProjectFolder()
{
    return true;
}

bool mbCoreFileManager::processHiddenTemporaryFolder()
{
    bool success = true;
    QFileInfoList entries = m_hiddenTemporaryDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    Q_FOREACH (const QFileInfo &entry, entries)
    {
        if (entry.isDir())
        {
            // Recursively clear and remove subdirectories
            QDir subDir(entry.absoluteFilePath());
            if (!subDir.removeRecursively())
                success = false;
        }
        else
        {
            // Remove files
            if (!QFile::remove(entry.absoluteFilePath()))
                success = false;
        }
    }
    return success;
}
