#ifndef CORE_FILEMANAGER_H
#define CORE_FILEMANAGER_H

#include <QObject>
#include <QFile>
#include <QDir>

#include "core.h"

class MB_EXPORT mbCoreFileManager : public QObject
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings
    {
        const QString dirname_mbtools  ;
        const QString dirname_project  ;
        const QString dirname_temporary;

        Strings();
        static const Strings &instance();
    };
public:
    explicit mbCoreFileManager(mbCore *core, QObject *parent = nullptr);

public:
    bool getOrCreateHiddenFile(const QString &fileName, QFile &file, QIODevice::OpenMode mode = QIODevice::WriteOnly);
    bool createTemporaryFile(const QString &fileName, QFile &file, QIODevice::OpenMode mode = QIODevice::WriteOnly);

Q_SIGNALS:

protected:
    bool checkAndCreateHiddenFolder();
    bool cdOrCreate(const QString &dirName, QDir &dir);
    bool processHiddenProjectFolder();
    bool processHiddenTemporaryFolder();

protected:
    mbCore *m_core;
    QDir m_hiddenProjectDir;
    QDir m_hiddenTemporaryDir;
};

#endif // CORE_FILEMANAGER_H
