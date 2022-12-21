#include <assert.h>

#include "simple_logger.h"

#include "gf3d_buffers.h"
#include "gf3d_swapchain.h"
#include "gf3d_commands.h"
#include "gf3d_vgraphics.h"
#include "gf3d_obj_load.h"
#include "gf3d_uniform_buffers.h"

#include "gf3d_model.h"

typedef struct
{
    Model               *   model_list;
    Uint32                  max_models;
    Uint32                  chain_length;   /**<length of swap chain*/
    VkDevice                device;
    Pipeline            *   pipe;           /**<the pipeline associated with model rendering*/
}ModelManager;

typedef struct
{
    Vector4D    lightDirection;
    Vector4D    lightDiffuse;
    Vector4D    lightAmbient;
    Vector4D    lightSpecular;
} SceneLighting;

static ModelManager gf3d_model = {0};

static SceneLighting gf3d_lighting = {0};

void lighting_init(){
    gf3d_lighting.lightDirection = vector4d(-0.2f,-1.0f,-0.3f,0);
    gf3d_lighting.lightDiffuse = vector4d(1,1,1,1);
    gf3d_lighting.lightAmbient = vector4d(0.2,0.2,0.2,1);
    gf3d_lighting.lightSpecular = vector4d(1,1,1,0);

}

void lighting_set_specular(Vector4D specular){
    vector4d_copy(gf3d_lighting.lightSpecular, specular);
}

void lighting_set_diffuse(Vector4D diffuse){
    vector4d_copy(gf3d_lighting.lightDiffuse, diffuse);
}

void lighting_set_ambient(Vector4D ambient){
    vector4d_copy(gf3d_lighting.lightAmbient, ambient);
}

void lighting_set_direction(Vector4D direction){
    vector4d_copy(gf3d_lighting.lightDirection, direction);
}


void gf3d_model_delete(Model *model);

void gf3d_model_create_descriptor_pool(Model *model);
void gf3d_model_create_descriptor_sets(Model *model);
void gf3d_model_create_descriptor_set_layout();
void gf3d_model_update_uniform_buffer(
    Model *model,
    UniformBuffer *ubo,
    Matrix4 modelMat,
    Vector4D colorMod,
    Vector4D ambientLight,
    Vector4D texOffset
);
void gf3d_map_update_uniform_buffer(
    Model *model,
    UniformBuffer *ubo,
    Matrix4 modelMat,
    Vector4D lightDirection,
    Vector4D lightDiffuse,
    Vector4D lightAmbient,
    Vector4D lightSpecular,
    float shininess
);
void gf3d_model_update_highlight_uniform_buffer(
    Model *model,
    UniformBuffer *ubo,
    Matrix4 modelMat,
    Vector4D highlightColor);

void gf3d_model_update_highlight_model_descriptor_set(
    Model *model,
    VkDescriptorSet descriptorSet,
    Uint32 chainIndex,
    Matrix4 modelMat,
    Vector4D highlightColor);

void gf3d_model_update_basic_model_descriptor_set(
    Model *model,
    VkDescriptorSet descriptorSet,
    Uint32 chainIndex,
    Matrix4 modelMat,
    Vector4D colorMod,
    Vector4D ambientLight,
    Vector4D textureOffset);

void gf3d_model_update_map_descriptor_set(
    Model *model,
    VkDescriptorSet descriptorSet,
    Uint32 chainIndex,
    Matrix4 modelMat,
    Vector4D lightDirection,
    Vector4D lightDiffuse,
    Vector4D lightAmbient,
    Vector4D lightSpecular,
    float shininess);

void gf3d_model_update_displacement_model_descriptor_set(
    Model *model,
    VkDescriptorSet descriptorSet,
    Uint32 chainIndex,
    Matrix4 modelMat,
    Vector4D colorMod,
    Vector4D ambientLight,
    Vector4D textureOffset);

VkDescriptorSetLayout * gf3d_model_get_descriptor_set_layout();

void gf3d_model_manager_close()
{
    int i;
    for (i = 0; i < gf3d_model.max_models;i++)
    {
        gf3d_model_delete(&gf3d_model.model_list[i]);
    }
    if (gf3d_model.model_list)
    {
        free(gf3d_model.model_list);
    }
    memset(&gf3d_model,0,sizeof(ModelManager));
    slog("model manager closed");
}

void gf3d_model_manager_init(Uint32 max_models)
{
    if (max_models == 0)
    {
        slog("cannot intilizat model manager for 0 models");
        return;
    }
    gf3d_model.chain_length = gf3d_swapchain_get_chain_length();
    gf3d_model.model_list = (Model *)gfc_allocate_array(sizeof(Model),max_models);
    gf3d_model.max_models = max_models;
    gf3d_model.device = gf3d_vgraphics_get_default_logical_device();
    gf3d_model.pipe = gf3d_mesh_get_pipeline();
    
    slog("model manager initiliazed");
    atexit(gf3d_model_manager_close);
}

Model * gf3d_model_new()
{
    int i;
    for (i = 0; i < gf3d_model.max_models;i++)
    {
        if (!gf3d_model.model_list[i]._inuse)
        {
            gf3d_model_delete(&gf3d_model.model_list[i]);
            gf3d_model.model_list[i]._inuse = 1;
            return &gf3d_model.model_list[i];
        }
    }
    slog("unable to make a new model, out of space");
    return NULL;
}

Model * gf3d_model_load(const char * filename)
{    
    SJson *json,*config;
    Model *model;
    if (!filename)return NULL;
    json = sj_load(filename);
    if (!json)return NULL;
    config = sj_object_get_value(json,"model");
    if (!config)
    {
        slog("file %s contains no model object",filename);
        sj_free(json);
        return NULL;
    }
    model = gf3d_model_load_from_config(config);
    sj_free(json);
    return model;
}

Model * gf3d_model_load_full(const char * modelFile,const char *textureFile)
{
    Model *model;
    model = gf3d_model_new();
    if (!model)return NULL;
    
    gfc_line_cpy(model->filename,modelFile);

    model->mesh = gf3d_mesh_load(modelFile);
    if (!model->mesh)
    {
        gf3d_model_free(model);
        return NULL;
    }
    model->texture = gf3d_texture_load(textureFile);

    if (!model->texture)
    {
        model->texture = gf3d_texture_load("images/default.png");
    }
    
    return model;
}

Model * gf3d_model_load_from_config(SJson *json)
{
    const char *model;
    const char *texture;
    if (!json)return NULL;
    model = sj_get_string_value(sj_object_get_value(json,"model"));
    texture = sj_get_string_value(sj_object_get_value(json,"texture"));
    return gf3d_model_load_full(model,texture);
}


void gf3d_model_free(Model *model)
{
    gf3d_model_delete(model);
}

void gf3d_model_delete(Model *model)
{
    if (!model)return;
    if (!model->_inuse)return;// not in use, nothing to do
    
    gf3d_mesh_free(model->mesh);
    gf3d_texture_free(model->texture);
    memset(model,0,sizeof(Model));
}

void gf3d_model_draw(Model *model,Matrix4 modelMat,Vector4D colorMod,Vector4D ambientLight,Vector4D texOffset)
{
    VkDescriptorSet *descriptorSet = NULL;
    VkCommandBuffer commandBuffer;
    Uint32 bufferFrame;
    if (!model)
    {
        return;
    }
    commandBuffer = gf3d_mesh_get_model_command_buffer();
    bufferFrame = gf3d_vgraphics_get_current_buffer_frame();
    descriptorSet = gf3d_pipeline_get_descriptor_set(gf3d_model.pipe, bufferFrame);
    if (descriptorSet == NULL)
    {
        slog("failed to get a free descriptor Set for model rendering");
        return;
    }
    gf3d_model_update_basic_model_descriptor_set(model,*descriptorSet,bufferFrame,modelMat,colorMod,ambientLight,texOffset);
    gf3d_mesh_render(model->mesh,commandBuffer,descriptorSet);
}

void gf3d_model_draw_map(Model *model,Matrix4 modelMat, float shininess)
{
    VkDescriptorSet *descriptorSet = NULL;
    VkCommandBuffer commandBuffer;
    Uint32 bufferFrame;
    if (!model)
    {
        return;
    }
    commandBuffer = gf3d_mesh_get_map_command_buffer();
    bufferFrame = gf3d_vgraphics_get_current_buffer_frame();
    descriptorSet = gf3d_pipeline_get_descriptor_set(gf3d_mesh_get_map_pipeline(), bufferFrame);
    if (descriptorSet == NULL)
    {
        slog("failed to get a free descriptor Set for model rendering");
        return;
    }
    gf3d_model_update_map_descriptor_set(model,*descriptorSet,bufferFrame,modelMat,gf3d_lighting.lightDirection,gf3d_lighting.lightDiffuse,gf3d_lighting.lightAmbient,gf3d_lighting.lightSpecular, shininess);
    gf3d_mesh_render_map(model->mesh,commandBuffer,descriptorSet);
}

void gf3d_model_draw_water(Model *model,Matrix4 modelMat,Vector4D colorMod,Vector4D ambientLight,Vector4D texOffset)
{
    VkDescriptorSet *descriptorSet = NULL;
    VkCommandBuffer commandBuffer;
    Uint32 bufferFrame;
    if (!model)
    {
        return;
    }
    commandBuffer = gf3d_mesh_get_water_command_buffer();
    bufferFrame = gf3d_vgraphics_get_current_buffer_frame();
    descriptorSet = gf3d_pipeline_get_descriptor_set(gf3d_mesh_get_water_pipeline(), bufferFrame);
    if (descriptorSet == NULL)
    {
        slog("failed to get a free descriptor Set for model rendering");
        return;
    }
    gf3d_model_update_basic_model_descriptor_set(model,*descriptorSet,bufferFrame,modelMat,colorMod,ambientLight,texOffset);
    gf3d_mesh_render_water(model->mesh,commandBuffer,descriptorSet);
}

void gf3d_model_draw_displacement(Model *model,Matrix4 modelMat,Vector4D colorMod,Vector4D ambientLight,Vector4D texOffset)
{
    VkDescriptorSet *descriptorSet = NULL;
    VkCommandBuffer commandBuffer;
    Uint32 bufferFrame;
    if (!model)
    {
        return;
    }
    commandBuffer = gf3d_mesh_get_displacement_command_buffer();
    bufferFrame = gf3d_vgraphics_get_current_buffer_frame();
    descriptorSet = gf3d_pipeline_get_descriptor_set(gf3d_mesh_get_displacement_pipeline(), bufferFrame);
    if (descriptorSet == NULL)
    {
        slog("failed to get a free descriptor Set for model rendering");
        return;
    }
    gf3d_model_update_displacement_model_descriptor_set(model,*descriptorSet,bufferFrame,modelMat,colorMod,ambientLight,texOffset);
    gf3d_mesh_render_displacement(model->mesh,commandBuffer,descriptorSet);
}

void gf3d_model_draw_highlight(Model *model,Matrix4 modelMat,Vector4D highlight)
{
    VkDescriptorSet *descriptorSet = NULL;
    VkCommandBuffer commandBuffer;
    Uint32 bufferFrame;
    if (!model)
    {
        return;
    }
    commandBuffer = gf3d_mesh_get_highlight_command_buffer();
    bufferFrame = gf3d_vgraphics_get_current_buffer_frame();
    descriptorSet = gf3d_pipeline_get_descriptor_set(gf3d_mesh_get_highlight_pipeline(), bufferFrame);
    if (descriptorSet == NULL)
    {
        slog("failed to get a free descriptor Set for model rendering");
        return;
    }
    gf3d_model_update_highlight_model_descriptor_set(model,*descriptorSet,bufferFrame,modelMat,highlight);
    gf3d_mesh_render_highlight(model->mesh,commandBuffer,descriptorSet);
}

void gf3d_model_update_sky_uniform_buffer(
    Model *model,
    UniformBuffer *ubo,
    Matrix4 modelMat,
    Vector4D colorMod)
{
    void* data;
    UniformBufferObject graphics_ubo;
    SkyUBO modelUBO;
    graphics_ubo = gf3d_vgraphics_get_uniform_buffer_object();
    
    gfc_matrix_copy(modelUBO.model,modelMat);
    gfc_matrix_copy(modelUBO.view,graphics_ubo.view);
     modelUBO.view[0][3] = 0;
     modelUBO.view[1][3] = 0;
     modelUBO.view[2][3] = 0;
     modelUBO.view[3][0] = 0;
     modelUBO.view[3][1] = 0;
     modelUBO.view[3][2] = 0;
    gfc_matrix_copy(modelUBO.proj,graphics_ubo.proj);
    
    vector4d_copy(modelUBO.color,colorMod);
        
    vkMapMemory(gf3d_model.device, ubo->uniformBufferMemory, 0, sizeof(MeshUBO), 0, &data);
    
        memcpy(data, &modelUBO, sizeof(SkyUBO));

    vkUnmapMemory(gf3d_model.device, ubo->uniformBufferMemory);
}

void gf3d_model_update_sky_model_descriptor_set(
    Model *model,
    VkDescriptorSet descriptorSet,
    Uint32 chainIndex,
    Matrix4 modelMat,
    Vector4D colorMod)
{
    VkDescriptorImageInfo imageInfo = {0};
    VkWriteDescriptorSet descriptorWrite[2] = {0};
    VkDescriptorBufferInfo bufferInfo = {0};
    Pipeline *pipe;
    UniformBuffer *ubo = NULL;
    
    if (!model)
    {
        slog("no model provided for descriptor set update");
        return;
    }
    if (descriptorSet == VK_NULL_HANDLE)
    {
        slog("null handle provided for descriptorSet");
        return;
    }
    pipe = gf3d_mesh_get_sky_pipeline();
    ubo = gf3d_uniform_buffer_list_get_buffer(pipe->uboList, chainIndex);
    if (!ubo)
    {
        slog("failed to get a free uniform buffer for draw call");
        return;
    }
    
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = model->texture->textureImageView;
    imageInfo.sampler = model->texture->textureSampler;

    gf3d_model_update_sky_uniform_buffer(model,ubo,modelMat,colorMod);
    bufferInfo.buffer = ubo->uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(SkyUBO);        
    
    descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite[0].dstSet = descriptorSet;
    descriptorWrite[0].dstBinding = 0;
    descriptorWrite[0].dstArrayElement = 0;
    descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite[0].descriptorCount = 1;
    descriptorWrite[0].pBufferInfo = &bufferInfo;

    descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite[1].dstSet = descriptorSet;
    descriptorWrite[1].dstBinding = 1;
    descriptorWrite[1].dstArrayElement = 0;
    descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite[1].descriptorCount = 1;                        
    descriptorWrite[1].pImageInfo = &imageInfo;
    descriptorWrite[1].pTexelBufferView = NULL; // Optional

    vkUpdateDescriptorSets(gf3d_model.device, 2, descriptorWrite, 0, NULL);
}

void gf3d_model_draw_sky(Model *model,Matrix4 modelMat,Color color)
{
    VkDescriptorSet *descriptorSet = NULL;
    VkCommandBuffer commandBuffer;
    Uint32 bufferFrame;
    if (!model)
    {
        return;
    }
    commandBuffer = gf3d_mesh_get_sky_command_buffer();
    bufferFrame = gf3d_vgraphics_get_current_buffer_frame();
    descriptorSet = gf3d_pipeline_get_descriptor_set(gf3d_mesh_get_sky_pipeline(), bufferFrame);
    if (descriptorSet == NULL)
    {
        slog("failed to get a free descriptor Set for model rendering");
        return;
    }
    gf3d_model_update_sky_model_descriptor_set(model,*descriptorSet,bufferFrame,modelMat,gfc_color_to_vector4f(color));
    gf3d_mesh_render_sky(model->mesh,commandBuffer,descriptorSet);
}


void gf3d_model_update_basic_model_descriptor_set(
    Model *model,
    VkDescriptorSet descriptorSet,
    Uint32 chainIndex,
    Matrix4 modelMat,
    Vector4D colorMod,
    Vector4D ambientLight,
    Vector4D texOffset)
{
    VkDescriptorImageInfo imageInfo = {0};
    VkWriteDescriptorSet descriptorWrite[2] = {0};
    VkDescriptorBufferInfo bufferInfo = {0};
    UniformBuffer *ubo = NULL;
    
    if (!model)
    {
        slog("no model provided for descriptor set update");
        return;
    }
    if (descriptorSet == VK_NULL_HANDLE)
    {
        slog("null handle provided for descriptorSet");
        return;
    }
    ubo = gf3d_uniform_buffer_list_get_buffer(gf3d_model.pipe->uboList, chainIndex);
    if (!ubo)
    {
        slog("failed to get a free uniform buffer for draw call");
        return;
    }
    
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = model->texture->textureImageView;
    imageInfo.sampler = model->texture->textureSampler;

    gf3d_model_update_uniform_buffer(model,ubo,modelMat,colorMod,ambientLight,texOffset);
    bufferInfo.buffer = ubo->uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(MeshUBO);        
    
    descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite[0].dstSet = descriptorSet;
    descriptorWrite[0].dstBinding = 0;
    descriptorWrite[0].dstArrayElement = 0;
    descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite[0].descriptorCount = 1;
    descriptorWrite[0].pBufferInfo = &bufferInfo;

    descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite[1].dstSet = descriptorSet;
    descriptorWrite[1].dstBinding = 1;
    descriptorWrite[1].dstArrayElement = 0;
    descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite[1].descriptorCount = 1;                        
    descriptorWrite[1].pImageInfo = &imageInfo;
    descriptorWrite[1].pTexelBufferView = NULL; // Optional

    vkUpdateDescriptorSets(gf3d_model.device, 2, descriptorWrite, 0, NULL);
}

void gf3d_model_update_map_descriptor_set(
    Model *model,
    VkDescriptorSet descriptorSet,
    Uint32 chainIndex,
    Matrix4 modelMat,
    Vector4D lightDirection,
    Vector4D lightDiffuse,
    Vector4D lightAmbient,
    Vector4D lightSpecular,
    float shininess)
{
    VkDescriptorImageInfo imageInfo = {0};
    VkWriteDescriptorSet descriptorWrite[2] = {0};
    VkDescriptorBufferInfo bufferInfo = {0};
    UniformBuffer *ubo = NULL;
    
    if (!model)
    {
        slog("no model provided for descriptor set update");
        return;
    }
    if (descriptorSet == VK_NULL_HANDLE)
    {
        slog("null handle provided for descriptorSet");
        return;
    }

    Pipeline* pipe = gf3d_mesh_get_map_pipeline();
    ubo = gf3d_uniform_buffer_list_get_buffer(pipe->uboList, chainIndex);
    if (!ubo)
    {
        slog("failed to get a free uniform buffer for draw call");
        return;
    }
    
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = model->texture->textureImageView;
    imageInfo.sampler = model->texture->textureSampler;

    gf3d_map_update_uniform_buffer(model,ubo,modelMat, lightDirection, lightDiffuse, lightAmbient, lightSpecular, shininess);
    bufferInfo.buffer = ubo->uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(MapUBO);        
    
    descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite[0].dstSet = descriptorSet;
    descriptorWrite[0].dstBinding = 0;
    descriptorWrite[0].dstArrayElement = 0;
    descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite[0].descriptorCount = 1;
    descriptorWrite[0].pBufferInfo = &bufferInfo;

    descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite[1].dstSet = descriptorSet;
    descriptorWrite[1].dstBinding = 1;
    descriptorWrite[1].dstArrayElement = 0;
    descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite[1].descriptorCount = 1;                        
    descriptorWrite[1].pImageInfo = &imageInfo;
    descriptorWrite[1].pTexelBufferView = NULL; // Optional

    vkUpdateDescriptorSets(gf3d_model.device, 2, descriptorWrite, 0, NULL);
}

void gf3d_model_update_displacement_model_descriptor_set(
    Model *model,
    VkDescriptorSet descriptorSet,
    Uint32 chainIndex,
    Matrix4 modelMat,
    Vector4D colorMod,
    Vector4D ambientLight,
    Vector4D texOffset)
{
    VkDescriptorImageInfo imageInfo = {0};
    VkDescriptorImageInfo imageInfoDisplacement = {0};
    VkWriteDescriptorSet descriptorWrite[3] = {0};
    VkDescriptorBufferInfo bufferInfo = {0};
    UniformBuffer *ubo = NULL;
    
    if (!model)
    {
        slog("no model provided for descriptor set update");
        return;
    }
    if (descriptorSet == VK_NULL_HANDLE)
    {
        slog("null handle provided for descriptorSet");
        return;
    }
    Pipeline* pipe = gf3d_mesh_get_displacement_pipeline();
    ubo = gf3d_uniform_buffer_list_get_buffer(pipe->uboList, chainIndex);
    if (!ubo)
    {
        slog("failed to get a free uniform buffer for draw call");
        return;
    }
    
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = model->texture->textureImageView;
    imageInfo.sampler = model->texture->textureSampler;

    imageInfoDisplacement.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfoDisplacement.imageView = model->displacementTexture->textureImageView;
    imageInfoDisplacement.sampler = model->displacementTexture->textureSampler;

    gf3d_model_update_uniform_buffer(model,ubo,modelMat,colorMod,ambientLight,texOffset);
    bufferInfo.buffer = ubo->uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(MeshUBO);        
    
    descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite[0].dstSet = descriptorSet;
    descriptorWrite[0].dstBinding = 0;
    descriptorWrite[0].dstArrayElement = 0;
    descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite[0].descriptorCount = 1;
    descriptorWrite[0].pBufferInfo = &bufferInfo;

    descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite[1].dstSet = descriptorSet;
    descriptorWrite[1].dstBinding = 1;
    descriptorWrite[1].dstArrayElement = 0;
    descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite[1].descriptorCount = 1;                        
    descriptorWrite[1].pImageInfo = &imageInfo;
    descriptorWrite[1].pTexelBufferView = NULL; // Optional

    descriptorWrite[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite[2].dstSet = descriptorSet;
    descriptorWrite[2].dstBinding = 2;
    descriptorWrite[2].dstArrayElement = 0;
    descriptorWrite[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite[2].descriptorCount = 1;                        
    descriptorWrite[2].pImageInfo = &imageInfoDisplacement;
    descriptorWrite[2].pTexelBufferView = NULL; // Optional

    vkUpdateDescriptorSets(gf3d_model.device, 3, descriptorWrite, 0, NULL);
}

void gf3d_model_update_highlight_model_descriptor_set(
    Model *model,
    VkDescriptorSet descriptorSet,
    Uint32 chainIndex,
    Matrix4 modelMat,
    Vector4D highlightColor)
{
    VkDescriptorImageInfo imageInfo = {0};
    VkWriteDescriptorSet descriptorWrite[2] = {0};
    VkDescriptorBufferInfo bufferInfo = {0};
    Pipeline *pipe;
    UniformBuffer *ubo = NULL;
    
    if (!model)
    {
        slog("no model provided for descriptor set update");
        return;
    }
    if (descriptorSet == VK_NULL_HANDLE)
    {
        slog("null handle provided for descriptorSet");
        return;
    }
    pipe = gf3d_mesh_get_highlight_pipeline();
    ubo = gf3d_uniform_buffer_list_get_buffer(pipe->uboList, chainIndex);
    if (!ubo)
    {
        slog("failed to get a free uniform buffer for draw call");
        return;
    }
    
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = model->texture->textureImageView;
    imageInfo.sampler = model->texture->textureSampler;

    gf3d_model_update_highlight_uniform_buffer(model,ubo,modelMat,highlightColor);
    bufferInfo.buffer = ubo->uniformBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(HighlightUBO);        
    
    descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite[0].dstSet = descriptorSet;
    descriptorWrite[0].dstBinding = 0;
    descriptorWrite[0].dstArrayElement = 0;
    descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite[0].descriptorCount = 1;
    descriptorWrite[0].pBufferInfo = &bufferInfo;

    descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite[1].dstSet = descriptorSet;
    descriptorWrite[1].dstBinding = 1;
    descriptorWrite[1].dstArrayElement = 0;
    descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite[1].descriptorCount = 1;                        
    descriptorWrite[1].pImageInfo = &imageInfo;
    descriptorWrite[1].pTexelBufferView = NULL; // Optional

    vkUpdateDescriptorSets(gf3d_model.device, 2, descriptorWrite, 0, NULL);
}

void gf3d_model_update_uniform_buffer(
    Model *model,
    UniformBuffer *ubo,
    Matrix4 modelMat,
    Vector4D colorMod,
    Vector4D ambient,
    Vector4D texOffset)
{
    void* data;
    UniformBufferObject graphics_ubo;
    MeshUBO modelUBO;
    graphics_ubo = gf3d_vgraphics_get_uniform_buffer_object();
    
    gfc_matrix_copy(modelUBO.model,modelMat);
    gfc_matrix_copy(modelUBO.view,graphics_ubo.view);
    gfc_matrix_copy(modelUBO.proj,graphics_ubo.proj);
    
    vector4d_copy(modelUBO.color,colorMod);
    vector4d_copy(modelUBO.ambient,ambient);
    vector4d_copy(modelUBO.texoffset,texOffset);

    vkMapMemory(gf3d_model.device, ubo->uniformBufferMemory, 0, sizeof(MeshUBO), 0, &data);
    
        memcpy(data, &modelUBO, sizeof(MeshUBO));

    vkUnmapMemory(gf3d_model.device, ubo->uniformBufferMemory);
}

void gf3d_map_update_uniform_buffer(
    Model *model,
    UniformBuffer *ubo,
    Matrix4 modelMat,
    Vector4D lightDirection,
    Vector4D lightDiffuse,
    Vector4D lightAmbient,
    Vector4D lightSpecular,
    float shininess
)
{
    void* data;
    UniformBufferObject graphics_ubo;
    MapUBO mapUBO;
    graphics_ubo = gf3d_vgraphics_get_uniform_buffer_object();
    
    gfc_matrix_copy(mapUBO.model,modelMat);
    gfc_matrix_copy(mapUBO.view,graphics_ubo.view);
    gfc_matrix_copy(mapUBO.proj,graphics_ubo.proj);
    
    Vector4D cameraPos = vector4d(graphics_ubo.view[3][0],graphics_ubo.view[3][1],graphics_ubo.view[3][2],graphics_ubo.view[3][3]);
    vector4d_copy(mapUBO.cameraPos, cameraPos);
    vector4d_copy(mapUBO.lightDirection,lightDirection);
    vector4d_copy(mapUBO.lightDiffuse,lightDiffuse);
    vector4d_copy(mapUBO.lightAmbient,lightAmbient);
    lightSpecular.w = shininess;
    vector4d_copy(mapUBO.lightSpecular,lightSpecular);

    vkMapMemory(gf3d_model.device, ubo->uniformBufferMemory, 0, sizeof(MapUBO), 0, &data);
    
    memcpy(data, &mapUBO, sizeof(MapUBO));

    vkUnmapMemory(gf3d_model.device, ubo->uniformBufferMemory);
}

void gf3d_model_update_highlight_uniform_buffer(
    Model *model,
    UniformBuffer *ubo,
    Matrix4 modelMat,
    Vector4D highlightColor)
{
    void* data;
    UniformBufferObject graphics_ubo;
    HighlightUBO modelUBO;
    graphics_ubo = gf3d_vgraphics_get_uniform_buffer_object();
    
    gfc_matrix_copy(modelUBO.model,modelMat);
    gfc_matrix_copy(modelUBO.view,graphics_ubo.view);
    gfc_matrix_copy(modelUBO.proj,graphics_ubo.proj);
    
    vector4d_copy(modelUBO.color,highlightColor);
        
    vkMapMemory(gf3d_model.device, ubo->uniformBufferMemory, 0, sizeof(MeshUBO), 0, &data);
    
        memcpy(data, &modelUBO, sizeof(HighlightUBO));

    vkUnmapMemory(gf3d_model.device, ubo->uniformBufferMemory);
}

/*eol@eof*/
