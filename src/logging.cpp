#include "logging.h"

#include "config.h"

void InitLogger()
{
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= MIN_LOG_LEVEL
    );
}
