//
// Created by Reza Alizadeh Majd on 2019-02-05.
//

#ifndef PX_EVENTS_SERVICE_LOGGER_H
#define PX_EVENTS_SERVICE_LOGGER_H

#include <iostream>
#include <sstream>
#include <map>

using namespace std;

enum class LogLevel {
    ERR = 0,
    WRN = 1,
    INF = 2
};


enum class LogTarget : int {
    CONSOLE = 0,
    SYSLOG = 1
};


class LogEngine {
public:
    explicit LogEngine(const string &title, LogLevel lvl);
    virtual ~LogEngine() = default;

protected:
    void init(const string &title, LogLevel lvl);
    virtual void init() = 0;

public:
    void setLevel(LogLevel lvl);
    virtual LogTarget target() const = 0;
    virtual void writeLog(LogLevel lvl, const string &message) = 0;

protected:
    LogLevel m_logLevel = LogLevel::WRN;
    string m_logTitle;
};


class SysLogEngine : public LogEngine {
public:
    SysLogEngine(const string &title, LogLevel lvl);
    virtual ~SysLogEngine();

protected:
    void init() override;

public:
    static int TranslateLog(LogLevel lvl);
    virtual LogTarget target() const override { return LogTarget::SYSLOG; };
    void writeLog(LogLevel lvl, const string &message) override;

protected:
    bool m_inited = false;
    int m_syslogLevel;
    int m_syslogfacility;
};


class ConsoleLogEngine : public LogEngine {
public:
    ConsoleLogEngine(const string &title, LogLevel lvl);

protected:
    void init() override;

public:
    virtual LogTarget target() const override { return LogTarget::CONSOLE; };
    void writeLog(LogLevel lvl, const string &message) override;
};


class Logger {

public:
    explicit Logger(const string &title, LogTarget target = LogTarget::SYSLOG, LogLevel lvl = LogLevel::WRN);
    virtual ~Logger();

    static LogEngine *MakeLogEngine(LogTarget target, const string &title, LogLevel lvl);

public:
    void setLevel(LogLevel lvl);
    void setTarget(LogTarget target);
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

            if (m_plogEngine != nullptr) {
                m_plogEngine->writeLog(lvl, sstream.str());
            }
        }
    }

protected:
    static string ExtractFileName(const string &path);

protected:
    string m_logTitle;
    LogLevel m_logLevel;
    LogTarget m_logTarget = LogTarget::SYSLOG;
    LogEngine *m_plogEngine = nullptr;
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
