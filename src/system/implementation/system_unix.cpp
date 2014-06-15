
#include "../system.hpp"

#include <stdlib.h>

#include <boost/regex.hpp>

#include <QProcess>

namespace
{
    std::string run(const std::string& program)
    {
        QProcess process;

        process.closeWriteChannel();
        process.start(program.c_str(), QProcess::ReadOnly);
        process.waitForFinished(3000);

        const QString line = process.readAll();
        const std::string result = line.toStdString();

        return result;
    }
}


std::string System::getApplicationConfigDir()
{
    std::string result(getenv("HOME"));

    result += "/.config/broom";

    return result;
}


std::string System::findProgram(const std::string& name)
{
    const std::string result = run("whereis " + name);

    boost::cmatch results;
    const boost::regex regex(name + ": ([^ ]+).*");

    const bool matches = boost::regex_match(result.c_str(), results, regex);

    std::string path;

    if (matches)
        path = results[1].str();

    return path;
}


std::string System::userName()
{
    const std::string result = run("whoami");

    QString name(result.c_str());
    name = name.simplified();

    return name.toStdString();
}

