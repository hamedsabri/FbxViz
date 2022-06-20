#include "node.h"

#include <algorithm>

namespace fbxViz {

Node::Node(std::string_view nodeName)
    : m_name( nodeName )
{
    std::replace(m_name.begin(), m_name.end(), ' ', '_');
}

void Node::setType(std::string_view typeName)
{
    m_type = typeName;
}

const std::string& Node::getType() const
{
    return m_type;
}

void Node::setName(std::string_view nodeName)
{
    m_name = nodeName;
    std::replace(m_name.begin(), m_name.end(), ' ', '_');
}

const std::string& Node::getName() const
{
    return m_name;
}

void Node::setDataInfo(std::string_view info)
{
    m_dataInfo = info;
}

const std::string& Node::getDataInfo() const
{
    return m_dataInfo;
}

} // namespace fbxViz
