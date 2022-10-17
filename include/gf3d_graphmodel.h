#ifndef __GF3D_GRAPH_MODEL_H__
#define __GF3D_GRAPH_MODEL_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_text.h"

#include "gf3d_texture.h"
#include "gf3d_mesh.h"

typedef struct {
    Uint32          vertexCount;
    VkBuffer        buffer;
    VkDeviceMemory  bufferMemory;

    Uint32          primitiveCount;
    VkBuffer        primitiveBuffer;
    VkDeviceMemory  primitiveBufferMemory;
} GMesh;

typedef struct GraphNode {
    struct GraphNode* parent;
    struct GraphNode* child;
    struct GraphNode* previous;
    struct GraphNode* next;

    Matrix4 transform;

    uint16_t mesh;
    uint16_t material;
    
} GraphNode;

typedef struct GraphModel {
    Uint32 mesh_count;
    Uint32 texture_count;
    Uint32 node_count;
    
    GMesh* meshes;
    Texture* textures;
    GraphNode* nodes;
}

#endif