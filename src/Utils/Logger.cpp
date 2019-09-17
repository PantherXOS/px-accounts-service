//
// Created by Reza Alizadeh Majd on 2019-02-05.
//

#include "Logger.h"
#include <syslog.h>

/**
 * LogEngine Constructor that sets the LogLevel and Title
 * @param title title that prepends to each log message
 * @param lvl log level
 */
LogEngine::LogEngine(const string &title, LogLevel lvl) :
        m_logLevel(lvl),
        m_logTitle(title) {}

/**
 * init LogEngine by setting the title and level and call the
 * internal abstract init method.
 *
 * @param title log title
 * @param lvl log level
 */
void LogEngine::init(const string &title, LogLevel lvl) {
    m_logLevel = lvl;
    m_logTitle = title;
    this->init();
}

/**
 * set LogLevel and re-init the LogEngine
 * @param lvl log level
 */
void LogEngine::setLevel(LogLevel lvl) {
    m_logLevel = lvl;
    this->init();
}

// =======================================================================

/**
 * constructor for SysLogEngine sets the facility and calls the parent
 * construcotr and init methods.
 *
 * @param title log title
 * @param lvl log level
 */
SysLogEngine::SysLogEngine(const string &title, LogLevel lvl) :
        m_syslogFacility(LOG_USER),
        LogEngine(title, lvl) {
    LogEngine::init(title, lvl);
}

/**
 * checks if already inited and close syslog if so.
 */
SysLogEngine::~SysLogEngine() {
    if (m_inited) {
        closelog();
    }
}

/**
 * Translate Logger Level to syslog levels using an internal
 * mapping between LogLevel enum and syslog levels.
 *
 * @param lvl log level
 * @return syslog related integer value
 */
int SysLogEngine::TranslateLogLevel(LogLevel lvl) {
    static map<LogLevel, int> lvlDict = {
            std::make_pair<LogLevel, int>(LogLevel::ERR, LOG_ERR),
            std::make_pair<LogLevel, int>(LogLevel::WRN, LOG_WARNING),
            std::make_pair<LogLevel, int>(LogLevel::INF, LOG_INFO)
    };
    return lvlDict[lvl];
}

/**
 * checks ehether engine is already inited and close previous handle.
 * then sets the syslog mask and re/opens the syslog to send logs to.
 */
void SysLogEngine::init() {

    if (m_inited) {
        closelog();
    }
    m_syslogLevel = SysLogEngine::TranslateLogLevel(m_logLevel);
    setlogmask(LOG_UPTO(m_syslogLevel));
    openlog(m_logTitle.c_str(), LOG_CONS | LOG_PID | LOG_NDELAY, m_syslogFacility);
    this->m_inited = true;
}

/**
 * write a string message with specified facility and priority to syslog
 *
 * @param lvl log level
 * @param message log message
 */
void SysLogEngine::writeLog(LogLevel lvl, const string &message) {
    int syslogLvl = SysLogEngine::TranslateLogLevel(lvl);
    syslog(LOG_MAKEPRI(m_syslogFacility, syslogLvl), "%s", message.c_str());
}

// =======================================================================

/**
 * constructor that inits the LogEngine with specified level and title
 *
 * @param title log title
 * @param lvl log level
 */
ConsoleLogEngine::ConsoleLogEngine(const string &title, LogLevel lvl) :
        LogEngine(title, lvl) {
    LogEngine::init(title, lvl);
}

void ConsoleLogEngine::init() {
    this->m_inited = true;
}

/**
 * prints log message to stdout
 *
 * @param lvl log level
 * @param message log message
 */
void ConsoleLogEngine::writeLog(LogLevel lvl, const string &message) {
    std::cout << message;
}

// =======================================================================

/**
 * constructor that sets title target and level of Logger
 * @param title log title
 * @param target log target
 * @param lvl log level
 */
Logger::Logger(const string &title, LogTarget target, LogLevel lvl) :
        m_logTarget(target),
        m_logLevel(lvl),
        m_logTitle(title) {
    m_pLogEngine = Logger::MakeLogEngine(m_logTarget, m_logTitle, m_logLevel);
}

/**
 * delete allocated LogEngine from memory
 */
Logger::~Logger() {
    if (m_pLogEngine != nullptr) {
        delete m_pLogEngine;
    }
}

/**
 * Create initiate LogEngine based on provided details and return a pointer to it.
 * @param target log target
 * @param title log title
 * @param lvl log level
 *
 * @return pointer to created LogEngine
 */
LogEngine *Logger::MakeLogEngine(LogTarget target, const string &title, LogLevel lvl) {
    switch (target) {
        case LogTarget::SYSLOG:
            return new SysLogEngine(title, lvl);
        case LogTarget::CONSOLE:
            return new ConsoleLogEngine(title, lvl);
    }
    return nullptr;
}

/**
 * set log level and re-init Logger
 * @param lvl log level
 */
void Logger::setLevel(LogLevel lvl) {
    m_logLevel = lvl;
    this->init(m_logTarget, m_logLevel);
}

/**
 * set target and re-init Logger
 *
 * @param target log target
 */
void Logger::setTarget(LogTarget target) {
    m_logTarget = target;
    this->init(m_logTarget, m_logLevel);
}

/**
 * init Logger for specified target with provided level
 *
 * @param target log target
 * @param level log level
 */
void Logger::init(LogTarget target, LogLevel level) {
    m_logTarget = target;
    m_logLevel = level;

    if (m_pLogEngine == nullptr || m_pLogEngine->target() != target) {
        // Target Changed:
        if (m_pLogEngine != nullptr) {
            delete m_pLogEngine;
        }
        m_pLogEngine = Logger::MakeLogEngine(m_logTarget, m_logTitle, m_logLevel);
    } else {
        // Just Set Log Level:
        m_pLogEngine->setLevel(m_logLevel);
    }
}

/**
 * utility helper class that extract file name from File path
 *
 * @param path full path for a file
 * @return file name part
 */
string Logger::ExtractFileName(const string &path) {
    string fname;
    size_t lastpos = path.rfind('/');
    if (lastpos != std::string::npos) {
        fname = path.substr(lastpos + 1);
    }
    return fname;
}
