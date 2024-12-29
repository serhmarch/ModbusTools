#include "core_filemanager.h"

#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QUuid>
#include <QTextStream>

mbCoreFileManager::Strings::Strings() :
    dirname_mbtools  (QStringLiteral(".mbtools")),
    dirname_project  (QStringLiteral("project" )),
    dirname_temporary(QStringLiteral("tmp")),
    filename_projects(QStringLiteral("projects.xml")),
    filename_files   (QStringLiteral("files.xml")),
    dom_projects     (QStringLiteral("projects")),
    dom_project      (QStringLiteral("project"))
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
    connect(m_core, &mbCore::projectChanged, this, &mbCoreFileManager::setProject);
    setProject(m_core->projectCore());
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

void mbCoreFileManager::setProject(mbCoreProject *project)
{

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
    QList<ProjectInfo> projects = parseProjects();
    cleanupProjects(projects);
    m_projects.swap(projects);
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

QList<mbCoreFileManager::ProjectInfo> mbCoreFileManager::parseProjects()
{
    const Strings &s = Strings::instance();
    QList<ProjectInfo> projects;

    QString filePath = m_hiddenProjectDir.absoluteFilePath(s.filename_projects);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //qWarning() << "Failed to open file:" << filePath;
        return projects;
    }

    QDomDocument doc;
    if (!doc.setContent(&file))
    {
        //qWarning() << "Failed to parse XML file.";
        file.close();
        return projects;
    }
    file.close();

    QDomElement root = doc.documentElement();
    if (root.tagName() != s.dom_projects)
    {
        //qWarning() << "Invalid root element in XML.";
        return projects;
    }

    QDomNodeList projectNodes = root.elementsByTagName(s.dom_project);
    for (int i = 0; i < projectNodes.size(); ++i)
    {
        QDomElement projectElement = projectNodes.at(i).toElement();
        if (!projectElement.isNull())
        {
            QString id = projectElement.attribute("dir");
            QString absPath = projectElement.text();
            if (!id.isEmpty() && !absPath.isEmpty())
                projects.append({id, absPath});
        }
    }

    return projects;
}

bool mbCoreFileManager::saveProjects(const QList<ProjectInfo> &projects)
{
    const Strings &s = Strings::instance();
    QString filePath = m_hiddenProjectDir.absoluteFilePath(s.filename_projects);
    QDomDocument doc;

    QDomElement root = doc.createElement(s.dom_projects);
    doc.appendChild(root);

    Q_FOREACH (const ProjectInfo &project, projects)
    {
        QDomElement projectElement = doc.createElement(s.dom_project);
        projectElement.setAttribute("dir", project.id);
        QDomText pathText = doc.createTextNode(project.absPath);
        projectElement.appendChild(pathText);
        root.appendChild(projectElement);
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        //qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    doc.save(stream, 4);
    file.close();

    return true;}

void mbCoreFileManager::cleanupProjects(QList<ProjectInfo> &projects)
{
    for (auto it = projects.begin(); it != projects.end();)
    {
        const ProjectInfo &project = *it;
        if (QFile::exists(project.absPath))
            it++;
        else
        {
            // Path does not exist, delete the subfolder
            QDir projectDir(m_hiddenProjectDir.filePath(project.id));
            if (projectDir.exists())
            {
                //qDebug() << "Deleting directory:" << projectDir.absolutePath();
                projectDir.removeRecursively();
            }
            it = projects.erase(it);
        }
    }
}
