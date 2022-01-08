#pragma once
#include "../NCLGL/OGLRenderer.h"

#include "../nclgl/SceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
#include "../nclgl/Frustum.h"
#include "Grass.h"
#include "Tree.h"
#include "Water.h";

class Camera;
class SceneNode;
class Mesh;
class Shader;
class Light;
class HeightMap;
//class FlameParticles;
class ParticleEmitter;

const unsigned int SHADOW_MAP_WIDTH = 2048, SHADOW_MAP_HEIGHT = 2048;

class Renderer : public OGLRenderer	{
public:
	Renderer(Window &parent);
	 ~Renderer(void);
	 void RenderScene(bool useSpotRenderer = false)				override;
	 void UpdateScene(float msec, bool useSpotRenderer=false)	override;

	 bool isCameraAutoMove = true;
	 void SwitchCameraAutoMove();
	 void CameraAutoMove(float f);

	 bool isSunLightAutoMove = true;
	 void SwitchSunLightAutoMove();
	 void SunlightAutoMove(float f);

	 bool useSecondCamera = false;
	 void SwitchSecondCamera() {
		 useSecondCamera = !useSecondCamera;
		 if (useSecondCamera) {
			Resize(0, 0, 960, 540);
		 }
		 if (!useSecondCamera) {
			 Resize(0, 0, 1920, 1080);
		 }
	 };

	 int current_post_process_type = 0;
	 int POST_PROCESS_TYPE_NUMS = 5;
	 std::string POST_PROCESS_TYPE[5] = {"none", "blur", "gamma", "bloom", "dof"};
	 void SwitchPostProcess() {
		 current_post_process_type = (current_post_process_type + 1) % POST_PROCESS_TYPE_NUMS;
	 }

	 Matrix4 defaultProjMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	 void Resize(int x1, int y1, int x2, int y2) {
		 width = std::abs(x2-x1);
		 height = std::abs(y2-y1);
		 glViewport(x1, y1, x2, y2);

		 // generate post process frame buffer
		 GeneratePostProcessPrepared();
		 // generate defer render prepared
		 GenerateDerferRenderPrepared();
		 // generata cube shadow mapping;
		 GenerateShadowCubeMapPrepared();
	 }

protected:

	//std::vector<Vector3> objVertices;
	//std::vector<Vector2> objUvs;
	//Mesh* obj;
	//void DrawOBJ(SceneNode* n);
	//Shader* basicShader;

	Shader* spotShader;
	HeightMap* spotHeightMap;
	GLuint spotHeightMapTexture;
	Camera* spotCamera;
	void DrawSpotLightScene();

	void BuildNodeLists(SceneNode* from);
	void SortNodeLists();
	void ClearNodeLists();
	void DrawNodes(std::string& type);
	void DrawNode(SceneNode* n, std::string& type);

	void DrawScenes();
	void DrawSceneNodes(std::string& type);

	void DrawSkyBox();
	void DrawGround(SceneNode* n);
	//void DrawWater(SceneNode* n);
	void DrawBetterWater(SceneNode* n);
	void DrawSunSphere(SceneNode* n);
	void DrawShadowCube(SceneNode* n);
	void DrawParticle();
	void DrawGrasses(SceneNode* n);
	void DrawTrees(SceneNode* n);
	void DrawSkinEagle(SceneNode* n);

	void DrawBetterWater();

	void SetShaderParticleSize(float f);
	ParticleEmitter* flameParticleEmitter;

	void InitRenderToFrameBuffer();
	void GeneratePostProcessPrepared();
	void ChooseFrameTexture();
	void DrawNonePostProcess();
	void DrawBlurPostProcess();
	void DrawGammaPostProcess();
	void DrawBloomPostProcess();
	void DrawDofPostProcess();
	void DrawPostProcess();
	void DrawPresentScene();

	void GenerateScreenTexture(GLuint& into, bool depth = false);
	void GenerateDerferRenderPrepared();
	void DrawLights();
	void CombineDeferRenderBuffers();

	float farPlane = 25.0f;
	Shader* shadowCubeMapShader;
	GLuint shadowDepthCubeMapFBO;
	GLuint shadowDepthCubeMapTex;
	void GenerateShadowCubeMapPrepared();
	void DrawShadowDepthCubeMap();
	std::vector<Matrix4> GenerateShadowTransformsMatrix(Vector3 lPos, float f);
	void DrawObjectShadow(SceneNode* n);
	Matrix4 GenerateModelMatrix(SceneNode* n);

	SceneNode* root;
	Mesh* skybox;
	HeightMap* ground;
	Mesh* grownTree;
	Mesh* waveGrass;
	//Mesh* water;
	std::vector<unsigned int> indices_water;
	Mesh* betterWater;
	Mesh* sunSphere;
	Mesh* shadowCube;
	std::vector<Grass*> Grasses;
	std::vector<Tree*> Trees;
	//FlameParticles* flameParticles;
	Mesh* skinEagle;
	MeshAnimation* skinEagleAnim;
	MeshMaterial* skinEagleMat;
	vector<GLuint> skinEagleMatTextures;
	int currentFrameForAnim;
	float frameTimeForAnim;

	Light* sunLight;
	SpotLight* spotLight;

	GLuint skyboxTexture;
	GLuint groundGrassTexture;
	GLuint groundGrassBumpTexture;
	GLuint groundGrassGlossTexture;
	GLuint groundSandTexture;
	GLuint groundSandBumpTexture;
	GLuint groundSandGlossTexture;
	//GLuint waterTexture;
	//GLuint waterBumpTexture;
	GLuint betterWaterTexture;
	GLuint sunTexture;
	GLuint treeTrunkTex;
	GLuint treeLeafTex;

	// post process need
	Mesh* postProcessQuad;
	GLuint sceneFBO;
	GLuint postProcessFBO;
	GLuint gammaPostProcessFBO;
	GLuint bloomBlurFBO;
	GLuint bloomBrightFBO;
	GLuint bloomPostProcessFBO;
	GLuint postProcessBufferColourTex[2];
	GLuint postProcessGammaBufferColourTex[2];
	GLuint postProcessBloomBufferColourTex[2];
	GLuint postProcessBloomBlurBufferColourTex[2];
	GLuint postProcessBloomBrightBufferColourTex[2];
	GLuint postProcessDepthTex;

	// defer rendering need
	// use postProcessQuad to draw
	Mesh* deferRenderQuad;
	GLuint derferRenderFBO;
	GLuint deferRenderBufferColorTex;
	GLuint deferRenderBufferNormalTex;
	GLuint deferRenderBufferDepthTex;
	GLuint deferRenderLightsFBO;
	GLuint deferRenderLightsDepthTex;
	GLuint deferRenderLightsDiffuseTex;
	GLuint deferRenderLightsSpecularTex;
	GLuint deferRenderLightsSkyTex;
	GLuint deferRenderLightsParticleTex;

	Shader* groundShader;
	Shader* skyShader;
	//Shader* waterShader;
	Shader* betterWaterShader;
	Shader* sunSphereShader;
	Shader* particleShader;
	Shader* shadowCubeShader;
	Shader* grassShader;
	Shader* treeShader;
	Shader* leafShader;
	Shader* skinObjShader;

	Shader* nonePostProcessShader;
	Shader* blurPostProcessShader;
	Shader* gammaCorrectionPostProcessShader;
	Shader* bloomBrightShader;
	Shader* bloomBlurShader;
	Shader* bloomFinalShader;
	Shader* dofShader;
	Shader* presentSceneShader;

	Shader* lightsShader;
	Shader* deferRenderCombineShader;

	Frustum frameFrustum;

	Camera* mainCamera;
	float sceneTime;
	float waterRotate;
	float waterCycle;

	std::vector<SceneNode*> transparentNodeList;
	std::vector<SceneNode*> nodeList;
};
