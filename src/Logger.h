#ifndef GENERATIONS_LOGGER_H
#define GENERATIONS_LOGGER_H

#include <memory>
#include <fstream>
#include "boxer/boxer.h"

namespace SGE {

    class Logger {
    public:
        static Logger *getInstance() {
            if (!logger) {
                logger = std::make_unique<Logger>();
            }

            return logger.get();
        }

        Logger() {
            logFile.open("log.txt", std::fstream::out | std::fstream::trunc);
        }

        ~Logger() {
            logFile.close();
        }

        void writeToLog(const std::string &write) {
            if (logFile.is_open()) {
                logFile << write << "\n";
            } else {
                boxer::show("Error: Log file is not open", "Log file error.");
            }
        }

    private:

        std::fstream logFile;

        static std::unique_ptr<Logger> logger;
    };

    std::unique_ptr<Logger> Logger::logger;

}


#endif //GENERATIONS_LOGGER_H
