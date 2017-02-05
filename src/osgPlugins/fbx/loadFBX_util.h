
#ifndef _LOAD_FBX_UTIL_H_
#define _LOAD_FBX_UTIL_H_

#include <winNls.h>

//#include<iostream>
//#include<sstream>
//#include<map>
//#include<vector>

//#include <fbxsdk.h>

//#include<osg/Matrix>
//#include<osg/Texture2D>
//#include<osg/Material>
//#include<osgDB/ReadFile>


static std::wstring m2w(std::string ch, unsigned int CodePage = CP_ACP)
{
    if (ch.empty())return L"";
    std::wstring ret;
    DWORD dwOutSize = 0;
    dwOutSize = MultiByteToWideChar(CodePage, 0, ch.c_str(), -1, NULL, 0);

    ret.resize(dwOutSize - 1);
    MultiByteToWideChar(CodePage, 0, ch.c_str(), ch.size(), &ret[0], dwOutSize);

    return ret;
}

static std::string w2m(std::wstring wch, unsigned int CodePage = CP_ACP)
{
    std::string ret;
    DWORD dwOutSize = 0;
    dwOutSize = WideCharToMultiByte(CodePage, 0, wch.c_str(), -1, NULL, 0, NULL, FALSE);

    char *pwText = 0;
    pwText = new char[dwOutSize];
    pwText[dwOutSize - 1] = '\0';

    WideCharToMultiByte(CodePage, 0, wch.c_str(), wch.size(), pwText, dwOutSize, NULL, FALSE);

    ret = pwText;
    if (pwText)delete[]pwText;

    return ret;
}


//合并当前node的local矩阵
static osg::Matrix getLocalMatrix(FbxNode* node)
{
    //vmml::mat4f mat;
    //vmml::identity(mat);

    //osg::Matrix mat;

    //使用FBX评估方式 覆盖这个结果
    FbxMatrix m = node->EvaluateLocalTransform(0);
    double* dm = m;
    //float fm[16];
    //for (int i = 0; i < 16; i++)
    //{
        //fm[i] = dm[i];
    //}
    //mat.set(&fm[0], &fm[15], false);

    //
    //return mat;
    return osg::Matrix(
        dm[0], dm[1], dm[2], dm[3]
        , dm[4], dm[5], dm[6], dm[7]
        , dm[8], dm[9], dm[10], dm[11]
        , dm[12], dm[13], dm[14], dm[15]
    );
}

static osg::Matrix getWorldMatrix(FbxNode* node)
{
    //vmml::mat4f mat;
    //vmml::identity(mat);

    //使用FBX评估方式 覆盖这个结果
    FbxMatrix m = node->EvaluateGlobalTransform(0);
    double* dm = m;
    //float fm[16];
    //for (int i = 0; i < 16; i++)
    //{
    //    fm[i] = dm[i];
    //}
    //mat.set(&fm[0], &fm[15], false);

    //
    //return mat;
    return osg::Matrix(
        dm[0], dm[1], dm[2], dm[3]
        , dm[4], dm[5], dm[6], dm[7]
        , dm[8], dm[9], dm[10], dm[11]
        , dm[12], dm[13], dm[14], dm[15]
    );
}


static bool hasSkin(FbxMesh* pMesh)
{
    int deformerCount = pMesh->GetDeformerCount();
    FbxDeformer* pFBXDeformer;
    FbxSkin*     pFBXSkin;

    for (int i = 0; i < deformerCount; ++i)
    {
        //获取变形器
        pFBXDeformer = pMesh->GetDeformer(i);
        if (pFBXDeformer == NULL) { continue; }

        //获取skin类型
        if (pFBXDeformer->GetDeformerType() != FbxDeformer::eSkin) { continue; }

        //获取skin对象
        pFBXSkin = (FbxSkin*)(pFBXDeformer);
        if (pFBXSkin == NULL) { continue; }

        return true;
    }

    return false;
}

static void getCustomNodeProperty(FbxNode* pNode)
{
    FbxProperty p;

    p = pNode->FindProperty("UDP3DSMAX", false);
    if (p.IsValid())
    {
        std::string nodeName = p.GetName();
        std::cout << "found property: " << nodeName << std::endl;
    }
}



//遍历场景搜索joint的bindpose位置
static void getPose(FbxScene* fbx_scene)
{
    unsigned int num_pose = fbx_scene->GetPoseCount();
    for (unsigned int i = 0; i < num_pose; i++)
    {
        FbxPose* pose = fbx_scene->GetPose(i);
        //pose->SetIsBindPose(true);
        if (!pose->IsBindPose()) continue;

        unsigned int num_node = pose->GetCount();
        for (unsigned int j = 0; j < num_node; j++)
        {
            FbxNode* node = pose->GetNode(j);

            //这里得到的都是bind的世界矩阵
            FbxMatrix m = pose->GetMatrix(j);

            printf("");
        }
    }
}

static std::vector<char> readfile(std::string filename)
{
    std::vector<char> ret;
    std::ifstream fl(filename, std::ios::binary);
    if (fl.good())
    {
        fl.seekg(0, std::ios::end);
        size_t len = fl.tellg();
        ret.resize(len);
        fl.seekg(0, std::ios::beg);
        fl.read(&ret[0], len);
        fl.close();
    }
    else
    {
        printf("read file failed : %s\n", filename.c_str());
    }
    return ret;
}


#endif // !_LOAD_FBX_UTIL_H_
