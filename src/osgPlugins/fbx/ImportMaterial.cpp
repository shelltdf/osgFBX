
#include "ImportFunction.h"

//#include <iostream>
//
//#include <fbxsdk.h>
//
//#include "loadFBX_util.h"
//#include <nbSG/Public.h>

using namespace fbxsdk;


//extern osg::Texture* LoadMaterialTexture( FbxProperty* pProperty /*, std::map<std::string,std::string>& copy_image_list*/ );
//extern nbBase::ref_ptr<nbSG::Program> maya_normalmap_program;
//extern nbBase::ref_ptr<nbSG::Program> maya_skinning_normalmap_program;

#if 0
class CustomMaterialProperty
{
public:
    CustomMaterialProperty()
        :_skin(false)
    {
    }
    bool _skin;
};

CustomMaterialProperty getCustomMaterialProperty( FbxSurfaceMaterial* pNode )
{
    FbxProperty p;
    CustomMaterialProperty ret;
        
    p = pNode->FindProperty("_skin", false);
    if (p.IsValid())
    {
        std::string nodeName = p.GetName();
        std::cout << "found property: " << nodeName <<std::endl;
        ret._skin = p.Get<FbxBool>();
    }

    return ret;
}
#endif

osg::StateSet* LoadMaterialAttribute( FbxSurfaceMaterial* pSurfaceMaterial /*, std::map<std::string,std::string>& copy_image_list*/ )
{
    //查询自定义属性
    //CustomMaterialProperty cmp = getCustomMaterialProperty( pSurfaceMaterial );

    // Get the name of material  
    //pSurfaceMaterial->GetName();
    //printf("Material type = %s name = %s\n" , pSurfaceMaterial->GetClassId().GetName(),pSurfaceMaterial->GetName() );

    //建立材质
    osg::Material* mat = new osg::Material();
    //mat->setUniformPath( pSurfaceMaterial->GetName() );

    //初始化默认材质信息 如果不能被正确识别 就显示紫色
    //nbSG::Material::Tech* tech = mat->getTech();
    //nbSG::Material::Pass* pass0 = new nbSG::Material::Pass();
    //tech->mPasses.push_back(pass0);

    float ambient[4] = {0.5f, 0.5f, 0.5f, 1.0f};
    float diffuse[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    float emission[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float opacity = 1.0f;
    float specular[4] = {0.0f, 0.0f, 0.0f, 1.0f};   //适应Lambert的默认值
    float shininess = 0.0f;                         //适应Lambert的默认值
    float reflectivity = 0.0f;                      //适应Lambert的默认值


    /*
    材质输出标准
    tex0 diffuse
    tex1 lightmap/ambient
    tex2 normal
    tex3 emission/illumination
    tex4 specular
    tex5 reflection
    */
    osg::Texture* tex_diffuse    = 0;   //本色
    osg::Texture* tex_ambient    = 0;   //lightmap
    osg::Texture* tex_bump       = 0;   //normalmap
    osg::Texture* tex_emissive   = 0;   //Emissive
    osg::Texture* tex_specular   = 0;   //高光
    osg::Texture* tex_reflection = 0;   //反射

    //FbxSurfacePhong* sp = dynamic_cast<FbxSurfacePhong*>(pSurfaceMaterial);
    //FbxSurfaceLambert* sl = dynamic_cast<FbxSurfaceLambert*>(pSurfaceMaterial);


    // Phong material  
    if(pSurfaceMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))  
    {
        FbxDouble3 AmbientColor  = ((FbxSurfacePhong*)pSurfaceMaterial)->Ambient;
        FbxDouble3 DiffuseColor  = ((FbxSurfacePhong*)pSurfaceMaterial)->Diffuse;
        FbxDouble3 EmissiveColor = ((FbxSurfacePhong*)pSurfaceMaterial)->Emissive;
        FbxDouble3 SpecularColor = ((FbxSurfacePhong*)pSurfaceMaterial)->Specular;
        FbxDouble  Shininess     = ((FbxSurfacePhong*)pSurfaceMaterial)->Shininess;
        FbxDouble  Opacity       = ((FbxSurfacePhong*)pSurfaceMaterial)->TransparencyFactor;
        FbxDouble  Reflectivity  = ((FbxSurfacePhong*)pSurfaceMaterial)->ReflectionFactor;
        FbxDouble3 TransparentColor = ((FbxSurfacePhong*)pSurfaceMaterial)->TransparentColor;

        //重新计算max和maya的透明度设置
        osg::Vec3 tc(TransparentColor[0], TransparentColor[1], TransparentColor[2]);
        tc.normalize();
        opacity = tc.length() * Opacity;

        ambient[0] = AmbientColor[0];
        ambient[1] = AmbientColor[1];
        ambient[2] = AmbientColor[2];
        ambient[3] = 1.0;

        diffuse[0] = DiffuseColor[0];
        diffuse[1] = DiffuseColor[1];
        diffuse[2] = DiffuseColor[2];
        diffuse[3] = 1.0 - opacity;

        emission[0] = EmissiveColor[0];
        emission[1] = EmissiveColor[1];
        emission[2] = EmissiveColor[2];
        emission[3] = 1.0;

        specular[0] = SpecularColor[0];
        specular[1] = SpecularColor[1];
        specular[2] = SpecularColor[2];
        specular[3] = 1.0;

        shininess = Shininess;


        tex_diffuse    = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->Diffuse /*,copy_image_list*/);    //本色
        tex_ambient    = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->Ambient /*,copy_image_list*/);    //lightmap
        tex_bump       = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->Bump /*,copy_image_list*/);       //normalmap
        if(!tex_bump)tex_bump = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->NormalMap /*,copy_image_list*/);       //normalmap
        tex_emissive   = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->Emissive /*,copy_image_list*/);       //Emissive
        tex_specular   = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->Specular /*,copy_image_list*/);   //高光
        tex_reflection = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->Reflection /*,copy_image_list*/); //反射

        //return mat;
    }

    // Lambert material  
    else if(pSurfaceMaterial->GetClassId().Is( FbxSurfaceLambert::ClassId ))  
    {  
        FbxDouble3 AmbientColor  = ((FbxSurfaceLambert*)pSurfaceMaterial)->Ambient;
        FbxDouble3 DiffuseColor  = ((FbxSurfaceLambert*)pSurfaceMaterial)->Diffuse;
        FbxDouble3 EmissiveColor = ((FbxSurfaceLambert*)pSurfaceMaterial)->Emissive;
        FbxDouble  Opacity       = ((FbxSurfaceLambert*)pSurfaceMaterial)->TransparencyFactor;
        FbxDouble3 TransparentColor = ((FbxSurfacePhong*)pSurfaceMaterial)->TransparentColor;


        //重新计算max和maya的透明度设置
        osg::Vec3 tc(TransparentColor[0], TransparentColor[1], TransparentColor[2]);
        tc.normalize();
        opacity = tc.length() * Opacity;

        ambient[0] = AmbientColor[0];
        ambient[1] = AmbientColor[1];
        ambient[2] = AmbientColor[2];
        ambient[3] = 1.0;

        diffuse[0] = DiffuseColor[0];
        diffuse[1] = DiffuseColor[1];
        diffuse[2] = DiffuseColor[2];
        diffuse[3] = 1.0 - opacity;

        emission[0] = EmissiveColor[0];
        emission[1] = EmissiveColor[1];
        emission[2] = EmissiveColor[2];
        emission[3] = 1.0;

        tex_ambient = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->Ambient /*,copy_image_list*/);
        tex_diffuse = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->Diffuse /*,copy_image_list*/);
        tex_bump    = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->Bump /*,copy_image_list*/);
        if(!tex_bump)tex_bump = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->NormalMap /*,copy_image_list*/);       //normalmap
        tex_emissive   = LoadMaterialTexture( &((FbxSurfacePhong*)pSurfaceMaterial)->Emissive /*,copy_image_list*/);

        //return mat;
    }
    else
    {
        //printf("missing Material\n");

        printf("missing Material %s\n" , pSurfaceMaterial->GetClassId().GetName());


    }

    ////设置默认shader
    //if(cmp._skin)
    //{
    //    pass0->setProgram(maya_skinning_normalmap_program.get());
    //}
    //else
    //{
    //    pass0->setProgram(maya_normalmap_program.get());
    //}


    //pass0->setUniform<float>(NFX_gl_FrontMaterial_ambient ,nbSG::UniformBase::UniformDataType::UDT_FLOAT_VEC4 , &ambient[0] , 4 ,"");
    //pass0->setUniform<float>(NFX_gl_FrontMaterial_diffuse ,nbSG::UniformBase::UniformDataType::UDT_FLOAT_VEC4 , &diffuse[0] , 4 ,"");
    //pass0->setUniform<float>(NFX_gl_FrontMaterial_emission ,nbSG::UniformBase::UniformDataType::UDT_FLOAT_VEC4 , &emission[0] , 4 ,"");
    //pass0->setUniform<float>(NFX_gl_FrontMaterial_opacity ,nbSG::UniformBase::UniformDataType::UDT_FLOAT , &opacity , 1 ,"");
    //pass0->setUniform<float>(NFX_gl_FrontMaterial_specular ,nbSG::UniformBase::UniformDataType::UDT_FLOAT_VEC4 , &specular[0] , 4 ,"");
    //pass0->setUniform<float>(NFX_gl_FrontMaterial_shininess ,nbSG::UniformBase::UniformDataType::UDT_FLOAT , &shininess , 1 ,"");
    //pass0->setUniform<float>(NFX_gl_FrontMaterial_reflectivity ,nbSG::UniformBase::UniformDataType::UDT_FLOAT , &reflectivity , 1 ,"");

    mat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(ambient[0], ambient[1], ambient[2], ambient[3]));
    mat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(diffuse[0], diffuse[1], diffuse[2], diffuse[3]));
    mat->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(emission[0], emission[1], emission[2], emission[3]));
    mat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(specular[0], specular[1], specular[2], specular[3]));
    mat->setShininess(osg::Material::FRONT_AND_BACK, shininess);

    osg::StateSet* ss = new osg::StateSet();
    ss->setAttributeAndModes(mat);
    ss->setName(pSurfaceMaterial->GetName());

    //如果有透明设置
    if (diffuse[3] < 1.0)
    {
        osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
        //osg::Depth* dp = new osg::Depth(osg::Depth::LESS,0,1,false);
        ss->setAttributeAndModes(bf, osg::StateAttribute::ON /*| osg::StateAttribute::OVERRIDE*/);
        //ss->setAttributeAndModes(dp  , osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
        ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        //ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
        //ss->setMode(GL_ALPHA_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
        //ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
    }

    if(tex_diffuse)
    {
        //pass0->setTextureUnit( NB_UNIFORM_TEXTURE0 ,0);
        //pass0->setTexture(0,tex_diffuse);

        ss->setTextureAttributeAndModes(0, tex_diffuse);
        ss->addUniform(new osg::Uniform("texture_0", 0));
    }
    if(tex_ambient)
    {
        //pass0->setTextureUnit( NB_UNIFORM_TEXTURE1 ,1);
        //pass0->setTexture(1,tex_ambient);

        ss->setTextureAttributeAndModes(1, tex_ambient);
        ss->addUniform(new osg::Uniform("texture_1", 1));
    }
    if(tex_bump)
    {
        //pass0->setTextureUnit( NB_UNIFORM_TEXTURE2 ,2);
        //pass0->setTexture(2,tex_bump);

        ss->setTextureAttributeAndModes(2, tex_bump);
        ss->addUniform(new osg::Uniform("texture_2", 2));
    }
    if(tex_emissive)
    {
        //pass0->setTextureUnit( NB_UNIFORM_TEXTURE3 ,3);
        //pass0->setTexture(3,tex_emissive);

        ss->setTextureAttributeAndModes(3, tex_emissive);
        ss->addUniform(new osg::Uniform("texture_0", 3));
    }
    if(tex_specular)
    {
        //pass0->setTextureUnit( NB_UNIFORM_TEXTURE4 ,4);
        //pass0->setTexture(4,tex_specular);

        ss->setTextureAttributeAndModes(4, tex_specular);
        ss->addUniform(new osg::Uniform("texture_0", 4));
    }
    if(tex_reflection)
    {
        //pass0->setTextureUnit( NB_UNIFORM_TEXTURE5 ,5);
        //pass0->setTexture(5,tex_reflection);

        ss->setTextureAttributeAndModes(5, tex_reflection);
        ss->addUniform(new osg::Uniform("texture_0", 5));
    }

    //默认开关设置
    //int skinning = 0;
    //pass0->setUniform<int>("skinning" ,nbSG::UniformBase::UniformDataType::UDT_INT , &skinning , 1 ,"");

    return ss;
} 