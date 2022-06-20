#include "common.h"
#include "curveInfo.h"

#include "graph/graph.h"
#include "graph/node.h"

#include <fmt/core.h>

namespace {

    fbxViz::Node createNode(fbxViz::DGraph& graph, std::string_view name, std::string_view type)
    {
        fbxViz::Node node(name);
        node.setType(type);
        graph.addNode(node);
        return node;
    }

    fbxViz::Edge createEdge(fbxViz::DGraph& graph, fbxViz::Node& nodeA, fbxViz::Node& nodeB, std::string_view name=" ")
    {
        fbxViz::Edge edge(nodeA, nodeB);
        edge.setName(name);
        graph.addEdge(edge);
        return edge;
    }

    FbxString attributeTypeName(FbxNodeAttribute::EType type) 
    {
        switch(type) {
            case FbxNodeAttribute::eUnknown: return "unidentified";
            case FbxNodeAttribute::eNull: return "null";
            case FbxNodeAttribute::eMarker: return "marker";
            case FbxNodeAttribute::eSkeleton: return "skeleton";
            case FbxNodeAttribute::eMesh: return "mesh";
            case FbxNodeAttribute::eNurbs: return "nurbs";
            case FbxNodeAttribute::ePatch: return "patch";
            case FbxNodeAttribute::eCamera: return "camera";
            case FbxNodeAttribute::eCameraStereo: return "stereo";
            case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
            case FbxNodeAttribute::eLight: return "light";
            case FbxNodeAttribute::eOpticalReference: return "optical reference";
            case FbxNodeAttribute::eOpticalMarker: return "marker";
            case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
            case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
            case FbxNodeAttribute::eBoundary: return "boundary";
            case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
            case FbxNodeAttribute::eShape: return "shape";
            case FbxNodeAttribute::eLODGroup: return "lodgroup";
            case FbxNodeAttribute::eSubDiv: return "subdiv";
            default: return "unknown";
        }
    }

    std::string getAttrName(FbxNode* pNode) 
    {
        FbxString typeName;
        for(int i = 0; i < pNode->GetNodeAttributeCount(); i++) {
            FbxNodeAttribute* pAttribute = pNode->GetNodeAttributeByIndex(i);
            typeName += attributeTypeName(pAttribute->GetAttributeType());
        }

        return std::string( typeName.Buffer() );
    }

    // recursively traverse a node
    void traverseNode(fbxViz::DGraph& dGraph, FbxNode* fbxNode)
    {
        for(auto i= 0; i < fbxNode->GetChildCount(); i++) {
           auto childNode = createNode(dGraph, fbxNode->GetName(), getAttrName(fbxNode));
           auto nextChildNode = createNode(dGraph, fbxNode->GetChild(i)->GetName(), getAttrName(fbxNode->GetChild(i)));
           createEdge(dGraph, childNode, nextChildNode);

           traverseNode( dGraph, fbxNode->GetChild(i));
        }
    }

    void curveKeyInfo(fbxViz::DGraph& dGraph, FbxNode* pNode, std::string_view propName, FbxAnimCurve* curveNode, fbxViz::Node& animNode) 
    {
       std::string nodeName(pNode->GetName());
       nodeName.append("_");
       nodeName.append(propName.data());

       auto propertyNode = createNode(dGraph, nodeName, "property");
       createEdge(dGraph, animNode, propertyNode);

       std::string animCurveNodeName(pNode->GetName());
       animCurveNodeName.append("_");
       animCurveNodeName.append(propName.data());
       animCurveNodeName.append("_animCurve");

       auto animCurveNode = createNode(dGraph, animCurveNodeName, "property");
       animCurveNode.setDataInfo(fbxViz::getCurveAnimKeyInfo(curveNode));
       dGraph.addNode(animCurveNode);

       createEdge(dGraph, propertyNode, animCurveNode);
    }

    void getAnimCurveInfo(fbxViz::DGraph& dGraph, FbxNode* pNode, FbxAnimLayer* pAnimLayer, fbxViz::Node& animNode) 
    {
        FbxAnimCurve* curveNode{nullptr};
        curveNode = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        // T
        if (curveNode) {
           curveKeyInfo(dGraph, pNode, "TX", curveNode, animNode);
        }
        curveNode = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (curveNode) {
           curveKeyInfo(dGraph, pNode, "TY", curveNode, animNode);
        }
        curveNode = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (curveNode) {
           curveKeyInfo(dGraph, pNode, "TZ", curveNode, animNode);
        }
        // R
        curveNode = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if (curveNode){
            curveKeyInfo(dGraph, pNode, "RX", curveNode, animNode);
        }
        curveNode = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (curveNode){
            curveKeyInfo(dGraph, pNode, "RY", curveNode, animNode);
        }
        curveNode = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (curveNode){
            curveKeyInfo(dGraph, pNode, "RZ", curveNode, animNode);
        }
        // S
        curveNode = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
        if (curveNode){
            curveKeyInfo(dGraph, pNode, "SX", curveNode, animNode);
        }
        curveNode = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
        if (curveNode){
            curveKeyInfo(dGraph, pNode, "SY", curveNode, animNode);
        }
        curveNode = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
        if (curveNode){
            curveKeyInfo(dGraph, pNode, "SZ", curveNode, animNode);
        }
    }

    // traverse animation layer recursively
    void traverseAnimationLayer(fbxViz::DGraph& dGraph, FbxAnimStack* pAnimStack, FbxAnimLayer* pAnimLayer, FbxNode* pNode, fbxViz::Node& animLayerNode)
    {
        // if the curve count is greater than zero, the node has animation info 
        // that we are interested in
        FbxArray<FbxAnimCurve*> curves;
        GetAllAnimCurves( pNode, pAnimStack, curves );
        if (curves.GetCount() > 0 ) {

            auto animNode = createNode(dGraph, pNode->GetName(), getAttrName(pNode));
            createEdge(dGraph, animLayerNode, animNode);

            getAnimCurveInfo(dGraph, pNode, pAnimLayer, animNode);
        }

        for(auto i = 0; i < pNode->GetChildCount(); ++i) {
            traverseAnimationLayer(dGraph, pAnimStack, pAnimLayer, pNode->GetChild(i), animLayerNode);
        }
    }

} // namespace

namespace fbxViz {

bool initialize(FbxManager*& pManager, FbxScene*& pScene)
{
    pManager = FbxManager::Create();
    if( !pManager ) {
        fmt::print("Error: Unable to create FBX Manager!\n");
        return false;
    }
    else {
        fmt::print("Autodesk FBX SDK version {}\n", pManager->GetVersion());
    }

    FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
    pManager->SetIOSettings(ios);

    // create an FBX scene. This object holds most objects imported/exported from/to files.
    pScene = FbxScene::Create(pManager, "Empty Scene");
    if( !pScene ) {
        fmt::print("Error: Unable to create FBX scene!\n");
        return false;
    }

    return true;
}

void destroy(FbxManager* pManager)
{
    if( pManager ) {
        pManager->Destroy();
        fmt::print("FBX Sdk destroyed successfully!\n");
    }
}

bool loadScene(FbxManager* pManager, FbxDocument* pScene, std::string_view pFilename)
{
    auto fileMajor{-1}, fileMinor{-1}, fileRevision{-1};
    bool status = false;

    // create an importer
    FbxImporter* importer = FbxImporter::Create(pManager, "");

    // initialize the importer
    status = importer->Initialize(pFilename.data(), -1, pManager->GetIOSettings());
    if (!status) {
        return false;
    }

    if (importer->IsFBX()) {
        importer->GetFileVersion(fileMajor, fileMinor, fileRevision);
        fmt::print("FBX file format version for file {} is {}.{}.{}\n", pFilename.data(), fileMajor, fileMinor, fileRevision);
    }

    // import the contents of the file into the scene.
    status = importer->Import(pScene);
    
    if (status) {
        // check the scene integrity
        FbxStatus stat;
        FbxArray< FbxString*> details;
        FbxSceneCheckUtility sceneCheck(FbxCast<FbxScene>(pScene), &stat, &details);
        status = sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData);
    }

    // destroy the importer
    importer->Destroy();

    return status;
}

void dumpSceneToFile(FbxScene* pScene, std::string_view fileName)
{
    FbxNode* rootNode = pScene->GetRootNode();

    if (rootNode) {

        DGraph dGraph("dag_scene");
        
        // root node ( graph )
        auto dgRoot = createNode(dGraph, rootNode->GetName(), getAttrName(rootNode));
            
        for(auto i=0; i < rootNode->GetChildCount(); ++i) {

            auto childNode = createNode(dGraph, rootNode->GetChild(i)->GetName(), getAttrName(rootNode->GetChild(i)));
            createEdge(dGraph, dgRoot, childNode);

            traverseNode(dGraph, rootNode->GetChild(i));
        }

        dGraph.dumpDag(fileName);
    }
}

void dumpAnimStackToFile(FbxScene* pScene, std::string_view fileName)
{
    FbxNode* rootNode = pScene->GetRootNode();

    if (rootNode) {

        DGraph dGraph("dag_animstack");
        
        for (auto i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++) {

            FbxAnimStack* animStack = pScene->GetSrcObject<FbxAnimStack>(i);
        
            // animation stack node ( graph )
            auto animStackNode = createNode(dGraph, animStack->GetName(), "AnimationStack");

            // traverse animation stack
            for (auto i = 0; i < animStack->GetMemberCount<FbxAnimLayer>(); ++i) {
                            
                FbxAnimLayer* animLayer = animStack->GetMember<FbxAnimLayer>(i);

                auto animLayerNode = createNode(dGraph, animLayer->GetName(), "AnimationLayer");
                createEdge(dGraph, animStackNode, animLayerNode);
                
                traverseAnimationLayer(dGraph, animStack, animLayer, pScene->GetRootNode(), animLayerNode);
            }
        }

        dGraph.dumpAnimStack(fileName);   
    }
}

} // fbxViz namespace
