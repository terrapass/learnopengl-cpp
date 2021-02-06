#include "boost_utils.h"

#include <boost/version.hpp>

#include "logging.h"

void LogBoostVersion()
{
    const int major = BOOST_VERSION / 100000;
    const int minor = BOOST_VERSION / 100 % 1000;
    const int patch = BOOST_VERSION % 100;

    BOOST_LOG_TRIVIAL(info)<< "Using Boost version " << major << '.' << minor << '.' << patch;
}
