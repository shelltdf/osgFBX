
#include "ImportFunction.h"

using namespace fbxsdk;


osg::CameraView* exportCamera(FbxNode* pNode, FbxCamera* camera)
{
    osg::CameraView* ret = new osg::CameraView();

    return ret;
}

osg::LightSource* exportLight(FbxNode* pNode, FbxLight* light)
{
    osg::LightSource* ret = new osg::LightSource();
    osg::Light* li = new osg::Light();
    ret->setLight(li);

    li->setPosition(osg::Vec4(0, 0, 0, 1));

    return ret;
}

