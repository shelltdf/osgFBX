
//#include <windows.h>
//#include <cereal/cereal.hpp>
//#include <nbSG/Public.h>
//#include "../nbViewer/tinymatrix.h"

#include "ImportFunction.h"
#include "loadFBX.h"
#include "loadFBX_util.h"

//#include <osgCore/Utils>
//#include <Stringapiset.h>


using namespace fbxsdk;

//ȫ�ֶ���
FbxManager* lSdkManager = 0;



//extern osg::Node* exportMesh( FbxNode* pNode , FbxMesh* mesh 
//    //,nbSG::SkeletonActorControler* sac, std::map<std::string,std::string>& copy_image_list,bool world_position
//);
//extern void ExportDefaultProgramAndShader(nbSG::ResourceManager* rmg);
//extern void ExportDefaultTextureAndImage( nbSG::ResourceManager* rmg );
//extern nbSG::SkeletonActorControler* LoadSkeletonActorControler( FbxScene* scene );
//extern nbSG::Animation* LoadAnimation( FbxScene* scene , int _frame_begin = -1 , int _frame_count = -1 );
//extern nbSG::ComponentBase* exportCamera( FbxNode* pNode , FbxCamera* mesh );
//extern nbSG::ComponentBase* exportLight( FbxNode* pNode , FbxLight* mesh );
//extern nbSG::ComponentBase* exportLine( FbxNode* pNode , FbxLine* line );


osg::Node* exportAttribute(FbxNode* pNode , FbxNodeAttribute* pAttribute 
    //, nbSG::SkeletonActorControler* sac, std::map<std::string,std::string>& copy_image_list,bool world_position
)
{
    if(!pAttribute) return nullptr;

    //FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
    fbxsdk::FbxNodeAttribute::EType type = pAttribute->GetAttributeType();
    FbxString attrName = pAttribute->GetName();

    //ת������
    std::string name = w2m(m2w(attrName.Buffer(), CP_UTF8));

    switch (type)
    {
    case fbxsdk::FbxNodeAttribute::eUnknown:
        std::cout << "  eUnknown : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eNull:
        std::cout << "  eNull : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eMarker:
        std::cout << "  eMarker : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eSkeleton:
        std::cout << "  eSkeleton : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eMesh:
        std::cout << "  eMesh : " << name << std::endl;
        return exportMesh( pNode , (FbxMesh*) pAttribute /*,sac,copy_image_list , world_position*/);
        break;
    case fbxsdk::FbxNodeAttribute::eNurbs:
        std::cout << "  eNurbs : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::ePatch:
        std::cout << "  ePatch : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eCamera:
        std::cout << "  eCamera : " << attrName.Buffer() << std::endl;
        return exportCamera( pNode , (FbxCamera*) pAttribute);
        break;
    case fbxsdk::FbxNodeAttribute::eCameraStereo:
        std::cout << "  eCameraStereo : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eCameraSwitcher:
        std::cout << "  eCameraSwitcher : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eLight:
        std::cout << "  eLight : " << attrName.Buffer() << std::endl;
        return exportLight( pNode , (FbxLight*) pAttribute);
        break;
    case fbxsdk::FbxNodeAttribute::eOpticalReference:
        std::cout << "  eOpticalReference : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eOpticalMarker:
        std::cout << "  eOpticalMarker : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eNurbsCurve:
        std::cout << "  eNurbsCurve : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eTrimNurbsSurface:
        std::cout << "  eTrimNurbsSurface : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eBoundary:
        std::cout << "  eBoundary : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eNurbsSurface:
        std::cout << "  eNurbsSurface : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eShape:
        std::cout << "  eShape : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eLODGroup:
        std::cout << "  eLODGroup : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eSubDiv:
        std::cout << "  eSubDiv : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eCachedEffect:
        std::cout << "  eCachedEffect : " << attrName.Buffer() << std::endl;
        break;
    case fbxsdk::FbxNodeAttribute::eLine:
        std::cout << "  eLine : " << attrName.Buffer() << std::endl;
        return exportLine( pNode , (FbxLine*) pAttribute);
        break;
    default:
        break;
    }

    return nullptr;
}



//���������FBX�ڵ� ������Ӧ��DOM SceneNode
osg::Node* exportNode(FbxNode* pNode
    //, nbSG::SkeletonActorControler* sac
    //, std::map<std::string, std::string>& copy_image_list
    //, bool collapse_mesh
    //, bool skinning_mode
)
{
    getCustomNodeProperty( pNode );

    const char* nodeName   = pNode->GetName();
    std::string name = w2m(m2w(nodeName, CP_UTF8));//ת������
    std::cout << "Node : " << name << std::endl;

    osg::Node* sn = 0;
    //sn->setName(nodeName);
    //std::cout << "Node : " << nodeName << std::endl;

    //��ȡ��ǰlocal
    osg::Matrix mat = getLocalMatrix( pNode );
    
    //FbxTime keyTimer;
    //keyTimer.SetSecondDouble( 0.0 );
    //FbxAMatrix curveKeyLocalMatrix  = pNode->EvaluateLocalTransform(/*keyTimer*/);
    ////FbxAMatrix curveKeyGlobalMatrix = pNode->EvaluateGlobalTransform(/*keyTimer*/);
    //double* m = curveKeyLocalMatrix;
    //vmml::mat4d mat;
    //mat.set(&m[0],&m[15],false);

    //�������ǰmesh�Ƿ���Ҫִ������
    bool local_collapse_mesh = false;

    //������Ҫ���� (��node��������ص�����)
    //   transform joint
    if( pNode->GetSkeleton() )
    {
        //����joint��Ϊ���
        //nbSG::Joint3D* joint = new nbSG::Joint3D();
        //sn->addComponent(joint);
        //joint->setLocalMatrix(mat);

        osg::MatrixTransform* mt = new osg::MatrixTransform();
        mt->setMatrix(mat);
        mt->setName(nodeName);
        sn = mt;
    }
    else
    {
        ////����transform��Ϊ���
        //nbSG::Transfrom3D* transfrom = new nbSG::Transfrom3D();
        //sn->addComponent(transfrom);

        ////�Ƿ�����mesh�����¼������
        ////  ��ǰ�����skinningģʽ
        ////      �����mesh����skin           ����ѡ������collapse_meshֱ��Ϊfalse
        ////      ���mesh����skin������       ����ѡ������collapse_meshֱ��Ϊture
        ////  ��ǰ�������skinningģʽ
        ////      �����mesh����skin           ��ѡ����collapse_mesh
        ////      ���mesh����skin������       ����ѡ������collapse_meshֱ��Ϊtrue
        //if(skinning_mode)
        //{
        //    if( pNode->GetMesh() && hasSkin( pNode->GetMesh()) )
        //    {
        //        //ʲôҲ��������ԭ��
        //        local_collapse_mesh = true;
        //    }
        //    else
        //    {
        //        transfrom->setLocalMatrix(mat);
        //    }
        //}
        //else
        //{
        //    if( ( pNode->GetMesh()  && !hasSkin( pNode->GetMesh()) && collapse_mesh)
        //        ||( pNode->GetMesh() && hasSkin( pNode->GetMesh()) )
        //        )
        //    {
        //        //ʲôҲ��������ԭ��
        //        local_collapse_mesh = true;
        //    }
        //    else
        //    {
        //        transfrom->setLocalMatrix(mat);
        //    }
        //}

        osg::MatrixTransform* mt = new osg::MatrixTransform();
        mt->setMatrix(mat);
        mt->setName(nodeName);
        sn = mt;
    }


    //������������
    for(int i = 0; i < pNode->GetNodeAttributeCount(); i++)
    {
        osg::Node* component = exportAttribute(pNode, pNode->GetNodeAttributeByIndex(i)
            //, sac, copy_image_list, local_collapse_mesh
        );
        if(component)
        {
            //����ض���һ��group
            sn->asGroup()->addChild(component);
        }
    }

    // Recursively children.
    for (int j = 0; j < pNode->GetChildCount(); j++)
    {
        osg::Node* child = exportNode(pNode->GetChild(j) /*, sac,copy_image_list,collapse_mesh,skinning_mode*/);

        //����ض���һ��group
        sn->asGroup()->addChild(child);
    }

    return sn;
}


bool exportFBXScene(FbxScene* scene, const char* filename)
{
    // Create an exporter.
    FbxExporter* lExporter = FbxExporter::Create(lSdkManager, "");

    // Declare the path and filename of the file to which the scene will be exported.
    // In this case, the file will be in the same directory as the executable.
    //const char* lFilename = "file.fbx";

    int pFileFormat = -1;
    if (pFileFormat < 0 || pFileFormat >= lSdkManager->GetIOPluginRegistry()->GetWriterFormatCount())
    {
        // Write in fall back format if pEmbedMedia is true
        pFileFormat = lSdkManager->GetIOPluginRegistry()->GetNativeWriterFormat();

        //if (!pEmbedMedia)
        {
            //Try to export in ASCII if possible
            int lFormatIndex, lFormatCount = lSdkManager->GetIOPluginRegistry()->GetWriterFormatCount();

            for (lFormatIndex = 0; lFormatIndex<lFormatCount; lFormatIndex++)
            {
                if (lSdkManager->GetIOPluginRegistry()->WriterIsFBX(lFormatIndex))
                {
                    FbxString lDesc = lSdkManager->GetIOPluginRegistry()->GetWriterFormatDescription(lFormatIndex);
                    const char *lASCII = "ascii";
                    if (lDesc.Find(lASCII) >= 0)
                    {
                        pFileFormat = lFormatIndex;
                        break;
                    }
                }
            }
        }
    }


    // Initialize the exporter.
    bool lExportStatus = lExporter->Initialize(filename, pFileFormat, lSdkManager->GetIOSettings());
    if (!lExportStatus) 
    {
        printf("Call to FbxExporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
        return false;
    }

    // Export the scene to the file.
    lExporter->Export(scene);

    // Destroy the exporter.
    lExporter->Destroy();

    return true;
}


FbxScene* importFBXScene( const char* lFilename )
{
    // Change the following filename to a suitable filename value.
    //const char* lFilename = "file.fbx";
    
    // Initialize the SDK manager. This object handles all our memory management.
    /*FbxManager**/ lSdkManager = FbxManager::Create();
    
    // Create the IO settings object.
    FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
    lSdkManager->SetIOSettings(ios);

    // Create an importer using the SDK manager.
    FbxImporter* lImporter = FbxImporter::Create(lSdkManager,"");
    
    // Use the first argument as the filename for the importer.
    if(!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings()))
    { 
        printf("Call to FbxImporter::Initialize() failed.\n"); 
        printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString()); 
        //exit(-1); 
        return nullptr;
    }
    
    // Create a new scene so that it can be populated by the imported file.
    FbxScene* lScene = FbxScene::Create(lSdkManager,"myScene");

    // Import the contents of the file into the scene.
    lImporter->Import(lScene);

    // The file is imported; so get rid of the importer.
    lImporter->Destroy();


    //�������
    //bool b = exportFBXScene(lScene, (std::string(lFilename) + "out.fbx").c_str());


    //ǿ�����ǻ�
    FbxGeometryConverter lConverter(lSdkManager);
    lConverter.Triangulate( lScene , true );

    return lScene;
}



//int main(int argc, char* argv[])
osg::Node* loadFBX(const std::string& filename)
{
    ////�����п���
    //bool collapse_mesh = false;                 //����mesh���� ����ǿ��skin mesh����(���ɿ���)
    //bool exoprt_sikinning_controler = false;    //�������skin����������
    //bool exoprt_animation_controler = false;    //�����������������
    //bool exoprt_animation= false;               //���������Ϣ�ļ�
    //bool only_animation= false;                 //ֻ���������Ϣ�ļ�

    ////Ĭ�������ʽ exe xx.fbx
    //if( argc < 2 ) return 1;
    //std::string fbxfilename = argv[1];

    ////�жϿ���
    //if( argc == 2 ) //ֻ���ļ��� û�п���
    //{
    //    //Ĭ����������ܱ�������ԭʼ��Ϣ
    //    collapse_mesh = false;
    //    exoprt_sikinning_controler = true;
    //    exoprt_animation_controler = true; 
    //    exoprt_animation = true;
    //}
    //else if( argc >= 3 )
    //{
    //    if(std::string(argv[2]) == "-static")
    //    {
    //        //��̬ģ������ ֻ������̬�Ŀ���Ⱦ��Ϣ
    //        collapse_mesh = true;
    //        exoprt_sikinning_controler = false;
    //        exoprt_animation_controler = false;
    //        exoprt_animation = false;
    //    }
    //    if(std::string(argv[2]) == "-skinning")
    //    {
    //        //���skin��ɫ
    //        collapse_mesh = true;  //��ɫ�����ϵĸ���ģ����Ϊû��skin������ ���Կ����������ݲ���
    //        exoprt_sikinning_controler = true;
    //        exoprt_animation_controler = true;
    //        exoprt_animation = false;
    //    }
    //    if(std::string(argv[2]) == "-animation")
    //    {
    //        //ֻ�������
    //        exoprt_animation = true;
    //        only_animation = true;
    //    }
    //}


    //����FBX����
    //FbxScene* fbx_scene = importFBXScene( fbxfilename.c_str() );
    FbxScene* fbx_scene = importFBXScene(filename.c_str());
    if( !fbx_scene ) return 0;


    /*
        }
    Object: 1508289913968, "ADSKAssetReferencesVersion3.0", "" {
        Properties70:  {
            P: "ADSKAssetReferencesBlobVersion3.0", "Blob", "", "",135577 {
                BinaryData: "UEsDBBQABggIAE5k8kgj7Qmp02gAACjEAQBPAAAAMUQ5MjQ0QzQ
                */

    //����ADSKAssetReferencesVersion����
    FbxObject* ADSKAssetReferencesVersion = 0;
    int membercount = fbx_scene->GetMemberCount();
    for (size_t i = 0; i < membercount; i++)
    {
        FbxObject* obj = fbx_scene->GetMember(i);

        std::string name = w2m(m2w(obj->GetName(), CP_UTF8));//ת������
        //printf("Object: %s\n", (char*)name.c_str());

        if (name.find("ADSKAssetReferencesVersion") != std::string::npos)
        {
            ADSKAssetReferencesVersion = obj;
        }
    }

    //����ADSKAssetReferencesVersion����
    if (ADSKAssetReferencesVersion)
    {

    }


    //getPose( fbx_scene );

    //��ʼ����Դ��Դ������
    //nbBase::niubi_entry_arg arg;
    //nbBase::ref_ptr< nbSG::ResourceManager > rmg;
    //rmg = new nbSG::ResourceManager(arg,true);

    //ע��д���·��
    //std::string export_dirname = fbxfilename.substr(0,fbxfilename.size()-4);
    //rmg->registerArchive("", export_dirname , true );

    //����Ĭ����Դ
    //ExportDefaultProgramAndShader( rmg.get() );
    //ExportDefaultTextureAndImage( rmg.get() );
    //rmg->updateUniformAssetPath(); //����asset��


    //����������Ϣ����ǰĬ�ϵ�stack��
    //nbBase::ref_ptr<nbSG::Animation> anim = LoadAnimation(fbx_scene);

    ////���ֻ���������  �����г��� ֱ��д�붯�������� �Ա�ﵽ�滻������Ŀ��
    //if(only_animation)
    //{
    //    //ֱ��д��animation
    //    rmg->writeResource( anim.get() );

    //    //�����г���
    //    nbSG::ResourceGroup rg;
    //    nbSG::Resource* res = rmg->readResourceGroup(nbBase::Path("|scene.scene"),&rg,false);
    //    if(res)
    //    {
    //        nbSG::Scene* nb_scene = dynamic_cast<nbSG::Scene*>(res);
    //        if(nb_scene)
    //        {
    //            printf("load scene : %s\n" , nb_scene->getUniformPath().getPathMel().c_str() );

    //            nbSG::SkeletonActorControler* sac = nb_scene->getRoot()->getComponent<nbSG::SkeletonActorControler>();
    //            if(sac) sac->setAnimation(anim.get());

    //            nbSG::ActorControler* ac = nb_scene->getRoot()->getComponent<nbSG::SkeletonActorControler>();
    //            if(sac) sac->setAnimation(anim.get());
    //        }
    //        rmg->writeResource( nb_scene );
    //    }

    //    // Destroy the SDK manager and all the other objects it was handling.
    //    lSdkManager->Destroy();
    //    printf("done animation\n");
    //    return 0;
    //}


    ////����DOM scene
    //nbSG::Scene* scene = new nbSG::Scene();
    //scene->setUniformPath("|scene");

    ////�������ܴ��ڵĹ�������������
    //nbSG::SkeletonActorControler* sac = LoadSkeletonActorControler( fbx_scene );
    //if(sac && exoprt_sikinning_controler)
    //{
    //    if(anim && exoprt_animation) sac->setAnimation(anim.get());
    //    scene->getRoot()->addComponent( sac );
    //}
    //else
    //{
    //    //������ڶ�����Ϣ �ͽ����㼶����������
    //     if(anim && exoprt_animation_controler)
    //     {
    //         nbSG::ActorControler* ac = new nbSG::ActorControler();
    //         if(anim && exoprt_animation) ac->setAnimation( anim.get() );
    //         scene->getRoot()->addComponent( ac );
    //     }
    //}


    //��Ҫ���Ƶ��ⲿ���� ��Ҫ��image
    //from ���ؾ���·��
    //to   uniform·��
    std::map<std::string/*from*/,std::string/*to*/> copy_image_list;

    //����ֵ���ڵ�
    osg::Node* root = 0;

    //�Ӹ��ڵ㿪ʼ�ݹ����
    printf("export root node\n");
    FbxNode* lRootNode = fbx_scene->GetRootNode();
    if(lRootNode)
    {
        root = exportNode(lRootNode/*, sac, copy_image_list, collapse_mesh, exoprt_sikinning_controler*/);

        //for(int i = 0; i < lRootNode->GetChildCount(); i++)
        //{
        //    nbSG::SceneNode* child = exportNode(lRootNode->GetChild(i), sac, copy_image_list, collapse_mesh, exoprt_sikinning_controler);
        //    scene->getRoot()->addChild(child);
        //}
    }
    printf("export root node done\n");


    //osg::StateSet* ss = root->getOrCreateStateSet();
    ////ss->setMode(GL_DEPTH_CLAMP, osg::StateAttribute::ON);
    //ss->setMode(GL_NORMALIZE, osg::StateAttribute::ON);


    //�����ⲿ�ļ�
    //printf("copy image\n");
    //for (auto it = copy_image_list.begin(); it != copy_image_list.end(); it++)
    //{
    //    printf("copy \"%s\" \"%s\"\n" ,it->first.c_str() , it->second.c_str() );

    //    std::vector<char> f = readfile(it->first);

    //    //char* aaaa = "qqqq";
    //    rmg->writeFile(nbBase::Path(it->second),&f[0],f.size());
    //}


    //ֱ��д��DOM scene
    //rmg->writeResource(scene);

    //�ռ�ȫ����Դд��
    //std::map< std::string , nbBase::ref_ptr< nbSG::Resource > > result;
    //scene->collectionResourceObject( result );
    //nbSG::ResourceGroup rg( scene );
    //rg.searchAll();

    //
    //printf("write resource group\n");
    //rmg->writeResourceGroup(&rg);


    // Destroy the SDK manager and all the other objects it was handling.
    lSdkManager->Destroy();
    lSdkManager = 0;

    printf("done\n");
    return root;
}
