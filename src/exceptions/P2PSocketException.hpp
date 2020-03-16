#pragma once

#include <string>

class P2PSocketException
{
private:
    std::string m_description;

public:
    P2PSocketException(std::string &&desc) : m_description{std::move(desc)} {}
    std::string what() const {return m_description;}
};
