#include "curveInfo.h"

#include <fmt/core.h>

#include <fstream>

namespace {

// Key interpolation type
std::string getKeyInterpolationType( FbxAnimCurve* animCurve, int keyIndex ) {
    switch ( animCurve->KeyGetInterpolation( keyIndex ) ) {
    case FbxAnimCurveDef::eInterpolationConstant:
        return "constant"; // Animation Curve value stays the same until next key

        break;

    case FbxAnimCurveDef::eInterpolationLinear:
        return "linear"; // Animation curve is a straight line

        break;

    case FbxAnimCurveDef::eInterpolationCubic:
        return "cubic"; // Animation curve is a Bezier spline

        break;

    default:
        return "?";
    }
}

// Key Tangent Mode. This method is meaningful for cubic interpolation only.
// Using this method for non cubic interpolated key will return unpredictable value.
std::string getKeyTangentMode( FbxAnimCurve* animCurve, int keyIndex ) {

    switch ( animCurve->KeyGetTangentMode( keyIndex ) ) {
    case FbxAnimCurveDef::eTangentAuto:
        return "Auto"; // Spline cardinal

        break;

    case FbxAnimCurveDef::eTangentAutoBreak:
        return "AutoBreak";

        break;

    case FbxAnimCurveDef::eTangentTCB:
        return "TCB"; // Tension, Continuity, Bias

        break;

    case FbxAnimCurveDef::eTangentUser:
        return "User"; // Next slope at the left equal to slope at the right

        break;

    case FbxAnimCurveDef::eTangentGenericBreak:
        return "GenericBreak";

        break;

    case FbxAnimCurveDef::eTangentBreak:
        return "Break";

        break;

    default:
        return "?";
    }
}

// Get key's tangent weight mode. This method is meaningful for cubic interpolation only.
std::string getTangentweight( FbxAnimCurve* animCurve, int keyIndex ) {
    switch ( animCurve->KeyGet( keyIndex ).GetTangentWeightMode() ) {
    case FbxAnimCurveDef::eWeightedNone:
        return "None"; // Tangent has default weights of 0.333; we define this state as not weighted.

        break;

    case FbxAnimCurveDef::eWeightedRight:
        return "WeightedRight"; // Right tangent is weighted.

        break;

    case FbxAnimCurveDef::eWeightedNextLeft:
        return "WeightedNextLeft"; // Left tangent is weighted.

        break;

    case FbxAnimCurveDef::eWeightedAll:
        return "WeightedAll"; // Both left and right tangents are weighted

        break;

    default:
        return "?";
    }
}

// Get key's constant mode. This method is only relevant if the key's interpolation
// type is constant (eInterpolationConstant).
std::string getKeyConstantMode( FbxAnimCurve* animCurve, int keyIndex ) {
    switch ( animCurve->KeyGetConstantMode( keyIndex ) ) {
    case FbxAnimCurveDef::eConstantStandard:
        return "ConstantStandard";  // Curve value is constant between this key and the next.

        break;

    case FbxAnimCurveDef::eConstantNext:
        return "ConstantNext";     // Curve value is constant, with next key's value.

        break;

    default:
        return "?";
    }
}

} // anonymous namespace

namespace fbxViz {

std::string getCurveAnimKeyInfo( FbxAnimCurve* animCurve ) {

    std::string outputStr;
    char timeString[256];

    const auto keyCount = animCurve->KeyGetCount();
    outputStr = "<tr><td align='left'><b>";
    outputStr += "Key counts= ";
    outputStr += "<font color='red'><b>";
    outputStr += std::to_string( keyCount );
    outputStr += "</b></font>";
    outputStr += "</b></td></tr>\n";

    for ( auto keyIndex = 0; keyIndex < keyCount; ++keyIndex ) {
        auto keyTime = animCurve->KeyGetTime( keyIndex );
        auto keyValue = animCurve->KeyGetValue( keyIndex );
        auto keyInterpolationType = getKeyInterpolationType( animCurve, keyIndex );

        outputStr += "<tr><td align='left'>";
        outputStr += "Key Time= ";
        outputStr += "<font color='red'><b>";
        outputStr += keyTime.GetTimeString( timeString, sizeof( timeString ) );
        outputStr += "</b></font>";
        outputStr += " , ";
        outputStr += "Key Value= ";
        outputStr += "<font color='red'><b>";
        outputStr += std::to_string( keyValue );
        outputStr += "</b></font>";

        outputStr += " , ";
        outputStr += "InterpolationType= ";
        outputStr += "<font color='red'><b>";
        outputStr += keyInterpolationType;
        outputStr += "</b></font>";

        if ( "cubic" == keyInterpolationType ) {
            outputStr += " , ";
            outputStr += "TangentMode= ";
            outputStr += "<font color='red'><b>";
            outputStr += getKeyTangentMode( animCurve, keyIndex );
            outputStr += "</b></font>";
            outputStr += ", ";
            outputStr += "TangentWeight= ";
            outputStr += "<font color='red'><b>";
            outputStr += getTangentweight( animCurve, keyIndex );
            outputStr += "</b></font>";
        } else if ( "constant" == keyInterpolationType ) {
            outputStr += " , ";
            outputStr += "<font color='red'><b>";
            outputStr += getKeyConstantMode( animCurve, keyIndex );
            outputStr += "</b></font>";
            outputStr += ", ";
        }
        outputStr += "</td></tr>\n";
    }

    return outputStr;
}

} // namespace fbxViz
