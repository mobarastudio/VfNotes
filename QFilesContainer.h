#include <QFileSystemModel>
#include <QFile>
#include <QDir>
#include <QTextStream>
class QFilesContainer
{
    QStringList filesList;
    QDir dir;
    QFile currnetFile;
    QString lastFile, patch;
    bool isOpenDirectory;
public:
    QStringList getFilesList();

    QString openFile(QString fileName);

    void closeFile();

    bool saveCurrentFile(QString content);

    QStringList newFile(QString fileName);

    QStringList removeFile();

    QStringList renameFile(QString oldName, QString newName);

    QStringList openDirectory(QString location);

    bool checkOpenDirectory();

    bool checkOpenFile();

    QStringList closeDirectory();

    QFilesContainer();
};
