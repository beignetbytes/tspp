#pragma once
#ifndef time_series_errors_hpp
#define time_series_errors_hpp

#include <string>
#include <exception>
#include <functional>
#include <memory>
#include <sstream>

namespace tspp {

    #pragma region Precondition macro

    //! Base error class
    class time_series_error : public std::exception {
      public:
        time_series_error(const std::string& file,
                  long line,
                  const std::string& functionName,
                  const std::string& message = "") : message_(std::make_shared<std::string>(format(file, line, functionName, message)))
        {

        };

        ~time_series_error() throw() {}

        const char* what() const throw() { return message_->c_str(); }

      private:
        std::shared_ptr<std::string> message_;
        static std::string format(

            const std::string& file,
            long line,
            const std::string& function,
            const std::string& message) {
            std::ostringstream msg;
            if (function != "(unknown)")
                msg << function << ": ";
            msg << "\n  " << file << "(" << line << "): \n";
            msg << message;
            return msg.str();
        }
    };

/*! \def time_series_REQUIRE
\brief throw an error if the given pre-condition is not verified can also use BOOST_CURRENT_FUNCTION
 */
#define time_series_REQUIRE(condition, message)                                       \
    if (!(condition)) {                                                       \
        std::ostringstream _msg_stream;                                       \
        _msg_stream << message;                                               \
        throw tspp::time_series_error(__FILE__, __LINE__, __FUNCTION__, _msg_stream.str()); \
    } else {}

#pragma endregion

}
#endif