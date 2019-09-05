//
// Created by Reza Alizadeh Majd on 2019-02-05.
//

#include "Logger.h"
#include <syslog.h>


//Logger gLogger;

LogEngine::LogEngine(const string &title, LogLevel lvl) :
        m_logLevel(lvl),
        m_logTitle(title) {}

void LogEngine::init(const string &title, LogLevel lvl) {
    m_logLevel = lvl;
    m_logTitle = title;
    this->init();
}

void LogEngine::setLevel(LogLevel lvl) {
    m_logLevel = lvl;
    this->init();
}

// =======================================================================

SysLogEngine::SysLogEngine(const string &title, LogLevel lvl) :
        m_syslogfacility(LOG_USER),
        LogEngine(title, lvl) {
    LogEngine::init(title, lvl);
}

SysLogEngine::~SysLogEngine() {
    if (m_inited) {
        closelog();
    }
}

int SysLogEngine::TranslateLog(LogLevel lvl) {
    static map<LogLevel, int> lvlDict = {
            std::make_pair<LogLevel, int>(LogLevel::ERR, LOG_ERR),
            std::make_pair<LogLevel, int>(LogLevel::WRN, LOG_WARNING),
            std::make_pair<LogLevel, int>(LogLevel::INF, LOG_INFO)
    };
    return lvlDict[lvl];
}

void SysLogEngine::init() {

    if (m_inited) {
        closelog();
    }
    m_syslogLevel = SysLogEngine::TranslateLog(m_logLevel);
    setlogmask(LOG_UPTO(m_syslogLevel));
    openlog(m_logTitle.c_str(), LOG_CONS | LOG_PID | LOG_NDELAY, m_syslogfacility);
}

void SysLogEngine::writeLog(LogLevel lvl, const string &message) {
    int syslogLvl = SysLogEngine::TranslateLog(lvl);
    syslog(LOG_MAKEPRI(m_syslogfacility, syslogLvl), "%s", message.c_str());
}

// =======================================================================

ConsoleLogEngine::ConsoleLogEngine(const string &title, LogLevel lvl) :
        LogEngine(title, lvl) {
    LogEngine::init(title, lvl);
}

void ConsoleLogEngine::init() {}

void ConsoleLogEngine::writeLog(LogLevel lvl, const string &message) {
    std::cout << message;
}

// =======================================================================

Logger::Logger(const string &title, LogTarget target, LogLevel lvl) :
        m_logTarget(target),
        m_logLevel(lvl),
        m_logTitle(title) {
    m_plogEngine = Logger::MakeLogEngine(m_logTarget, m_logTitle, m_logLevel);
}

Logger::~Logger() {
    if (m_plogEngine != nullptr) {
        delete m_plogEngine;
    }
}

LogEngine *Logger::MakeLogEngine(LogTarget target, const string &title, LogLevel lvl) {
    switch (target) {
        case LogTarget::SYSLOG:
            return new SysLogEngine(title, lvl);
        case LogTarget::CONSOLE:
            return new ConsoleLogEngine(title, lvl);
    }
    return nullptr;
}

void Logger::setLevel(LogLevel lvl) {
    m_logLevel = lvl;
    this->init(m_logTarget, m_logLevel);
}

void Logger::setTarget(LogTarget target) {
    m_logTarget = target;
    this->init(m_logTarget, m_logLevel);
}

void Logger::init(LogTarget target, LogLevel level) {
    m_logTarget = target;
    m_logLevel = level;

    if (m_plogEngine == nullptr || m_plogEngine->target() != target) {
        // Target Changed:
        if (m_plogEngine != nullptr) {
            delete m_plogEngine;
        }
        m_plogEngine = Logger::MakeLogEngine(m_logTarget, m_logTitle, m_logLevel);
    } else {
        // Just Set Log Level:
        m_plogEngine->setLevel(m_logLevel);
    }
}

string Logger::ExtractFileName(const string &path) {
    string fname;
    size_t lastpos = path.rfind('/');
    if (lastpos != std::string::npos) {
        fname = path.substr(lastpos + 1);
    }
    return fname;

}
