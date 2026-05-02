// I don't know why they used extern, but nvcodec_utils needs a globally defined
// logger so...
#include "Logger.h"
simplelogger::Logger *logger =
    simplelogger::LoggerFactory::CreateConsoleLogger();
