#include "core_filemanager.h"

#include <QFile>
#include <QDir>
#include <QUuid>
#include <QTextStream>

#include <project/core_project.h>

mbCoreFileManager::Strings::Strings() :
    dirname_mbtools  (QStringLiteral(".mbtools")),
    dirname_project  (QStringLiteral("project" )),
    dirname_temporary(QStringLiteral("tmp")),
    filename_projects(QStringLiteral("projects.conf")),
    filename_files   (QStringLiteral("files.xml")),
    id_path          (QStringLiteral("path"))
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
    m_project = nullptr;
    m_settings = nullptr;

    checkAndCreateHiddenFolder();
    connect(m_core, &mbCore::projectChanged, this, &mbCoreFileManager::setProject);
    setProject(m_core->projectCore());
}

mbCoreFileManager::~mbCoreFileManager()
{
    saveProjects(m_projects);
}

bool mbCoreFileManager::getFile(const QString &fileName, QFile &file, QIODevice::OpenMode modeIfExists)
{
    if (m_project && m_project->absoluteFilePath().count())
    {
        if (getOrCreateHiddenFile(fileName, file, modeIfExists))
            return true;
    }
    return createTemporaryFile(fileName, file);
}

bool mbCoreFileManager::getOrCreateHiddenFile(const QString &fileName, QFile &file, QIODevice::OpenMode mode)
{
    QString sFileName = m_currentProjectDir.filePath(fileName);
    file.setFileName(sFileName);
    if (m_project->isModified())
        return false;
    QFileInfo info(file);
    if (!file.exists() || (info.lastModified() < m_project->fileModified()))
        return file.open(QIODevice::WriteOnly);
    return file.open(mode);
}

bool mbCoreFileManager::createTemporaryFile(const QString &fileName, QFile &file, QIODevice::OpenMode mode)
{
    QString sFileName = m_hiddenTemporaryDir.filePath(fileName);
    file.setFileName(sFileName);
    return file.open(mode);
}

void mbCoreFileManager::setProject(mbCoreProject *project)
{
    if (m_project)
    {
        m_currentProjectDir.setPath(QString());
    }
    m_project = project;
    if (m_project)
    {
        QString hiddenDirPath;
        QString filePath = m_project->absoluteFilePath();
        ProjectInfo *info = projectInfo(filePath);
        if (!info)
        {
            info = new ProjectInfo;
            info->id = QUuid::createUuidV5(mbCore::globalCore()->applicationName(), filePath).toString(QUuid::WithoutBraces);
            info->absPath = filePath;
            addProjectInfo(info);
        }
        hiddenDirPath = info->id;
        m_currentProjectDir = m_hiddenProjectsDir;
        cdOrCreate(hiddenDirPath, m_currentProjectDir);
    }
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
    m_hiddenProjectsDir = workDir;
    m_hiddenTemporaryDir = workDir;
    if (!cdOrCreate(s.dirname_project, m_hiddenProjectsDir))
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
    QList<ProjectInfo*> projects = parseProjects();
    cleanupProjects(projects);
    m_projects.swap(projects);
    Q_FOREACH(ProjectInfo *info, m_projects)
        m_hashProjects.insert(info->absPath, info);
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

QList<mbCoreFileManager::ProjectInfo*> mbCoreFileManager::parseProjects()
{
    const Strings &s = Strings::instance();
    QList<ProjectInfo*> projects;
    QSettings *conf = getQSettings();
    Q_FOREACH (const QString &id, conf->childGroups())
    {
        conf->beginGroup(id);
        QString absPath = conf->value(s.id_path).toString();
        conf->endGroup();
        if (!id.isEmpty() && !absPath.isEmpty())
        {
            ProjectInfo *info = new ProjectInfo;
            info->id = id;
            info->absPath = absPath;
            projects.append(info);
        }
    }
    return projects;
}

bool mbCoreFileManager::saveProjects(const QList<ProjectInfo*> &projects)
{
    const Strings &s = Strings::instance();
    QSettings *conf = getQSettings();
    conf->clear();
    Q_FOREACH (const ProjectInfo *info, projects)
    {
        conf->beginGroup(info->id);
        conf->setValue(s.id_path, info->absPath);
        conf->endGroup();
    }

    return true;
}

void mbCoreFileManager::cleanupProjects(QList<ProjectInfo*> &projects)
{
    for (auto it = projects.begin(); it != projects.end();)
    {
        ProjectInfo *info = *it;
        if (QFile::exists(info->absPath))
            it++;
        else
        {
            // Path does not exist, delete the subfolder
            QDir projectDir(m_hiddenProjectsDir.filePath(info->id));
            if (projectDir.exists())
            {
                //qDebug() << "Deleting directory:" << projectDir.absolutePath();
                projectDir.removeRecursively();
            }
            it = projects.erase(it);
            delete info;
        }
    }
}

void mbCoreFileManager::addProjectInfo(ProjectInfo *info)
{
    m_projects.append(info);
    m_hashProjects.insert(info->absPath, info);
}

QSettings *mbCoreFileManager::getQSettings()
{
    if (!m_settings)
    {
        const Strings &s = Strings::instance();
        QString filePath = m_hiddenProjectsDir.absoluteFilePath(s.filename_projects);
        m_settings = new QSettings(filePath, QSettings::IniFormat, this);
        m_settings->setIniCodec("UTF-8");
    }
    return m_settings;
}
