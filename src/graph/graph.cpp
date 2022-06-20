#include "graph.h"

#include <fmt/core.h>

#include <fstream>

namespace fbxViz {

DGraph::DGraph(std::string_view name)
    : graphName(name)
{
}

void DGraph::dumpDag(std::string_view fileName) 
{
    std::ofstream out(fileName.data());

    out << "digraph " << graphName << " {\n\n";
    
    out << "# Nodes" << "\n"; 
    for (const auto& node : m_nodes) {

        std::string_view shape = "ellipse";

        // special case ( root )
        if (node.getType().empty()){
            shape = "box";   
        } else if(node.getType() == "mesh") {
            shape = "diamond";   
        }

        out << node.getName() << " [" << '\n'
            << "  label = " << "<" << '\n'
            << "  <table border='0' cellborder='0' cellspacing='1'>" << '\n'
            << "  <tr><td align=\"center\"><b>" << node.getName() << "</b></td></tr>" << '\n'
            << "  <tr><td align=\"center\">" << "(" << node.getType() << ")"<< "</td></tr>" << '\n'
            << "  </table>> " << '\n'
            << "  shape = " << shape << '\n'
            << "  fillcolor = \"#40e0d0\"" << '\n'
            << "  style=\"filled\"" << '\n'
            << "]\n\n";
    }

    out << "# Edges" << "\n";
    for (const Edge& edge : m_edges) {
        out << edge << ";\n";
    }

    out << "\n}\n";

    fmt::print("Generated graph to {} successfully!!!\n", fileName);
}

void DGraph::dumpAnimStack(std::string_view fileName)
{
    std::ofstream out(fileName.data());

    out << "digraph " << graphName << " {\n\n";
    
    out << "# Nodes" << "\n"; 
    for (const auto& node : m_nodes) {

        std::string_view shape = "box";

        out << node.getName() << " [" << '\n'
            << "  label = " << "<" << '\n'
            << "  <table border='0' cellborder='0' cellspacing='1'>" << '\n'
            << "  <tr><td align=\"center\"><b>" << node.getName() << "</b></td></tr>" << '\n'
            << "  <tr><td align=\"center\">" << "(" << node.getType() << ")"<< "</td></tr>" << '\n'
            << node.getDataInfo()
            << "  </table>> " << '\n'
            << "  shape = " << shape << '\n'
            << "  fillcolor = \"#40e0d0\"" << '\n'
            << "  style=\"filled\"" << '\n'
            << "]\n\n";
    }

    out << "# Edges" << "\n";
    for (const Edge& edge : m_edges) {
        out << edge << ";\n";
    }

    out << "\n}\n";

    fmt::print("Generated graph to {} successfully!!!\n", fileName);
}

void DGraph::addNode(const Node& node)
{
    m_nodes.emplace_back(node);
}

const DGraph::Nodes& DGraph::getNodes() const
{
    return m_nodes;
}

void DGraph::addEdge(const Edge& edge)
{
    m_edges.emplace_back(edge);
}

const DGraph::Edges& DGraph::getEdges() const
{
    return m_edges;
}

std::ostream& operator<<(std::ostream& out, const Node& node)
{
    out << node.getName();
    return out;
}

std::ostream& operator<<(std::ostream& out, const Edge& edge) 
{
    out << edge.src() << " -> " << edge.dest();
    return out;
}

} // namespace fbxViz
