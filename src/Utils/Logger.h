//
// Created by Reza Alizadeh Majd on 2019-02-05.
//

#ifndef PX_EVENTS_SERVICE_LOGGER_H
#define PX_EVENTS_SERVICE_LOGGER_H

#include <iostream>
#include <sstream>
#include <map>

using namespace std;

/**
 * Enumeration that provides different log levels
 */
enum class LogLevel {
    ERR = 0,    ///< Error log messages
    WRN = 1,    ///< Warning log messages
    INF = 2     ///< Information log messages
};

/**
 * Enumeration that provides different log targets
 */
enum class LogTarget : int {
    CONSOLE = 0,        ///< Log to console
    SYSLOG = 1          ///< send logs to syslog
};

/// @brief Base class for different log engines
class LogEngine {
public:
    /// @brief initiate engine with e specific title and LogLevel
    explicit LogEngine(const string &title, LogLevel lvl);

    /// @brief default destructor for log engine
    virtual ~LogEngine() = default;

protected:
    /// @brief initiate log engine with a specific title and LogLevel
    void init(const string &title, LogLevel lvl);

    /// @brief abstract method for log engine that calls internally to perform engine-specific tasks
    virtual void init() = 0;

public:
    /// @brief set default log level for log engine
    void setLevel(LogLevel lvl);

    /// @brief abstract method to define target of specified LogEngine
    virtual LogTarget target() const = 0;

    /// @brief abstract method to send log line to specified target
    virtual void writeLog(LogLevel lvl, const string &message) = 0;

protected:
    bool m_inited = false;                  ///< @brief indicated if LogEngine is initiated
    LogLevel m_logLevel = LogLevel::WRN;    ///< @brief specified LogLevel for engine
    string m_logTitle;                      ///< @brief log title that prepends to each log line
};


/// @brief LogEngine that defined to send it's messages to syslog
class SysLogEngine : public LogEngine {
public:
    /// @brief LogEngine constructor
    SysLogEngine(const string &title, LogLevel lvl);

    /// @brief LogEngine destructor
    virtual ~SysLogEngine();

protected:
    /// @brief init LogEngine
    void init() override;

public:
    /// @brief Translate LogLevel to a matching syslog level
    static int TranslateLogLevel(LogLevel lvl);

    /// @brief return SYSLOG as LogTarget
    virtual LogTarget target() const override { return LogTarget::SYSLOG; };

    /// @brief Write Log message with a specified level
    void writeLog(LogLevel lvl, const string &message) override;

protected:
    int m_syslogLevel;      ///< @brief provided level for syslog
    int m_syslogFacility;   ///< @brief provided facility for syslog
};

/// @brief LogEngine tha defined to send it's messages to stdout
class ConsoleLogEngine : public LogEngine {
public:
    /// @brief LogEngine constructor
    ConsoleLogEngine(const string &title, LogLevel lvl);

protected:
    /// @brief init LogEngine
    void init() override;

public:
    /// returns CONSOLE as LogTarget
    virtual LogTarget target() const override { return LogTarget::CONSOLE; };

    /// @brief Write Log message with a specified level
    void writeLog(LogLevel lvl, const string &message) override;
};

/// @brief Logger class that prapare log messages and write them to target
class Logger {

public:
    /// @brief Logger class constructor
    explicit Logger(const string &title, LogTarget target = LogTarget::SYSLOG, LogLevel lvl = LogLevel::WRN);

    /// @brief default destructor for Logger
    virtual ~Logger();

    /// @brief static method that creates proper LogEngine based on specified title
    static LogEngine *MakeLogEngine(LogTarget target, const string &title, LogLevel lvl);

public:
    /// @brief set LogLevel
    void setLevel(LogLevel lvl);

    /// @brief set LogTarget
    void setTarget(LogTarget target);

    /// @brief initiate Logger
    void init(LogTarget target, LogLevel level);


    template<typename T>
    string log(T val) {
        stringstream sstream;
        sstream << val;
        return sstream.str();
    }

    template<typename HEAD, typename ... REST>
    string log(HEAD head, REST ... rest) {
        stringstream sstream;
        sstream << head << " " << log(rest ...);
        return sstream.str();
    }

    template<typename HEAD, typename ... REST>
    void log(bool force, LogLevel lvl, const char *file, const char *func, int line, HEAD head, REST ... rest) {
        static std::map<LogLevel, std::string> lvlDict;
        lvlDict[LogLevel::ERR] = "ERR";
        lvlDict[LogLevel::WRN] = "WRN";
        lvlDict[LogLevel::INF] = "INF";

        if (force || lvl <= m_logLevel) {
            stringstream sstream;
            sstream << "[" << lvlDict[lvl]
                    << "][" << Logger::ExtractFileName(file)
                    << "][" << func
                    << "](" << line << "): "
                    << log(head, rest ...)
                    << std::endl;

            if (m_pLogEngine != nullptr) {
                m_pLogEngine->writeLog(lvl, sstream.str());
            }
        }
    }

protected:
    /// @brief static method to extract file name from path
    static string ExtractFileName(const string &path);

protected:
    string m_logTitle;                          ///< @brief log title
    LogLevel m_logLevel;                        ///< @brief log level
    LogTarget m_logTarget = LogTarget::SYSLOG;  ///< @brief target that log messages should send to
    LogEngine *m_pLogEngine = nullptr;          ///< @brief pointer to provided  LogEngine
};

#define LOG_ERR_MSG(l, ...) (l.log(false, LogLevel::ERR, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define LOG_WRN_MSG(l, ...) (l.log(false, LogLevel::WRN, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define LOG_INF_MSG(l, ...) (l.log(false, LogLevel::INF, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__))

extern Logger gLogger;
#define GLOG_LEVEL(lvl) (gLogger.setLevel(lvl))
#define GLOG_INIT(target, level) (gLogger.init((target), (level)))
#define GLOG_ERR(...) (gLogger.log(false, LogLevel::ERR, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define GLOG_WRN(...) (gLogger.log(false, LogLevel::WRN, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define GLOG_INF(...) (gLogger.log(false, LogLevel::INF, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define GLOG_ERR_FORCE(...) (gLogger.log(true, LogLevel::ERR, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define GLOG_WRN_FORCE(...) (gLogger.log(true, LogLevel::WRN, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define GLOG_INF_FORCE(...) (gLogger.log(true, LogLevel::INF, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__))


#endif //PX_EVENTS_SERVICE_LOGGER_H
