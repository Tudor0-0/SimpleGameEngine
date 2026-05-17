#pragma once
#include <exception>
#include <string>
#include <sstream>
template <typename Category>
class StructuredException : public std::exception {
private:
    std::string m_message;
public:
    StructuredException(const std::string& p_message, const char* p_file, int p_line){
        std::ostringstream oss;
        oss << "[" << Category::name << " Error " << "] "
            << p_message << "\nLocation: " << p_file  << ":" << p_line;
        m_message = oss.str();
    }

    [[nodiscard]]const char* what() const noexcept override { return m_message.c_str(); }
};

struct SdlTag{ static inline const char* name = "SDL"; };


#define THROW_SDL_INIT_ERROR(msg) \
throw StructuredException<SdlTag>(msg, __FILE__, __LINE__)
#define THROW_SDL_RENDER_ERROR(msg) \
throw StructuredException<SdlTag>(msg, __FILE__, __LINE__)
