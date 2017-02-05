
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
        //��ѯ��ͼ����
        int textureCount = pProperty->GetSrcObjectCount(FbxTexture::ClassId);

        //Ŀǰֻ�����һ���ҵ�����ͼ
        for (int j = 0; j < textureCount; ++j)
        {
            FbxTexture* pTexture = FbxCast<FbxTexture>(pProperty->GetSrcObject(FbxTexture::ClassId, j));
            if (pTexture)
            {
                printf("Texture %s: %s\n", pProperty->GetName().Buffer(), pTexture->GetName());

                //�����ͼ����
                tex = new osg::Texture2D();
                //tex->setUniformPath(pTexture->GetName());
                tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
                tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
                tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
                tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
                tex->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);


                //�����file��ͼ
                FbxFileTexture* file_tex = FbxCast<FbxFileTexture>(pTexture);
                if (file_tex)
                {
                    std::string filename = file_tex->GetFileName(); //����·��
                    osg::Image* image = osgDB::readImageFile(filename);
                    if (image)
                    {
                        tex->setImage(0,image);
                    }

                    ////����image����
                    //nbSG::Image* image = new nbSG::Image();
                    //tex->setImage(image);

                    ////·������
                    //std::string filename = file_tex->GetFileName(); //����·��
                    //nbBase::Path p(filename);
                    //std::string name = p.getLastName();

                    ////���ö���
                    //image->setUniformPath(name);
                    //image->setExternalPath(name);

                    ////�����ⲿ�ļ�
                    ////  �� filename ����·��
                    ////  �� name     uniform·��
                    ////printf("copy %s %s\n" ,filename.c_str() , name.c_str() );
                    //copy_image_list[nbBase::Path(filename).getNativePath()] = name;
                }

                //ֻ���ص�һ��
                return tex;
            }
        }
    }

    return tex;
}

