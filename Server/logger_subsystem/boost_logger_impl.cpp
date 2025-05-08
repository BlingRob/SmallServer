#include "boost_logger_impl.h"

#include <utils.h>

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", int)

BoostLogger::BoostLogger(const std::string& logFileName)
              : sev_{Severities::Debug}
{
    std::string fileName;

    if(logFileName.empty())
    {
        fileName = (std::string("log_") + utils::CurrentDateTime() + ".txt");

        std::replace(fileName.begin(), fileName.end(), ':', '_');
    }
    else
    {
        fileName = logFileName;
    }

    boost::log::add_file_log(
        boost::log::keywords::file_name = fileName,
        boost::log::keywords::rotation_size = utils::mByte,
        boost::log::keywords::max_size = 20 * utils::mByte,
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        boost::log::keywords::format = "[%TimeStamp%] [%ThreadID%] [%Severity%] %Message%",
        boost::log::keywords::auto_flush = true
        );

    boost::log::add_common_attributes();
}

void BoostLogger::SetSeverities(Severities sev)
{
    boost::log::core::get()->set_filter
    (
        // here they've used a constant but you could use a global or
        // a function
        boost::log::trivial::severity >= boost::log::trivial::info
    );
}

void BoostLogger::Log(std::string_view msg, Severities sev)
{
    BOOST_LOG_TRIVIAL(trace) << msg;
    // BOOST_LOG_TRIVIAL(debug) << "This is a debug severity message";
    // BOOST_LOG_TRIVIAL(info) << "This is an informational severity message";
    // BOOST_LOG_TRIVIAL(warning) << "This is a warning severity message";
    // BOOST_LOG_TRIVIAL(error) << "This is an error severity message";
    // BOOST_LOG_TRIVIAL(fatal) << "and this is a fatal severity message";
}