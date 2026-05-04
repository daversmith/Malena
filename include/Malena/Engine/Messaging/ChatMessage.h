//
// Created by Dave Smith on 4/30/26.
//

#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H
#include <string>
#include <chrono>
#include <ctime>
namespace ml
{
    struct ChatMessage {
        std::string sender;
        std::string text;
        std::string timestamp;
        bool        isMine = false;
        static std::string isoToHM(const std::string& iso = "")
        {
            if (iso.empty())
            {
                std::time_t now = std::time(nullptr);
                std::tm* lt = std::localtime(&now);
                char buf[6];
                std::strftime(buf, sizeof(buf), "%H:%M", lt);
                return buf;
            }
            const auto t = iso.find('T');
            if (t == std::string::npos || t + 6 > iso.size()) return iso;
            return iso.substr(t + 1, 5);
        }
    };
}


#endif //CHATMESSAGE_H
