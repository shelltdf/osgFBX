#include <osg/Notify>
#include <osg/Geode>
#include <osg/Geometry>

#include <osgDB/FileNameUtils>
#include <osgDB/FileUtils>
#include <osgDB/fstream>
#include <osgDB/Registry>

#include <iostream>
#include <stdio.h>
#include <string.h>

#include "loadFBX.h"

struct FBXLoadOptionsStruct
{
    FBXLoadOptionsStruct()
    {
        staticMeshOnly = true;
        characterOnly = false;
        animationOnly = false;
        characterAndanimation = false;
    }

    bool staticMeshOnly;
    bool characterOnly;
    bool animationOnly;
    bool characterAndanimation;
};

FBXLoadOptionsStruct parseLoadOptions(const osgDB::ReaderWriter::Options* options)
{
    FBXLoadOptionsStruct opt;

    if (options != NULL)
    {
        std::istringstream iss(options->getOptionString());
        std::string opt;
        while (iss >> opt)
        {
            //if (opt == "draco_point_cloud")
            //{
            //    opt.isPointCloud = true;
            //}
        }
    }

    return opt;
}


class ReaderWriterFBX
    : public osgDB::ReaderWriter
{
public:

    ReaderWriterFBX()
    {
        supportsExtension("fbx", "fbx format");

        //supportsOption("draco_point_cloud", "save file as PointCloud");
    }

    virtual const char* className() const { return "Fbx reader/writer"; }

    virtual ReadResult readNode(const std::string& file, const osgDB::ReaderWriter::Options* options) const
    {
        std::string ext = osgDB::getLowerCaseFileExtension(file);
        if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

        std::string fileName = osgDB::findDataFile(file, options);
        if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;

        OSG_INFO << "Begin reading file " << fileName << std::endl;

        //perser options
        FBXLoadOptionsStruct fbx_load_options = parseLoadOptions(options);

        ReadResult ret = ReadResult::FILE_NOT_FOUND;

        //load fbx
        osg::Node* node = loadFBX(fileName);
        if (node)
        {
            ret = node;
        }

        OSG_INFO << "End reading file " << fileName << std::endl;
        return ret;
    }

    virtual WriteResult writeNode(const osg::Node& node, const std::string& fileName
        , const Options* options = NULL) const
    {
        return WriteResult::FILE_NOT_HANDLED;

        //std::string ext = osgDB::getLowerCaseFileExtension(fileName);
        //if (!acceptsExtension(ext)) return WriteResult::FILE_NOT_HANDLED;

        //OSG_INFO << "Writing file " << fileName << std::endl;

        return WriteResult::FILE_SAVED;
    }
};

REGISTER_OSGPLUGIN(fbx, ReaderWriterFBX)
