
#ifndef _LOAD_FBX_H_
#define _LOAD_FBX_H_

//#include <iostream>
//#include <fstream>

#include<osgDB/ReadFile>
#include<osg/MatrixTransform>

//#include <fbxsdk.h>

osg::Node* loadFBX(const std::string& filename);


#endif // !_LOAD_FBX_H_
