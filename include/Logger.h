#ifndef GENERATIONS_LOGGER_H
#define GENERATIONS_LOGGER_H

#include <memory>
#include <fstream>
#include <mutex>
#include "Includes.h"

namespace SGE {

    class Logger {
    public:
        static Logger *getInstance() {
            if (!m_logger) {
                m_logger = std::make_unique<Logger>();
            }

            return m_logger.get();
        }

        Logger() {
            m_logFile.open("log.txt", std::fstream::out | std::fstream::trunc);
        }

        ~Logger() {
            m_logFile.close();
        }

        void writeToLog(const std::string &write) {
            //locking here to prevent race conditions when writing to file
            std::lock_guard<std::mutex> m_lock(m_mutex);
            if (m_logFile.is_open()) {
                m_logFile << write << "\n";
            } else {
                boxer::show("Error: Log file is not open", "Log file error.");
            }
        }

    private:
        std::mutex m_mutex;

        std::fstream m_logFile;

        static std::unique_ptr<Logger> m_logger;
    };


}


#endif //GENERATIONS_LOGGER_H
