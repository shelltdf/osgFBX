
#include "ImportFunction.h"

//#include "loadFBX_util.h"
//#include <nbSG/Public.h>

using namespace fbxsdk;


osg::Texture* LoadMaterialTexture(FbxProperty* pProperty
    //, std::map<std::string, std::string>& copy_image_list
)
{
    osg::Texture* tex = 0;

    if (pProperty)
    {
        //查询贴图数量
        int textureCount = pProperty->GetSrcObjectCount(FbxTexture::ClassId);

        //目前只处理第一个找到的贴图
        for (int j = 0; j < textureCount; ++j)
        {
            FbxTexture* pTexture = FbxCast<FbxTexture>(pProperty->GetSrcObject(FbxTexture::ClassId, j));
            if (pTexture)
            {
                printf("Texture %s: %s\n", pProperty->GetName().Buffer(), pTexture->GetName());

                //如果贴图存在
                tex = new osg::Texture2D();
                //tex->setUniformPath(pTexture->GetName());
                tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
                tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
                tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
                tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
                tex->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);


                //如果是file贴图
                FbxFileTexture* file_tex = FbxCast<FbxFileTexture>(pTexture);
                if (file_tex)
                {
                    std::string filename = file_tex->GetFileName(); //绝对路径
                    osg::Image* image = osgDB::readImageFile(filename);
                    if (image)
                    {
                        tex->setImage(0,image);
                    }

                    ////处理image对象
                    //nbSG::Image* image = new nbSG::Image();
                    //tex->setImage(image);

                    ////路径处理
                    //std::string filename = file_tex->GetFileName(); //绝对路径
                    //nbBase::Path p(filename);
                    //std::string name = p.getLastName();

                    ////设置对象
                    //image->setUniformPath(name);
                    //image->setExternalPath(name);

                    ////复制外部文件
                    ////  从 filename 绝对路径
                    ////  到 name     uniform路径
                    ////printf("copy %s %s\n" ,filename.c_str() , name.c_str() );
                    //copy_image_list[nbBase::Path(filename).getNativePath()] = name;
                }

                //只返回第一个
                return tex;
            }
        }
    }

    return tex;
}

