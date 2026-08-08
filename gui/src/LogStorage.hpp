//
// Created by adarw on 8/8/26.
//

#ifndef SYSID_LOGSTORAGE_HPP
#define SYSID_LOGSTORAGE_HPP
#include "core/sample/logged_sample.hpp"

namespace sysid::gui {
    class LogStorage {
    private:
        inline static std::optional<LogFile> log;
    public:

        static void setLog(const LogFile& newLog) {
            log = newLog;
        }

        static void clearLog() {
            log = std::nullopt;
        }

        static bool hasLog() {
            return log.has_value();
        }

        static LogFile& getLog() {
            if (!log) {
                throw std::runtime_error("LogStorage doesn't have an imported log!");
            }
            return *log;
        }
    };
}

#endif //SYSID_LOGSTORAGE_HPP
