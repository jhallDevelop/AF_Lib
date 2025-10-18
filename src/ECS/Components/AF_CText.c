#include "ECS/Components/AF_CText.h"
#include "ECS/Components/AF_Component.h"
#include "AF_Vertex.h"
#include <stddef.h> // For NULL

#define AF_CTEXT_DEFAULT_FONT_PATH "assets/font/Antonio-Bold.ttf"
#define AF_CTEXT_DEFAULT_FONT_VERT_PATH "assets/shaders/text.vert"
#define AF_CTEXT_DEFAULT_FONT_FRAG_PATH "assets/shaders/text.frag"
#define AF_CTEXT_DEFAULT_FONT_SIZE 48
// Static quad vertices for text rendering (positions + texCoords)
static AF_Vertex AF_CTEXT_QUAD_VERTS[6] = {
    // Triangle 1: top-left, bottom-left, bottom-right
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // top-left
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // bottom-left
    {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // bottom-right
    
    // Triangle 2: top-left, bottom-right, top-right
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // top-left
    {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // bottom-right
    {{ 1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}  // top-right
};


/*
AF_CText_ZERO
Function used to create an empty text component
====================
*/
AF_CText AF_CText_ZERO(void){
    AF_CText returnMesh = {
    .enabled = AF_FALSE,
    .isDirty = AF_FALSE,
    .isShowing = AF_FALSE,
    .fontID = 0,
    .fontPath = "\0",
    .text = "\0",
    .screenPos = {0,0},
    .textBounds = {0,0},
    .textColor = {0,0,0,1},
    .textData = NULL,
    .mesh = AF_CMesh_ZERO(),
    .font = AF_Font_Zero()
    };

    return returnMesh;
}
/*
====================
AF_CText_ADD
Function used to Add the component
====================
*/
AF_CText AF_CText_ADD(void){
    PACKED_CHAR component = 0;
    component = AF_Component_SetHas(component, AF_TRUE);
    component = AF_Component_SetEnabled(component, AF_TRUE);

    AF_CText returnText = {
	component,
    AF_TRUE,
    AF_TRUE,
    0,
    AF_CTEXT_DEFAULT_FONT_PATH,
    "\0",
    {0,0},
    {0,0},
    {0,0,0,1},
    NULL,
    AF_CMesh_ZERO(),
    AF_Font_Zero()
    };

    // Set the default font size
    returnText.font.fontSize = AF_CTEXT_DEFAULT_FONT_SIZE;
    snprintf(returnText.font.fontPath, AF_MAX_PATH_CHAR_SIZE, "%s", AF_CTEXT_DEFAULT_FONT_PATH);

    // Initialize the mesh to be a quad for rendering text
    returnText.mesh.meshCount = 1;
    returnText.mesh.meshes[0].vertexCount = 6;
    returnText.mesh.meshes[0].indexCount = 6;
    // assign the static quad vertices
    returnText.mesh.meshes[0].vertices = AF_CTEXT_QUAD_VERTS;
    // set the default shader paths
    snprintf(returnText.mesh.shader.vertPath, sizeof(returnText.mesh.shader.vertPath), "%s", AF_CTEXT_DEFAULT_FONT_VERT_PATH);
    snprintf(returnText.mesh.shader.fragPath, sizeof(returnText.mesh.shader.fragPath), "%s", AF_CTEXT_DEFAULT_FONT_FRAG_PATH);

    return returnText;
}


