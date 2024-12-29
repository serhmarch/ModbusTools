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
    // Struct to represent a project record
    struct ProjectInfo
    {
        QString id      ; // GUID/UUID (dir attribute)
        QString absPath ; // Absolute path to the project
    };

    // Struct to represent a project record
    struct FileInfo
    {
        QString id      ; // GUID/UUID (dir attribute)
        QString absPath ; // Absolute path to the project
    };

public:
    struct MB_EXPORT Strings
    {
        const QString dirname_mbtools   ;
        const QString dirname_project   ;
        const QString dirname_temporary ;
        const QString filename_projects ;
        const QString filename_files    ;
        const QString dom_projects      ;
        const QString dom_project       ;

        Strings();
        static const Strings &instance();
    };
public:
    explicit mbCoreFileManager(mbCore *core, QObject *parent = nullptr);

public:
    QFile getFile(const QString &fileName, QIODevice::OpenMode modeIfExists = QIODevice::WriteOnly);
    bool getOrCreateHiddenFile(const QString &fileName, QFile &file, QIODevice::OpenMode mode = QIODevice::WriteOnly);
    bool createTemporaryFile(const QString &fileName, QFile &file, QIODevice::OpenMode mode = QIODevice::WriteOnly);

Q_SIGNALS:

protected Q_SLOTS:
    void setProject(mbCoreProject *project);

protected:
    bool checkAndCreateHiddenFolder();
    bool cdOrCreate(const QString &dirName, QDir &dir);
    bool processHiddenProjectFolder();
    bool processHiddenTemporaryFolder();
    QList<ProjectInfo> parseProjects();
    bool saveProjects(const QList<ProjectInfo> &projects);
    void cleanupProjects(QList<ProjectInfo> &projects);

protected:
    mbCore *m_core;
    mbCoreProject *m_project;
    QDir m_hiddenProjectDir;
    QDir m_hiddenTemporaryDir;
    QList<ProjectInfo> m_projects;
};

#endif // CORE_FILEMANAGER_H
