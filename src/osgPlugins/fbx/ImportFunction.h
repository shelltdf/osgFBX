
#ifndef _EXPORT_FUNCTION_H_
#define _EXPORT_FUNCTION_H_

#include <windows.h>
#include <sstream>

#include<osg/Matrix>
#include<osg/Texture2D>
#include<osg/Material>
#include<osg/StateSet>
#include<osg/LightSource>
#include<osg/CameraView>
#include<osgDB/ReadFile>
#include<osg/BlendFunc>

//#include <osgCore/Utils>

#include <fbxsdk.h>


osg::StateSet* LoadMaterialAttribute(FbxSurfaceMaterial* pSurfaceMaterial /*, std::map<std::string,std::string>& copy_image_list*/);

osg::Texture* LoadMaterialTexture(FbxProperty* pProperty /*, std::map<std::string,std::string>& copy_image_list*/);

osg::Geode* exportMesh(FbxNode* pNode, FbxMesh* mesh
    //,nbSG::SkeletonActorControler* sac , std::map<std::string,std::string>& copy_image_list ,bool world_position 
);

osg::Geode* exportLine(FbxNode* pNode, FbxLine* line);

osg::CameraView* exportCamera(FbxNode* pNode, FbxCamera* camera);

osg::LightSource* exportLight(FbxNode* pNode, FbxLight* light);

#endif // !_EXPORT_FUNCTION_H_
