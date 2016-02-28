
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;

    QCommandLineOption outputOption(QStringList() << "o" << "output",
                                    QCoreApplication::translate("main", "Output file name"),
                                    "output file"
                                   );

    QCommandLineOption entryOption(QStringList() << "e" << "entry",
                                   QCoreApplication::translate("main", "Entry to be added to config file in format NAME=VALUE"),
                                   "config entry"
                                  );

    parser.addOption(outputOption);
    parser.addOption(entryOption);
    parser.addHelpOption();

    parser.process(app);

    const QString outputFile = parser.value(outputOption);
    const QStringList entries = parser.values(entryOption);

    if (outputFile.isEmpty() == false)
    {
        QFile output(outputFile);
        output.open(QIODevice::WriteOnly);

        const QFileInfo fileInfo(outputFile);
        const QString guard = fileInfo.fileName()
                                      .toUpper()
                                      .replace(".", "_");

        QTextStream outputStream(&output);

        outputStream << '\n';
        outputStream << "#ifndef " << guard << '\n';
        outputStream << "#define " << guard << '\n';
        outputStream << '\n';

        for(const QString& entry: entries)
        {
            const QStringList entryData = entry.split('=');
            const int entryDataSize = entryData.size();

            if (entryDataSize > 0)
                outputStream << "#define " << entryData[0] << " ";

            if (entryDataSize > 1)
                outputStream << entryData[1] << '\n';
        }

        outputStream << '\n';
        outputStream << "#endif\n";
    }
}
