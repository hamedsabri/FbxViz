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
#include "common.h"

#include <fbxsdk.h>

#include <fmt/core.h>

#include <string_view>

namespace {

    void usage(std::string_view program) 
    {
        fmt::print("Usage: {} <input.fbx>\n", program);
    }

    bool hasFbxExtension(std::string_view inputFilePath)
    {
        auto len = inputFilePath.length();
        auto pos = inputFilePath.rfind('.', len);

        if (pos != std::string::npos) {
            auto ext = inputFilePath.substr(pos + 1 , len - 1);
            if (ext == "fbx" || ext == "FBX") {
                return true;
            } else {
                return false;
            }
        }

        return false;
    }
} // namespace

int main(int argc, char** argv)
{
    std::string_view program = argv[0];
    if (argv[1] == nullptr) {
        usage(program);
        fmt::print("Error: no input file provided!\n");
        return 1;
    }

    std::string_view inputFilePath = argv[1];
    if (!hasFbxExtension(inputFilePath)) {
        fmt::print("Error: no valid fbx file format provided!\n");
        return 1;
    }

    FbxManager* fbxManager {nullptr};
    FbxScene* fbxScene {nullptr};
    bool status {false};

    // initialize the FBX SDK.
    status = fbxViz::initialize(fbxManager, fbxScene);
    if (!status) {
        fmt::print("An error occurred initializing the fbx sdk...\n");
        fbxViz::destroy(fbxManager);
        return 1;
    }

    // load scene
    status = fbxViz::loadScene(fbxManager, fbxScene, inputFilePath);
    if (!status) {
        fmt::print("An error occurred while loading the scene...\n");
        fbxViz::destroy(fbxManager);
        return 1;
    }

    // dump scene DAG
    fbxViz::dumpSceneToFile(fbxScene, "dag.dot");

    // dump animation stack
    fbxViz::dumpAnimStackToFile(fbxScene, "animstack.dot");

    // destroy the FBX SDK.
    fbxViz::destroy(fbxManager);

    return 0;
}
