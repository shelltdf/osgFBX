
#include "ImportFunction.h"

using namespace fbxsdk;

osg::Geode* exportLine(FbxNode*  pNode, FbxLine* line)
{
    osg::Geode* ret = new osg::Geode();
    osg::Geometry* geometry(new osg::Geometry());
    osg::Vec3Array* vertices(new osg::Vec3Array());
    osg::Vec4Array* colors(new osg::Vec4Array());

    int count = line->GetControlPointsCount();
    for (size_t i = 0; i < count; i++) //不知道为什么 最后一个点是中心点
    {
        //line->GetPointIndexAt();
        FbxVector4 p = line->GetControlPointAt(i);

        vertices->push_back(osg::Vec3(p[0], p[1], p[2]));
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    }

    geometry->setVertexArray(vertices);
    geometry->setColorArray(colors);

    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    geometry->addPrimitiveSet(new osg::DrawArrays(
        osg::PrimitiveSet::LINE_LOOP, 0, count-1));

    ret->addDrawable(geometry);
    ret->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
    //ret->getOrCreateStateSet()->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0.0, 0.0001));

    return ret;
}



