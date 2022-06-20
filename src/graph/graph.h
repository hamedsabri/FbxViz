// Copyright (C) 2022 Hamed Sabri
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#pragma once

#include "node.h"
#include "edge.h"

#include <vector>
#include <string_view>

namespace fbxViz {

class DGraph final
{
public:
    using Nodes = std::vector<Node>;
    using Edges = std::vector<Edge>;

    DGraph() = default;
    DGraph(std::string_view name);
    virtual ~DGraph() = default;

    // delete copy and move
    DGraph(const DGraph& other) = delete;
    DGraph& operator=(const DGraph& other) = delete;
    DGraph(DGraph&& other) = delete;
    DGraph& operator=(DGraph&& other) = delete;

    void addNode(const Node& node);
    const Nodes& getNodes() const;

    void addEdge(const Edge& edge);
    const Edges& getEdges() const;

    void dumpDag(std::string_view fileName);
    void dumpAnimStack(std::string_view fileName);

    std::string_view graphName;

private:
    Nodes m_nodes;
    Edges m_edges;
};

std::ostream& operator<<(std::ostream& out, const Node& node);
std::ostream& operator<<(std::ostream& out, const Edge& edge);

} // namespace fbxViz
