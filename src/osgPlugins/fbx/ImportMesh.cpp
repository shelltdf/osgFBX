
#include "ImportFunction.h"

//#include <iostream>
//#include <list>

//#include <fbxsdk.h>
//
#include "loadFBX_util.h"

//#include <nbSG/Public.h>

using namespace fbxsdk;


//extern osg::StateSet* LoadMaterialAttribute( FbxSurfaceMaterial* pSurfaceMaterial /*, std::map<std::string,std::string>& copy_image_list*/);
//extern vmml::mat4f getLocalMatrix( FbxNode* node );
//extern vmml::mat4f getWorldMatrix( FbxNode* node );

#if 0
//记录一个控制点的权重信息
struct ControlPointWeight
{
    ControlPointWeight():index(0),weight(0){}

    std::vector<int>   index;
    std::vector<float> weight;
};

void AssociateSkeletonWithCtrlPoint( FbxSkin* pSkin , nbSG::SkeletonActorControler* sac ,std::vector<ControlPointWeight>& controlpointweights)
{  
    FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize;
    FbxCluster* pCluster = 0;
    FbxNode*    pLinkNode = 0;
    int         skeletonIndex = 0;
    //CSkeleton*   pSkeleton;
    FbxAMatrix  transformMatrix , transformLinkMatrix;
    int         clusterCount = pSkin->GetClusterCount();

    // 处理当前Skin中的每个Cluster（对应到Skeleton）  
    for(int i = 0 ; i < clusterCount ; ++i)
    {
        //获取簇
        pCluster = pSkin->GetCluster(i);
        if(!pCluster){continue;}

        //获取簇对应的node
        pLinkNode = pCluster->GetLink();
        if(!pLinkNode){ continue;}

        //当前node骨骼索引
        int skeletonIndex = nbSG::SkeletonActorControler::getSkeletonIndex(pLinkNode->GetName(),sac->mSkeletonMapping);

        // 得到每个Cluster（Skeleton）所对应的Transform和TransformLink矩阵，后面具体说明  
        pCluster->GetTransformMatrix(transformMatrix);
        pCluster->GetTransformLinkMatrix(transformLinkMatrix);

        // 其它适宜的操作，将Transform、TransformLink转换为映射矩阵并存储到相应的Skeleton中  
        // ...  

        int     associatedCtrlPointCount = pCluster->GetControlPointIndicesCount();
        int*    pCtrlPointIndices = pCluster->GetControlPointIndices();
        double* pCtrlPointWeights = pCluster->GetControlPointWeights();
        int     ctrlPointIndex = 0;
        double  ctrlPointWeight = 0;

        // 遍历当前Cluster所影响到的每个Vertex，并将对相应的信息做记录以便Skinning时使用  
        for(int j = 0 ; j < associatedCtrlPointCount ; ++j)
        {
            ctrlPointIndex  = pCtrlPointIndices[j];  //顶点index
            ctrlPointWeight = pCtrlPointWeights[j];  //对应当前Node的权重
            //ctrlPointSkeletonList[ctrlPointIndex].AddSkeleton(skeletonIndex , pCtrlPointWeights[j]);  

            //得到控制点索引
            //printf("joint:%s,id:%d,vertex_index=%d,weight=%f\n" , pLinkNode->GetName(),skeletonIndex , ctrlPointIndex, ctrlPointWeight);
            controlpointweights[ctrlPointIndex].index.push_back(skeletonIndex);
            controlpointweights[ctrlPointIndex].weight.push_back((float)ctrlPointWeight);
        }
    }  

}

//获取顶点index 对应的骨骼和权重信息
void AssociateSkeletonWithCtrlPoint(FbxMesh* pMesh , nbSG::SkeletonActorControler* sac ,std::vector<ControlPointWeight>& controlpointweights)  
{  
    int ctrlPointCount = pMesh->GetControlPointsCount();
    int deformerCount  = pMesh->GetDeformerCount();

    //初始化输出参数
    //ctrlPointSkeletonList.SetCapacity(ctrlPointCount);
    //ctrlPointSkeletonList.setListSize(ctrlPointCount);
    //controlpointweights.resize(ctrlPointCount);

    FbxDeformer* pFBXDeformer;
    FbxSkin*     pFBXSkin;

    for(int i = 0 ; i < deformerCount ; ++i)
    {
        //获取变形器
        pFBXDeformer = pMesh->GetDeformer(i);
        if(pFBXDeformer == NULL){continue;}

        //获取skin类型
        if(pFBXDeformer->GetDeformerType() != FbxDeformer::eSkin){continue;}

        //获取skin对象
        pFBXSkin = (FbxSkin*)(pFBXDeformer);
        if(pFBXSkin == NULL){continue;}

        //初始化输出参数
        controlpointweights.resize(ctrlPointCount);

        //处理skin
        AssociateSkeletonWithCtrlPoint(pFBXSkin,sac,controlpointweights);
    }
}

#endif

//获取对应三角面的材质索引
void ConnectMaterialToMesh(FbxMesh* pMesh , int triangleCount , int* pTriangleMtlIndex)  
{
    // Get the material index list of current mesh  
    FbxLayerElementArrayTemplate<int>* pMaterialIndices;  
    FbxGeometryElement::EMappingMode   materialMappingMode = FbxGeometryElement::eNone;  

    if(pMesh->GetElementMaterial())  
    {  
        pMaterialIndices = &pMesh->GetElementMaterial()->GetIndexArray();  
        materialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();  
        if(pMaterialIndices)  
        {  
            switch(materialMappingMode)  
            {  
            case FbxGeometryElement::eByPolygon:  
                {  
                    if(pMaterialIndices->GetCount() == triangleCount)  
                    {  
                        for(int triangleIndex = 0 ; triangleIndex < triangleCount ; ++triangleIndex)  
                        {  
                            int materialIndex = pMaterialIndices->GetAt(triangleIndex);  

                            pTriangleMtlIndex[triangleIndex] = materialIndex;  
                        }  
                    }  
                }  
                break;  

            case FbxGeometryElement::eAllSame:  
                {  
                    int lMaterialIndex = pMaterialIndices->GetAt(0);  

                    for(int triangleIndex = 0 ; triangleIndex < triangleCount ; ++triangleIndex)  
                    {  
                        int materialIndex = pMaterialIndices->GetAt(triangleIndex);  

                        pTriangleMtlIndex[triangleIndex] = materialIndex;  
                    }  
                }  
            }  
        }  
    }  
}  

//读顶点数据
void ReadVertex( FbxMesh* pMesh , int ctrlPointIndex , float* pVertex )  
{
    FbxVector4* pCtrlPoint = pMesh->GetControlPoints();

    pVertex[0] = (float)pCtrlPoint[ctrlPointIndex][0];
    pVertex[1] = (float)pCtrlPoint[ctrlPointIndex][1];
    pVertex[2] = (float)pCtrlPoint[ctrlPointIndex][2];
    //pVertex[3] = (float)pCtrlPoint[ctrlPointIndex][3];
}

//读顶点色彩
void ReadColor( FbxMesh* pMesh , int uvLayer , int ctrlPointIndex , int vertexCounter , float* pColor)  
{  
    //if(pMesh->GetElementVertexColorCount() < 1)  
    //{  
    //    return;  
    //}  

    FbxGeometryElementVertexColor* pVertexColor = pMesh->GetElementVertexColor(0);  
    switch(pVertexColor->GetMappingMode())  
    {  
    case FbxGeometryElement::eByControlPoint:  
        {  
            switch(pVertexColor->GetReferenceMode())  
            {  
            case FbxGeometryElement::eDirect:  
                {  
                    pColor[0] = (float)pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mRed;  
                    pColor[1] = (float)pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mGreen;  
                    pColor[2] = (float)pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mBlue;  
                    pColor[3] = (float)pVertexColor->GetDirectArray().GetAt(ctrlPointIndex).mAlpha;  
                }  
                break;  

            case FbxGeometryElement::eIndexToDirect:  
                {  
                    int id = pVertexColor->GetIndexArray().GetAt(ctrlPointIndex);  
                    pColor[0] = (float)pVertexColor->GetDirectArray().GetAt(id).mRed;  
                    pColor[1] = (float)pVertexColor->GetDirectArray().GetAt(id).mGreen;  
                    pColor[2] = (float)pVertexColor->GetDirectArray().GetAt(id).mBlue;  
                    pColor[3] = (float)pVertexColor->GetDirectArray().GetAt(id).mAlpha;  
                }  
                break;  

            default:  
                break;  
            }  
        }  
        break;  

    case FbxGeometryElement::eByPolygonVertex:  
        {  
            switch (pVertexColor->GetReferenceMode())  
            {  
            case FbxGeometryElement::eDirect:  
                {  
                    pColor[0] = (float)pVertexColor->GetDirectArray().GetAt(vertexCounter).mRed;  
                    pColor[1] = (float)pVertexColor->GetDirectArray().GetAt(vertexCounter).mGreen;  
                    pColor[2] = (float)pVertexColor->GetDirectArray().GetAt(vertexCounter).mBlue;  
                    pColor[3] = (float)pVertexColor->GetDirectArray().GetAt(vertexCounter).mAlpha;  
                }  
                break;  
            case FbxGeometryElement::eIndexToDirect:  
                {  
                    int id = pVertexColor->GetIndexArray().GetAt(vertexCounter);  
                    pColor[0] = (float)pVertexColor->GetDirectArray().GetAt(id).mRed;  
                    pColor[1] = (float)pVertexColor->GetDirectArray().GetAt(id).mGreen;  
                    pColor[2] = (float)pVertexColor->GetDirectArray().GetAt(id).mBlue;  
                    pColor[3] = (float)pVertexColor->GetDirectArray().GetAt(id).mAlpha;  
                }  
                break;  
            default:  
                break;  
            }  
        }  
        break;  
    }  
}  

//读UV数据
void ReadUV(FbxMesh* pMesh , int uvLayer , int ctrlPointIndex , int textureUVIndex  , float* pUV)  
{  
    //if(uvLayer >= 2 || pMesh->GetElementUVCount() <= uvLayer)  
    //{  
    //    return;
    //}

    FbxGeometryElementUV* pVertexUV = pMesh->GetElementUV(uvLayer);  

    switch(pVertexUV->GetMappingMode())  
    {  
    case FbxGeometryElement::eByControlPoint:  
        {  
            switch(pVertexUV->GetReferenceMode())  
            {  
            case FbxGeometryElement::eDirect:  
                {  
                    pUV[0] = (float)pVertexUV->GetDirectArray().GetAt(ctrlPointIndex)[0];  
                    pUV[1] = (float)pVertexUV->GetDirectArray().GetAt(ctrlPointIndex)[1];  
                }  
                break;  

            case FbxGeometryElement::eIndexToDirect:  
                {  
                    int id = pVertexUV->GetIndexArray().GetAt(ctrlPointIndex);  
                    pUV[0] = (float)pVertexUV->GetDirectArray().GetAt(id)[0];  
                    pUV[1] = (float)pVertexUV->GetDirectArray().GetAt(id)[1];  
                }  
                break;  

            default:  
                break;  
            }  
        }  
        break;  

    case FbxGeometryElement::eByPolygonVertex:  
        {  
            switch (pVertexUV->GetReferenceMode())  
            {  
            case FbxGeometryElement::eDirect:  
            case FbxGeometryElement::eIndexToDirect:  
                {  
                    pUV[0] = (float)pVertexUV->GetDirectArray().GetAt(textureUVIndex)[0];  
                    pUV[1] = (float)pVertexUV->GetDirectArray().GetAt(textureUVIndex)[1];  
                }  
                break;  

            default:  
                break;  
            }  
        }  
        break;  
    }  
}  

//读法线数据
void ReadNormal(FbxMesh* pMesh , int ctrlPointIndex , int vertexCounter , float* pNormal)  
{  
    //if(pMesh->GetElementNormalCount() < 1)  
    //{
    //    return;  
    //}

    FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(0);  
    switch(leNormal->GetMappingMode())  
    {  
    case FbxGeometryElement::eByControlPoint:  
        {  
            switch(leNormal->GetReferenceMode())  
            {  
            case FbxGeometryElement::eDirect:  
                {  
                    pNormal[0] = (float)leNormal->GetDirectArray().GetAt(ctrlPointIndex)[0];  
                    pNormal[1] = (float)leNormal->GetDirectArray().GetAt(ctrlPointIndex)[1];  
                    pNormal[2] = (float)leNormal->GetDirectArray().GetAt(ctrlPointIndex)[2];  
                }  
                break;  

            case FbxGeometryElement::eIndexToDirect:  
                {  
                    int id = leNormal->GetIndexArray().GetAt(ctrlPointIndex);  
                    pNormal[0] = (float)leNormal->GetDirectArray().GetAt(id)[0];  
                    pNormal[1] = (float)leNormal->GetDirectArray().GetAt(id)[1];  
                    pNormal[3] = (float)leNormal->GetDirectArray().GetAt(id)[2];  
                }  
                break;  

            default:
                break;  
            }  
        }  
        break;  

    case FbxGeometryElement::eByPolygonVertex:  
        {  
            switch(leNormal->GetReferenceMode())  
            {  
            case FbxGeometryElement::eDirect:  
                {  
                    pNormal[0] = (float)leNormal->GetDirectArray().GetAt(vertexCounter)[0];  
                    pNormal[1] = (float)leNormal->GetDirectArray().GetAt(vertexCounter)[1];  
                    pNormal[2] = (float)leNormal->GetDirectArray().GetAt(vertexCounter)[2];  
                }  
                break;  

            case FbxGeometryElement::eIndexToDirect:  
                {  
                    int id = leNormal->GetIndexArray().GetAt(vertexCounter);  
                    pNormal[0] = (float)leNormal->GetDirectArray().GetAt(id)[0];  
                    pNormal[1] = (float)leNormal->GetDirectArray().GetAt(id)[1];  
                    pNormal[2] = (float)leNormal->GetDirectArray().GetAt(id)[2];  
                }  
                break;  

            default:  
                break;  
            }  
        }  
        break;  
    }  
}  

//读切线数据
void ReadTangent(FbxMesh* pMesh , int uvLayer , int ctrlPointIndex , int vertecCounter , float* pTangent)  
{  
    //if(pMesh->GetElementTangentCount() < 1)  
    //{  
    //    return;  
    //}  

    FbxGeometryElementTangent* leTangent = pMesh->GetElementTangent(0);  

    switch(leTangent->GetMappingMode())  
    {  
    case FbxGeometryElement::eByControlPoint:  
        {  
            switch(leTangent->GetReferenceMode())  
            {  
            case FbxGeometryElement::eDirect:  
                {  
                    pTangent[0] = (float)leTangent->GetDirectArray().GetAt(ctrlPointIndex)[0];  
                    pTangent[1] = (float)leTangent->GetDirectArray().GetAt(ctrlPointIndex)[1];  
                    pTangent[2] = (float)leTangent->GetDirectArray().GetAt(ctrlPointIndex)[2];  
                }  
                break;

            case FbxGeometryElement::eIndexToDirect:  
                {
                    int id = leTangent->GetIndexArray().GetAt(ctrlPointIndex);  
                    pTangent[0] = (float)leTangent->GetDirectArray().GetAt(id)[0];  
                    pTangent[1] = (float)leTangent->GetDirectArray().GetAt(id)[1];  
                    pTangent[2] = (float)leTangent->GetDirectArray().GetAt(id)[2];  
                }
                break;

            default:
                break;  
            }  
        }  
        break;  

    case FbxGeometryElement::eByPolygonVertex:
        {  
            switch(leTangent->GetReferenceMode())  
            {  
            case FbxGeometryElement::eDirect:
                {  
                    pTangent[0] = (float)leTangent->GetDirectArray().GetAt(vertecCounter)[0];  
                    pTangent[1] = (float)leTangent->GetDirectArray().GetAt(vertecCounter)[1];  
                    pTangent[2] = (float)leTangent->GetDirectArray().GetAt(vertecCounter)[2];  
                }  
                break;  

            case FbxGeometryElement::eIndexToDirect:
                {  
                    int id = leTangent->GetIndexArray().GetAt(vertecCounter);  
                    pTangent[0] = (float)leTangent->GetDirectArray().GetAt(id)[0];  
                    pTangent[1] = (float)leTangent->GetDirectArray().GetAt(id)[1];  
                    pTangent[2] = (float)leTangent->GetDirectArray().GetAt(id)[2];  
                }  
                break;  

            default:  
                break;
            }
        }  
        break;
    }  
}  

//void DisplayMetaDataConnections(FbxObject* pObject)
//{
//    int nbMetaData = pObject->GetSrcObjectCount<FbxObjectMetaData>();
//    if (nbMetaData > 0)
//    {
//        printf("    MetaData connections %d", nbMetaData);
//    }
//    for (int i = 0; i < nbMetaData; i++)
//    {
//        FbxObjectMetaData* metaData = pObject->GetSrcObject<FbxObjectMetaData>(i);
//        std::string name = osgCore::w2m(osgCore::m2w(metaData->GetName(), CP_UTF8));//转换编码
//        printf("        Name: %s\n", (char*)name.c_str());
//    }
//}
void DisplayMetaDataConnections(FbxObject* pObject)
{
    int nbMetaData = pObject->GetSrcObjectCount<FbxObject>();
    if (nbMetaData > 0)
    {
        printf("    MetaData connections %d\n", nbMetaData);
    }
    for (int i = 0; i < nbMetaData; i++)
    {
        FbxObject* metaData = pObject->GetSrcObject<FbxObject>(i);
        std::string name = w2m(m2w(metaData->GetName(), CP_UTF8));//转换编码
        printf("        Name: %s\n", (char*)name.c_str());

        FbxProperty lProperty = metaData->GetFirstProperty();
        //int i = 0;
        while (lProperty.IsValid())
        {
            name = w2m(m2w(lProperty.GetName().Buffer(), CP_UTF8));//转换编码
            printf("            Property: %s\n", (char*)name.c_str());

            lProperty = metaData->GetNextProperty(lProperty);
        }
    }
}
/*
*   输出mesh相关信息
*       geometry material
*
*   geometry输出内容 输出RAW方式三角面
*       vec4 顶点位置 (FBX貌似只支持vec4)
*       vec3 法线     (如果有)
*       vec3 切线     (如果有)
*       vec2 UV信息   (如果有)
*       vec4 顶点色彩 (如果有)
*
*   world_position 是否塌陷所有数据到世界坐标
*/
osg::Geode* exportMesh( FbxNode* pNode , FbxMesh* mesh 
    //,nbSG::SkeletonActorControler* sac , std::map<std::string,std::string>& copy_image_list ,bool world_position 
)
{
    //DisplayMetaDataConnections(pNode);

    //因为强制三角化了 所以这里所有polygon都是三角面
    int triangleCount = mesh->GetPolygonCount();
    if( triangleCount <= 0 ) return nullptr;

    //查询材质索引表(读出的索引值并没有任何顺序)
    int material_count1 = pNode->GetMaterialCount(); //GetMaterialCount并不是完全可靠的接口
    int material_count2 = mesh->GetElementTangentCount();
    std::vector< int > material_index(triangleCount);//这里默认初始化索引全部为0 如果材质不存在就是全0
    ConnectMaterialToMesh( mesh , triangleCount , &material_index[0] );  
    
    //新的统计方式
    //   可能是存在subMesh但是这些对象没有材质造成的
    int material_count = 0;
    for (size_t i = 0; i < material_index.size(); i++)
    {
        if (material_index[i] >= material_count) material_count = material_index[i]+1;
    }

    //处理权重信息 获取控制点对应的索引和权重表
    //std::vector<ControlPointWeight> controlpointweights;
    //AssociateSkeletonWithCtrlPoint( mesh , sac ,controlpointweights ); 

    //处理权重信息 只取4个权重 并且归一化权重和
    {
    }

    //获取当前mesh的世界矩阵
    //vmml::mat4f current_mesh_matrix = getWorldMatrix( pNode );
    ////FbxAMatrix current_mesh_matrix; //默认单位矩阵
    //if(world_position)
    //{
    //    //FbxTime keyTimer;
    //    //keyTimer.SetSecondDouble( 0.0 );

    //    ////评估矩阵
    //    //FbxAMatrix curveKeyLocalMatrix  = pNode->EvaluateLocalTransform(/*keyTimer*/);
    //    //FbxAMatrix curveKeyGlobalMatrix = pNode->EvaluateGlobalTransform(/*keyTimer*/);

    //    //current_mesh_matrix = curveKeyGlobalMatrix;
    //}

    //查询当前数据通道是否存在
    bool has_vertex     = true;     //默认一定存在
    bool has_color[4]   = {false};  //0-3个色彩通道是否存在
    bool has_uv[4]      = {false};  //0-3个uv通道是否存在
    bool has_normal     = false;    //位移的法线通道是否存在
    bool has_tangent[4] = {false};  //对应不同的uv通道 应该存在对应的切线通道
    //bool has_weight     = controlpointweights.size()>0;    //权重信息
    //bool has_boneindex  = controlpointweights.size()>0;    //权重对应骨骼
    has_normal = mesh->GetElementNormalCount() > 0;  //实际上法线信息没有超过1一个通道的需求
    for(int k = 0 ; k < mesh->GetElementVertexColorCount()  && k<4; ++k) { has_color[k]   = true; }
    for(int k = 0 ; k < mesh->GetElementUVCount()           && k<4; ++k) { has_uv[k]      = true; }
    for(int k = 0 ; k < mesh->GetElementTangentCount()      && k<4; ++k) { has_tangent[k] = true; }

    //建立mesh相关
    //nbSG::MeshRender* meshrender = new nbSG::MeshRender();
    //nbBase::ref_ptr<nbSG::Geometry> geometry = new nbSG::Geometry();
    //meshrender->setGeometry( geometry.get() );
    osg::Geode* meshrender = new osg::Geode();
    meshrender->setName(pNode->GetName());

    //计算geometry的名称 
    //      因为需要geometry不允许名称重复 
    //      所以简单方法就是使用fbx的UID 
    //      FBX的UID不会因为读取时间和次数而改变 是相对于文件本身固定的
    //std::ostringstream oss;
    //oss << std::hex << std::setfill ('0') << std::setw( sizeof(FbxUInt64)*2 ) << mesh->GetUniqueID();
    //geometry->setUniformPath( oss.str() );


    //建立相关的material信息
    std::vector<osg::ref_ptr< osg::StateSet> > material_list;
    for(int materialIndex = 0 ; materialIndex < material_count ; materialIndex++)  
    {  
        FbxSurfaceMaterial* pSurfaceMaterial = pNode->GetMaterial(materialIndex);  
        if (pSurfaceMaterial)//有可能索引超过了当前数据实际存在的材质 见 新的统计方式
        {
            osg::StateSet* mat = LoadMaterialAttribute(pSurfaceMaterial /*,copy_image_list*/);
            //LoadMaterialTexture  ( pSurfaceMaterial );
            if (mat)
            {
                //meshrender->addMaterial(mat);
                material_list.push_back(mat);
            }
        }
    }


    //根据通道存在规则建立buffer容器
    //geometry->mBuffers[NFX_VectexAttribute_position] = new nbSG::ArrayT<float>(triangleCount*3*3);
    //if(has_normal){ geometry->mBuffers[NFX_VectexAttribute_normal] = new nbSG::ArrayT<float>(triangleCount*3*3); }
    //if(has_color[0]){ geometry->mBuffers[NFX_VectexAttribute_color0] = new nbSG::ArrayT<float>(triangleCount*3*4); }
    //if(has_color[1]){ geometry->mBuffers[NFX_VectexAttribute_color1] = new nbSG::ArrayT<float>(triangleCount*3*4); }
    //if(has_color[2]){ geometry->mBuffers[NFX_VectexAttribute_color2] = new nbSG::ArrayT<float>(triangleCount*3*4); }
    //if(has_color[3]){ geometry->mBuffers[NFX_VectexAttribute_color3] = new nbSG::ArrayT<float>(triangleCount*3*4); }
    //if(has_uv[0]){ geometry->mBuffers[NFX_VectexAttribute_uv0] = new nbSG::ArrayT<float>(triangleCount*3*2); }
    //if(has_uv[1]){ geometry->mBuffers[NFX_VectexAttribute_uv1] = new nbSG::ArrayT<float>(triangleCount*3*2); }
    //if(has_uv[2]){ geometry->mBuffers[NFX_VectexAttribute_uv2] = new nbSG::ArrayT<float>(triangleCount*3*2); }
    //if(has_uv[3]){ geometry->mBuffers[NFX_VectexAttribute_uv3] = new nbSG::ArrayT<float>(triangleCount*3*2); }
    //if(has_tangent[0]){ geometry->mBuffers[NFX_VectexAttribute_tangent0] = new nbSG::ArrayT<float>(triangleCount*3*3); }
    //if(has_tangent[1]){ geometry->mBuffers[NFX_VectexAttribute_tangent1] = new nbSG::ArrayT<float>(triangleCount*3*3); }
    //if(has_tangent[2]){ geometry->mBuffers[NFX_VectexAttribute_tangent2] = new nbSG::ArrayT<float>(triangleCount*3*3); }
    //if(has_tangent[3]){ geometry->mBuffers[NFX_VectexAttribute_tangent3] = new nbSG::ArrayT<float>(triangleCount*3*3); }
    //if(has_boneindex){ geometry->mBuffers[NFX_VectexAttribute_bone_indices]  = new nbSG::ArrayT<int>(triangleCount*3*4); }
    //if(has_weight)   { geometry->mBuffers[NFX_VectexAttribute_blend_weights] = new nbSG::ArrayT<float>(triangleCount*3*4); }

    osg::Vec3Array* vectex_list(new osg::Vec3Array(triangleCount * 3));
    osg::Vec3Array* normal_list = 0;
    osg::Vec4Array* color0_list = 0;
    osg::Vec2Array* uv0_list = 0;
    osg::Vec2Array* uv1_list = 0;
    osg::Vec2Array* uv2_list = 0;
    osg::Vec2Array* uv3_list = 0;
    osg::Vec3Array* tangent0_list = 0;

    if (has_normal) normal_list = new osg::Vec3Array(triangleCount * 3);
    if (has_color[0]) color0_list = new osg::Vec4Array(triangleCount * 3);
    if (has_uv[0]) uv0_list = new osg::Vec2Array(triangleCount * 3);
    if (has_uv[1]) uv1_list = new osg::Vec2Array(triangleCount * 3);
    if (has_uv[2]) uv2_list = new osg::Vec2Array(triangleCount * 3);
    if (has_uv[3]) uv3_list = new osg::Vec2Array(triangleCount * 3);
    if (has_tangent[0]) tangent0_list = new osg::Vec3Array(triangleCount * 3);



    //每次存储三个定点的数据
    //类型          多通道  三角面  float分量
    float vertex            [3]     [3]       ={0};
    float color     [4]     [3]     [4]       ={0};
    float uv        [4]     [3]     [2]       ={0};
    float normal            [3]     [3]       ={0};
    float tangent   [4]     [3]     [3]       ={0};
    int   boneindex         [3]     [4]       ={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    float weight            [3]     [4]       ={0};

    //如果不存在任何材质信息 就假设有一个为0的材质id 
    //虽然不能弥补材质的缺失 但是保证mesh被正常输出
    //因为max的物体创建时是没有材质的  maya创建时一定有默认材质 但是不排除操作错误
    if(material_count == 0)
    {
        material_count = 1;
    }

    //记录材质索引的数量 每个动态记录自己的出现的次数
    std::vector<int> material_histogram(material_count);
    for (int i = 0; i < material_count; i++)
    {
        material_histogram[i] = 0;
    }

    //根据材质id的顺序遍历
    unsigned int vertexCounter = 0;
    for(int mtl_index = 0 ; mtl_index < material_count ; ++mtl_index)
    {
        //循环所有三角面
        for(int tri_index = 0 ; tri_index < triangleCount ; ++tri_index)
        {
            //如果材质id不等于当前id 就执行下一次
            if( mtl_index != material_index[tri_index] ) continue;

            //记录直方图
            material_histogram[mtl_index] ++;
            

            //循环当前三角的三个点
            for(int point_index = 0 ; point_index < 3 ; point_index++)
            {
                //计算当前点
                int vertexCounter = tri_index * 3 + point_index;

                //当前三角面的三个顶点索引
                int ctrlPointIndex = mesh->GetPolygonVertex( tri_index , point_index );

                // Read the vertex
                ReadVertex( mesh , ctrlPointIndex , &vertex[point_index][0] );

                if( mesh->GetElementNormalCount() > 0 )  
                {
                    // Read the normal of each vertex
                    ReadNormal(mesh , ctrlPointIndex , vertexCounter , &normal[point_index][0]);
                }

                // Read the color of each vertex
                for(int channal_index = 0 ; channal_index < mesh->GetElementVertexColorCount() && channal_index<4; ++channal_index)
                {
                    ReadColor(mesh ,channal_index , ctrlPointIndex , vertexCounter , &color[channal_index][point_index][0]);
                }

                // Read the UV of each vertex
                for(int channal_index = 0 ; channal_index < mesh->GetElementUVCount() && channal_index<4; ++channal_index)
                {
                    ReadUV(mesh , channal_index , ctrlPointIndex , mesh->GetTextureUVIndex(tri_index, point_index)  , &(uv[channal_index][point_index][0]));
                }

                // Read the tangent of each vertex
                for(int channal_index = 0 ; channal_index < mesh->GetElementTangentCount() && channal_index<4; ++channal_index)
                {
                    ReadTangent(mesh , channal_index , ctrlPointIndex , vertexCounter , &tangent[channal_index][point_index][0]);
                }

                //if( has_boneindex 
                //    && has_weight 
                //    && ( ctrlPointIndex < (int)controlpointweights.size() )
                //    && ( ctrlPointIndex >= 0 )
                //    )
                //{
                //    ControlPointWeight cpw = controlpointweights[ctrlPointIndex];
                //    unsigned int index_size  = cpw.index.size();
                //    unsigned int weight_size = cpw.weight.size();

                //    boneindex[point_index][0] = index_size>0 ? cpw.index[0] : -1;
                //    boneindex[point_index][1] = index_size>1 ? cpw.index[1] : -1;
                //    boneindex[point_index][2] = index_size>2 ? cpw.index[2] : -1;
                //    boneindex[point_index][3] = index_size>3 ? cpw.index[3] : -1;

                //    weight[point_index][0] = weight_size>0 ? cpw.weight[0] : 0.0f;
                //    weight[point_index][1] = weight_size>1 ? cpw.weight[1] : 0.0f;
                //    weight[point_index][2] = weight_size>2 ? cpw.weight[2] : 0.0f;
                //    weight[point_index][3] = weight_size>3 ? cpw.weight[3] : 0.0f;
                //}

                //vertexCounter++;
            }//for(int point_index = 0 ; point_index < 3 ; point_index++)

            //成功获取一个三角面信息 追加给geometry的buffer
            //这些三角面通过这里的顺序是材质id增加的顺序 绝对保证不会混乱
            //printf("t%d",mtl_index);

            //如果有塌陷的需求 就转换vertex normal tangent
            //if(world_position 
            //    || controlpointweights.size()>0  //如果没有权重就不塌陷 因为这些对象只是层级关系
            //    )
            //{
            //    for (int w = 0; w < 3; w++) //三个顶点
            //    {
            //        //FbxVector4 v = FbxVector4(vertex[w][0],vertex[w][1],vertex[w][2]);
            //        //v = current_mesh_matrix.MultT(v);
            //        //vertex[w][0] = v[0];
            //        //vertex[w][1] = v[1];
            //        //vertex[w][2] = v[2];

            //        //计算世界矩阵
            //        float* m = current_mesh_matrix;
            //        vmml::mat4f vm;
            //        vm.set(&m[0],&m[15],false);

            //        //计算顶点
            //        vmml::vec3f v(vertex[w][0],vertex[w][1],vertex[w][2]);
            //        v = vm * v;
            //        vertex[w][0] = (float)v[0];
            //        vertex[w][1] = (float)v[1];
            //        vertex[w][2] = (float)v[2];


            //        //设置世界矩阵不需要位移信息
            //        vm.set_translation(0,0,0);

            //        if(has_normal)
            //        {
            //            v = vmml::vec3f(normal[w][0],normal[w][1],normal[w][2]);
            //            v = vm * v;
            //            v.normalize();
            //            normal[w][0] = (float)v[0];
            //            normal[w][1] = (float)v[1];
            //            normal[w][2] = (float)v[2];
            //        }

            //        if(has_tangent[0])
            //        {
            //            v = vmml::vec3f(tangent[0][w][0],tangent[0][w][1],tangent[0][w][2]);
            //            v = vm * v;
            //            v.normalize();
            //            tangent[0][w][0] = (float)v[0];
            //            tangent[0][w][1] = (float)v[1];
            //            tangent[0][w][2] = (float)v[2];
            //        }
            //    }
            //}

            ////复制数据到buffer
            //memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_position].get())[vertexCounter*3*3] , &vertex[0][0] ,sizeof(float)*3*3 );
            //if(has_normal){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_normal].get())[vertexCounter*3*3] , &normal[0][0] ,sizeof(float)*3*3 ); }
            //if(has_color[0]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_color0].get())[vertexCounter*3*4] , &color[0][0][0] ,sizeof(float)*3*4 ); }
            //if(has_color[1]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_color1].get())[vertexCounter*3*4] , &color[1][0][0] ,sizeof(float)*3*4 ); }
            //if(has_color[2]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_color2].get())[vertexCounter*3*4] , &color[2][0][0] ,sizeof(float)*3*4 ); }
            //if(has_color[3]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_color3].get())[vertexCounter*3*4] , &color[3][0][0] ,sizeof(float)*3*4 ); }
            //if(has_uv[0]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_uv0].get())[vertexCounter*3*2] , &uv[0][0][0] ,sizeof(float)*3*2 ); }
            //if(has_uv[1]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_uv1].get())[vertexCounter*3*2] , &uv[1][0][0] ,sizeof(float)*3*2 ); }
            //if(has_uv[2]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_uv2].get())[vertexCounter*3*2] , &uv[2][0][0] ,sizeof(float)*3*2 ); }
            //if(has_uv[3]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_uv3].get())[vertexCounter*3*2] , &uv[3][0][0] ,sizeof(float)*3*2 ); }
            //if(has_tangent[0]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_tangent0].get())[vertexCounter*3*3] , &tangent[0][0][0] ,sizeof(float)*3*3 ); }
            //if(has_tangent[1]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_tangent1].get())[vertexCounter*3*3] , &tangent[1][0][0] ,sizeof(float)*3*3 ); }
            //if(has_tangent[2]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_tangent2].get())[vertexCounter*3*3] , &tangent[2][0][0] ,sizeof(float)*3*3 ); }
            //if(has_tangent[3]){ memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_tangent3].get())[vertexCounter*3*3] , &tangent[3][0][0] ,sizeof(float)*3*3 ); }
            //if(has_boneindex){ memcpy( &(*(nbSG::ArrayT<int>*)geometry->mBuffers[NFX_VectexAttribute_bone_indices].get())   [vertexCounter*3*4] , &boneindex[0][0] ,sizeof(int)*3*4 ); }
            //if(has_weight)   { memcpy( &(*(nbSG::ArrayT<float>*)geometry->mBuffers[NFX_VectexAttribute_blend_weights].get())[vertexCounter*3*4] , &weight[0][0]    ,sizeof(float)*3*4 ); }

            ////设置定点属性说明
            //geometry->mVectexAttribute[NFX_VectexAttribute_position] = new nbSG::VectexAttribute(NFX_VectexAttribute_position,3,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_position);
            //if(has_normal){geometry->mVectexAttribute[NFX_VectexAttribute_normal] = new nbSG::VectexAttribute(NFX_VectexAttribute_normal,3,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_normal); }
            //if(has_color[0]){ geometry->mVectexAttribute[NFX_VectexAttribute_color0] = new nbSG::VectexAttribute(NFX_VectexAttribute_color0,4,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_color0); }
            //if(has_color[1]){ geometry->mVectexAttribute[NFX_VectexAttribute_color1] = new nbSG::VectexAttribute(NFX_VectexAttribute_color1,4,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_color1); }
            //if(has_color[2]){ geometry->mVectexAttribute[NFX_VectexAttribute_color2] = new nbSG::VectexAttribute(NFX_VectexAttribute_color2,4,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_color2); }
            //if(has_color[3]){ geometry->mVectexAttribute[NFX_VectexAttribute_color3] = new nbSG::VectexAttribute(NFX_VectexAttribute_color3,4,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_color3); }
            //if(has_uv[0]){geometry->mVectexAttribute[NFX_VectexAttribute_uv0] = new nbSG::VectexAttribute(NFX_VectexAttribute_uv0,2,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_uv0);}
            //if(has_uv[1]){geometry->mVectexAttribute[NFX_VectexAttribute_uv1] = new nbSG::VectexAttribute(NFX_VectexAttribute_uv1,2,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_uv1);}
            //if(has_uv[2]){geometry->mVectexAttribute[NFX_VectexAttribute_uv2] = new nbSG::VectexAttribute(NFX_VectexAttribute_uv2,2,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_uv2);}
            //if(has_uv[3]){geometry->mVectexAttribute[NFX_VectexAttribute_uv3] = new nbSG::VectexAttribute(NFX_VectexAttribute_uv3,2,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_uv3);}
            //if(has_tangent[0]){geometry->mVectexAttribute[NFX_VectexAttribute_tangent0] = new nbSG::VectexAttribute(NFX_VectexAttribute_tangent0,3,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_tangent0);}
            //if(has_tangent[1]){geometry->mVectexAttribute[NFX_VectexAttribute_tangent1] = new nbSG::VectexAttribute(NFX_VectexAttribute_tangent1,3,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_tangent1);}
            //if(has_tangent[2]){geometry->mVectexAttribute[NFX_VectexAttribute_tangent2] = new nbSG::VectexAttribute(NFX_VectexAttribute_tangent2,3,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_tangent2);}
            //if(has_tangent[3]){geometry->mVectexAttribute[NFX_VectexAttribute_tangent3] = new nbSG::VectexAttribute(NFX_VectexAttribute_tangent3,3,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_tangent3);}
            //if(has_boneindex){geometry->mVectexAttribute[NFX_VectexAttribute_bone_indices]  = new nbSG::VectexAttribute(NFX_VectexAttribute_bone_indices ,4,nbPlugins::Renderer::INT   ,false,0,NFX_VectexAttribute_bone_indices); }
            //if(has_weight)   {geometry->mVectexAttribute[NFX_VectexAttribute_blend_weights] = new nbSG::VectexAttribute(NFX_VectexAttribute_blend_weights,4,nbPlugins::Renderer::FLOAT ,false,0,NFX_VectexAttribute_blend_weights); }


            //复制数据到buffer
            memcpy(&(*vectex_list)[vertexCounter * 3], &vertex[0][0], sizeof(float) * 3 * 3);
            if (has_normal) { memcpy(&(*normal_list)[vertexCounter * 3], &normal[0][0], sizeof(float) * 3 * 3); }
            if (has_color[0]) { memcpy(&(*color0_list)[vertexCounter * 3], &color[0][0][0], sizeof(float) * 3 * 4); }
            if (has_uv[0]) { memcpy(&(*uv0_list)[vertexCounter * 3], &uv[0][0][0], sizeof(float) * 3 * 2); }
            if (has_uv[1]) { memcpy(&(*uv1_list)[vertexCounter * 3], &uv[1][0][0], sizeof(float) * 3 * 2); }
            if (has_uv[2]) { memcpy(&(*uv2_list)[vertexCounter * 3], &uv[2][0][0], sizeof(float) * 3 * 2); }
            if (has_uv[3]) { memcpy(&(*uv3_list)[vertexCounter * 3], &uv[3][0][0], sizeof(float) * 3 * 2); }
            if (has_tangent[0]) { memcpy(&(*tangent0_list)[vertexCounter * 3], &tangent[0][0][0], sizeof(float) * 3 * 3); }


            //最后记录定点数量 还可以当作索引使用
            vertexCounter++;
        }//for(int tri_index = 0 ; tri_index < triangleCount ; ++tri_index)
    }//for(int mtl_index = 0 ; mtl_index < material_count ; ++mtl_index)


    //测试数据 revit输出的数据 存在发现为0的情况
    bool zero = true;
    for (size_t i = 0; i < normal_list->size(); i++)
    {
        osg::Vec3 n = (*normal_list)[i];
        if (n.length() > 0.0) zero = false;
    }
    if (zero)
    {
        //printf("a");

        //重新计算面法线
        for (size_t i = 0; i < vectex_list->size(); i+=3)
        {
            osg::Vec3 p0 = (*vectex_list)[i+0];
            osg::Vec3 p1 = (*vectex_list)[i+1];
            osg::Vec3 p2 = (*vectex_list)[i+2];
            
            osg::Vec3 pa = (p2 - p1);
            osg::Vec3 pb = (p0 - p1);
            pa.normalize();
            pb.normalize();

            osg::Vec3 n = pa ^ pb;
            n.normalize();

            (*normal_list)[i + 0] = n;
            (*normal_list)[i + 1] = n;
            (*normal_list)[i + 2] = n;
        }
    }

    //根据材质索引的结果建立subgeometry
    int current_vertex_index = 0;
    for (unsigned int i = 0; i < material_histogram.size(); i++)
    {
        //geometry->mSubGeometry.push_back(new nbSG::SubGeometry(
        //    nbSG::PrimitiveSet::Mode::TRIANGLES
        //    ,current_vertex_index  * 3
        //    ,material_histogram[i] * 3
        //    ));


        //SubGeometry 0-n
        osg::Geometry* geometry_0 = new osg::Geometry();
        geometry_0->setVertexArray(vectex_list);
        if (has_normal) { geometry_0->setNormalArray(normal_list); geometry_0->setNormalBinding(osg::Geometry::AttributeBinding::BIND_PER_VERTEX); }
        //else
        //{
        //    printf("a");
        //}
        if (has_uv[0])geometry_0->setTexCoordArray(0, uv0_list);
        if (has_uv[1])geometry_0->setTexCoordArray(1, uv1_list);
        if (has_uv[2])geometry_0->setTexCoordArray(2, uv2_list);
        if (has_uv[3])geometry_0->setTexCoordArray(3, uv3_list);
        geometry_0->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, current_vertex_index*3, material_histogram[i]*3));
        if(i<material_list.size())geometry_0->setStateSet(material_list[i]);
        meshrender->addDrawable(geometry_0);


        current_vertex_index += material_histogram[i];
    }

    ////压缩索引
    //if(false)
    //{
    //    //顶点数目
    //    unsigned int vertex_count = 0;//geometry->get;

    //    //建立顶点索引数组

    //    //建立全部可能存在的VA数组 用于存储索引后的数据

    //    for (int vi= 0; vi < vertex_count; vi++)
    //    {
    //        //循环对比已经生成的顶点数组 如果一列数据完全相等 就认为重复了

    //    }
    //}

    //设置geometry
    //meshrender->setGeometry( geometry.get() );

    return meshrender;
}



