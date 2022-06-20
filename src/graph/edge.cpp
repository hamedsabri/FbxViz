#include "edge.h"
#include "node.h"

#include <algorithm>

namespace fbxViz {

Edge::Edge(const Node& src, const Node& dest)
    : m_src(src)
    , m_dest(dest)
{
    ++m_edgeId;
}

const Node& Edge::src() const
{
    return m_src;
}

const Node& Edge::dest() const
{
    return m_dest;
}

const int Edge::edgeId() const
{
    return m_edgeId;
}

void Edge::setName(std::string_view nodeName)
{
    m_name = nodeName;
}

const std::string& Edge::getName() const
{
    return m_name;
}


} // namespace fbxViz