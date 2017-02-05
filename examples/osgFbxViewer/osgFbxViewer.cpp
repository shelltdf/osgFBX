

#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileNameUtils>
#include <osgViewer/Viewer>


int main(int argc, char **argv)
{

    //viewer
    osgViewer::Viewer viewer;
    osg::Group* root = new osg::Group();
    viewer.setSceneData(root);

    ////
    //bool save_to_pointcloud = false;

    //osgDB::Options* ops = new osgDB::Options();
    //if (save_to_pointcloud) ops->setOptionString("draco_point_cloud");


    //offset
    osg::MatrixTransform* mt = new osg::MatrixTransform();
    mt->setMatrix(osg::Matrix::translate(0, 0, 0));

    //load from .fbx
    osg::Node* node_drc = osgDB::readNodeFile("../data/static_mesh.fbx");
    mt->addChild(node_drc);
    root->addChild(mt);

    //run
    viewer.setUpViewInWindow(100, 100, 1280, 720);
    viewer.run();

    return 0;
}

