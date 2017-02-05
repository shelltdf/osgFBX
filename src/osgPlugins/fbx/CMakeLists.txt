SET(NIUBI_SETUP_TARGET_NAME osgdb_fbx)

SET(HEADER_PATH ${PROJECT_SOURCE_DIR}/include/${NIUBI_SETUP_TARGET_NAME})
INCLUDE_DIRECTORIES(AFTER ${OSG_INCLUDE_DIR})

INCLUDE_DIRECTORIES(AFTER ${FBX_INCLUDE_DIR})

SET(NIUBI_SETUP_HEADERS
    ImportFunction.h
    
    loadFBX.h
    loadFBX_util.h
)
SET(NIUBI_SETUP_SOURCES

    loadFBX.cpp
    ReaderWriterFBX.cpp
    
    ImportCameraAndLight.cpp
    ImportLine.cpp
    ImportMaterial.cpp
    ImportMesh.cpp
    ImportTextureAndImage.cpp
)

NIUBI_SETUP_LIBRARY()
TARGET_LINK_LIBRARIES( ${NIUBI_SETUP_TARGET_NAME}
    # ${EXTERNAL_LIBRARIES}
    debug ${OPENTHREADS_LIBRARY_DEBUG} optimized ${OPENTHREADS_LIBRARY}
    debug ${OSG_LIBRARY_DEBUG} optimized ${OSG_LIBRARY}
    debug ${OSGDB_LIBRARY_DEBUG} optimized ${OSGDB_LIBRARY}
    debug ${OSGUTIL_LIBRARY_DEBUG} optimized ${OSGUTIL_LIBRARY}
    debug ${OSGGA_LIBRARY_DEBUG} optimized ${OSGGA_LIBRARY}
    debug ${OSGVIEWER_LIBRARY_DEBUG} optimized ${OSGVIEWER_LIBRARY}

    debug ${FBX_LIBRARY_DEBUG} optimized ${FBX_LIBRARY}
    )
NIUBI_SETUP_INSTALL()
