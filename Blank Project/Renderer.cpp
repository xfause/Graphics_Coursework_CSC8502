#include "Renderer.h"
#include <algorithm>

#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Shader.h"
#include "../nclgl/HeightMap.h"

#include "ParticleEmitter.h";
#include "../nclgl/ObjLoader.h";

Renderer::Renderer(Window &parent) : OGLRenderer(parent)	{

	// spot light scene
	spotHeightMap = new HeightMap(TEXTUREDIR"noise.png");
	spotHeightMapTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	spotShader = new Shader(SHADERDIR"coursework/spot_light/PerPixelVertex.glsl", SHADERDIR"coursework/spot_light/PerPixelFragment.glsl");

	if (!spotShader->LoadSuccess() || !spotHeightMapTexture) {
		return;
	}
	SetTextureRepeating(spotHeightMapTexture, true);
	Vector3 spotHeightMapSize = spotHeightMap->GetHeightMapSize();
	spotCamera = new Camera(-45.0f, 0.0f, spotHeightMapSize * Vector3(0.5f, 20.0f, 0.5f), 10.0f);
	spotLight = new SpotLight(spotHeightMapSize * Vector3(0.5f, 1.5f, 0.5f), spotHeightMapSize * Vector3(0.5f, 0.5f, 0.0f), Vector4(1, 1, 1, 1), 0.9f, 0.7f);



	// origin scene
	ground = new HeightMap(TEXTUREDIR"noise.png");
	Vector3 groundSize = ground->GetHeightMapSize();

	mainCamera = new Camera(-45.0f, 0.0f, groundSize * Vector3(0.5f, 5.0f, 0.5f), 10);
	sunLight = new Light(groundSize * Vector3(0.5f, 1.5f, 0.5f), Vector4(1, 1, 1, 1), groundSize.x);

	farPlane = sunLight->GetRadius();

	skybox = Mesh::GenerateQuad();
	//water = Mesh::GenerateQuad();
	sunSphere = Mesh::LoadFromMeshFile("Sphere.msh");
	shadowCube = Mesh::LoadFromMeshFile("Cube.msh");

	groundGrassTexture = SOIL_load_OGL_texture(TEXTUREDIR"coursework/grass_ground.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	groundGrassBumpTexture = SOIL_load_OGL_texture(TEXTUREDIR"coursework/grass_ground_bump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	groundGrassGlossTexture = SOIL_load_OGL_texture(TEXTUREDIR"coursework/grass_ground_gloss.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	groundSandTexture = SOIL_load_OGL_texture(TEXTUREDIR"coursework/sand_ground.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	groundSandBumpTexture = SOIL_load_OGL_texture(TEXTUREDIR"coursework/sand_ground_bump.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	groundSandGlossTexture = SOIL_load_OGL_texture(TEXTUREDIR"coursework/sand_ground_gloss.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	skyboxTexture = SOIL_load_OGL_cubemap(
		TEXTUREDIR"coursework/sky_left.bmp", TEXTUREDIR"coursework/sky_right.bmp",
		TEXTUREDIR"coursework/sky_up.bmp", TEXTUREDIR"coursework/sky_down.bmp",
		TEXTUREDIR"coursework/sky_back.bmp", TEXTUREDIR"coursework/sky_front.bmp",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0
	);
	//waterTexture = SOIL_load_OGL_texture(TEXTUREDIR"coursework/water.tga", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	//waterBumpTexture = SOIL_load_OGL_texture(TEXTUREDIR"coursework/waterbump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	betterWaterTexture = SOIL_load_OGL_texture(TEXTUREDIR"coursework/water.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	sunTexture = SOIL_load_OGL_texture(TEXTUREDIR"coursework/sun.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	treeTrunkTex = SOIL_load_OGL_texture(TEXTUREDIR"coursework/tree_trunk.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	treeLeafTex = SOIL_load_OGL_texture(TEXTUREDIR"coursework/leaf.bmp", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	// todo
	if (!skyboxTexture 
		|| !groundGrassTexture
		|| !groundGrassBumpTexture
		|| !groundGrassGlossTexture
		|| !betterWaterTexture
		//|| !waterTexture 
		//|| !waterBumpTexture 
		|| !sunTexture
		|| !treeTrunkTex
		|| !treeLeafTex
		) {
		return;
	}

	SetTextureRepeating(groundGrassTexture, true);
	SetTextureRepeating(groundGrassBumpTexture, true);
	SetTextureRepeating(groundGrassGlossTexture, true);
	SetTextureRepeating(groundSandTexture, true);
	SetTextureRepeating(groundSandBumpTexture, true);
	SetTextureRepeating(groundSandGlossTexture, true);
	SetTextureRepeating(betterWaterTexture, true);
	//SetTextureRepeating(waterTexture, true);
	//SetTextureRepeating(waterBumpTexture, true);
	SetTextureRepeating(sunTexture, true);
	SetTextureRepeating(treeTrunkTex, true);
	SetTextureRepeating(treeLeafTex, true);

	// generate post process frame buffer
	GeneratePostProcessPrepared();
	// generate defer render prepared
	GenerateDerferRenderPrepared();
	// generata cube shadow mapping;
	GenerateShadowCubeMapPrepared();

	// generate shader
	skyShader = new Shader(SHADERDIR"coursework/skyboxVertex.glsl", SHADERDIR"coursework/skyboxFragment.glsl");
	groundShader = new Shader(SHADERDIR"coursework/groundDeferVertex.glsl", SHADERDIR"coursework/groundDeferFragment.glsl");
	//waterShader = new Shader(SHADERDIR"coursework/waterVertex.glsl", SHADERDIR"coursework/waterFragment.glsl");
	betterWaterShader = new Shader(SHADERDIR"coursework/betterWaterVert.glsl", SHADERDIR"coursework/betterWaterFrag.glsl");
	sunSphereShader = new Shader(SHADERDIR"coursework/sunSphereVertex.glsl", SHADERDIR"coursework/sunSphereFragment.glsl");
	shadowCubeShader = new Shader(SHADERDIR"coursework/shadowCubeVertex.glsl", SHADERDIR"coursework/shadowCubeFragment.glsl");
	particleShader = new Shader(
		SHADERDIR"coursework/particle/vertex.glsl",
		SHADERDIR"coursework/particle/fragment.glsl",
		SHADERDIR"coursework/particle/geometry.glsl"
	);
	skinObjShader = new Shader(SHADERDIR"coursework/anim/SkinningVertex.glsl", SHADERDIR"coursework/anim/texturedFragment.glsl");

	nonePostProcessShader = new Shader(SHADERDIR"coursework/post/nonePostProcessVertex.glsl", SHADERDIR"coursework/post/nonePostProcessFragment.glsl");
	blurPostProcessShader = new Shader(SHADERDIR"coursework/post/blurPostProcessVertex.glsl", SHADERDIR"coursework/post/blurPostProcessFragment.glsl");
	gammaCorrectionPostProcessShader = new Shader(SHADERDIR"coursework/post/gammaPostProcessVertex.glsl",
													SHADERDIR"coursework/post/gammaPostProcessFragment.glsl");
	bloomBrightShader = new Shader(SHADERDIR"coursework/post/bloomBrightVert.glsl",
									SHADERDIR"coursework/post/bloomBrightFrag.glsl");
	bloomBlurShader = new Shader(SHADERDIR"coursework/post/bloomBlurVert.glsl",
									SHADERDIR"coursework/post/bloomBlurFrag.glsl");
	bloomFinalShader = new Shader(SHADERDIR"coursework/post/bloomFinalVert.glsl",
									SHADERDIR"coursework/post/bloomFinalFrag.glsl");

	dofShader = new Shader(SHADERDIR"coursework/post/dofVert.glsl", SHADERDIR"coursework/post/dofFrag.glsl");

	presentSceneShader = new Shader(SHADERDIR"coursework/post/presentSceneVertex.glsl", SHADERDIR"coursework/post/presentSceneFragment.glsl");

	lightsShader = new Shader(SHADERDIR"coursework/defer_render/lightsVert.glsl", SHADERDIR"coursework/defer_render/lightsFrag.glsl");
	deferRenderCombineShader = new Shader(SHADERDIR"coursework/defer_render/combineVert.glsl", 
			SHADERDIR"coursework/defer_render/combineFrag.glsl");

	shadowCubeMapShader = new Shader(
		SHADERDIR"coursework/shadow/point_shadow_depth_vert.glsl",
		SHADERDIR"coursework/shadow/point_shadow_depth_frag.glsl",
		SHADERDIR"coursework/shadow/point_shadow_depth_geom.glsl"
	);

	grassShader = new Shader(
		SHADERDIR"coursework/tree_grass/grass_vertex.glsl",
		SHADERDIR"coursework/tree_grass/grass_fragment.glsl"
	);
	treeShader = new Shader(
		SHADERDIR"coursework/tree_grass/tree_vertex.glsl",
		SHADERDIR"coursework/tree_grass/tree_fragment.glsl"
	);
	leafShader = new Shader(
		SHADERDIR"coursework/tree_grass/leaf_vertex.glsl",
		SHADERDIR"coursework/tree_grass/leaf_fragment.glsl"
	);

	// todo
	if (!skyShader->LoadSuccess()
		|| !groundShader->LoadSuccess()
		//|| !waterShader->LoadSuccess()
		|| !betterWaterShader->LoadSuccess()
		|| !sunSphereShader->LoadSuccess()
		|| !shadowCubeShader->LoadSuccess()
		|| !particleShader->LoadSuccess()
		|| !nonePostProcessShader->LoadSuccess()
		|| !blurPostProcessShader->LoadSuccess()
		|| !bloomBrightShader->LoadSuccess()
		|| !bloomBlurShader->LoadSuccess()
		|| !bloomFinalShader->LoadSuccess()
		|| !dofShader->LoadSuccess()
		|| !presentSceneShader->LoadSuccess()
		|| !lightsShader->LoadSuccess()
		|| !deferRenderCombineShader->LoadSuccess()
		|| !shadowCubeMapShader->LoadSuccess()
		|| !grassShader->LoadSuccess()
		|| !treeShader->LoadSuccess()
		|| !leafShader->LoadSuccess()
		|| !skinObjShader->LoadSuccess()
		) {
		return;
	}

	// add scene root
	root = new SceneNode();

	// test render obj
	//SceneNode* objNode = new SceneNode();
	//objNode->SetMesh(obj);
	//objNode->SetBoungdingRadius(std::max(groundSize.x, groundSize.z));
	//objNode->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
	//objNode->SetName("obj");
	//root->AddChild(objNode);

	// add ground to root
	SceneNode* groundNode = new SceneNode();
	groundNode->SetMesh(ground);
	groundNode->SetBoungdingRadius(std::max(groundSize.x, groundSize.z));
	groundNode->SetColour(Vector4(1.0, 1.0, 1.0, 1.0));
	groundNode->SetName("ground");
	root->AddChild(groundNode);

	// add water to root
	float rec_width = 0.5f;
	BetterWater wave = BetterWater();
	auto vertices_water = wave.init_plane(256, rec_width, 4.0f);
	indices_water = wave.init_indices(256);
	betterWater = wave.load_object(vertices_water, indices_water);

	SceneNode* waterNode = new SceneNode();
	waterNode->SetMesh(betterWater);
	waterNode->SetBoungdingRadius(std::max(groundSize.x, groundSize.z));
	waterNode->SetName("water");
	root->AddChild(waterNode);

	// add sun sphere to root
	SceneNode* sunNode = new SceneNode();
	sunNode->SetMesh(sunSphere);
	sunNode->SetBoungdingRadius(std::max(groundSize.x, groundSize.z));
	sunNode->SetName("sunSphere");
	root->AddChild(sunNode);

	// add cube  to root
	// show shadow effect
	SceneNode* shadowCubeNode = new SceneNode();
	shadowCubeNode->SetMesh(shadowCube);
	shadowCubeNode->SetBoungdingRadius(std::max(groundSize.x, groundSize.z));
	shadowCubeNode->SetName("shadowCube");
	shadowCubeNode->SetIsHaveShadow(true);
	root->AddChild(shadowCubeNode);

	// init grasses
	srand(time(NULL));
	float terrainWidth = ground->GetHeightMapSize().x;
	float terrainHeight = ground->GetHeightMapSize().z;
	for (size_t i = 0; i < 16 * 256; i += 256) {
		for (size_t j = 0; j < 16 * 256; j += 256) {
			Grasses.push_back(new Grass(300, 1, 25, i, j, 256, ground));
		}
	}
	SceneNode* grassesNode = new SceneNode();
	grassesNode->SetBoungdingRadius(std::max(groundSize.x, groundSize.z));
	grassesNode->SetName("grasses");
	grassesNode->SetIsHaveShadow(true);
	root->AddChild(grassesNode);

	// init trees
	int noTrees = 1;
	for (size_t i = 0; i < noTrees; i++)
	{
		Trees.push_back(new Tree(ground));
		Trees[i]->CreateTree();
	}
	SceneNode* treesNode = new SceneNode();
	treesNode->SetBoungdingRadius(std::max(groundSize.x, groundSize.z));
	treesNode->SetName("trees");
	treesNode->SetIsHaveShadow(true);
	root->AddChild(treesNode);

	// init skin eagle
	skinEagle = Mesh::LoadFromMeshFile("Eagle_Normal.msh");
	skinEagleAnim = new MeshAnimation("Eagle_Normal.anm");
	skinEagleMat = new MeshMaterial("Eagle_Normal.mat");

	for (int i = 0; i < skinEagle->GetSubMeshCount(); i++) {
		const MeshMaterialEntry* matEntry = skinEagleMat->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		skinEagleMatTextures.emplace_back(texID);
	}
	currentFrameForAnim = 0;
	frameTimeForAnim = 0.0f;

	SceneNode* eagleNode = new SceneNode();
	eagleNode->SetBoungdingRadius(std::max(groundSize.x, groundSize.z));
	eagleNode->SetMesh(skinEagle);
	eagleNode->SetName("eagle");
	eagleNode->SetIsHaveShadow(true);
	root->AddChild(eagleNode);

	// init flame emiiter
	flameParticleEmitter = new ParticleEmitter();
	flameParticleEmitter->SetParticleSize(1.0f);
	SetShaderParticleSize(flameParticleEmitter->GetParticleSize());

	projMatrix = defaultProjMatrix;;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	waterRotate = 0.0f;
	waterCycle = 0.0f;
	sceneTime = 0.0f;

	init = true;
}

Renderer::~Renderer(void)	{
	delete root;
	delete skybox;
	delete ground;
	//delete water;
	delete betterWater;
	delete sunSphere;
	delete shadowCube;
	delete skinEagle;
	delete skinEagleAnim;
	delete skinEagleMat;

	//delete obj;

	delete flameParticleEmitter;

	delete sunLight;

	delete groundShader;
	delete skyShader;
	//delete waterShader;
	delete betterWaterShader;
	delete sunSphereShader;
	delete particleShader;
	delete grassShader;
	delete treeShader;
	delete leafShader;
	delete skinObjShader;
	
	delete nonePostProcessShader;
	delete blurPostProcessShader;
	delete gammaCorrectionPostProcessShader;
	delete bloomBrightShader;
	delete bloomBlurShader;
	delete bloomFinalShader;
	delete dofShader;
	delete presentSceneShader;
	delete mainCamera;

	glDeleteTextures(1, &groundGrassTexture);
	glDeleteTextures(1, &groundGrassBumpTexture);
	glDeleteTextures(1, &groundGrassGlossTexture);
	glDeleteTextures(1, &groundSandTexture);
	glDeleteTextures(1, &groundSandBumpTexture);
	glDeleteTextures(1, &groundSandGlossTexture);
	glDeleteTextures(1, &betterWaterTexture);
	//glDeleteTextures(1, &waterTexture);
	//glDeleteTextures(1, &waterBumpTexture);
	glDeleteTextures(1, &sunTexture);

	delete shadowCubeMapShader;
	glDeleteFramebuffers(1, &shadowDepthCubeMapFBO);
	glDeleteTextures(1, &shadowDepthCubeMapTex);

	delete postProcessQuad;
	glDeleteTextures(2, postProcessBufferColourTex);
	glDeleteTextures(2, postProcessGammaBufferColourTex);
	glDeleteTextures(2, postProcessBloomBufferColourTex);
	glDeleteTextures(2, postProcessBloomBufferColourTex);
	glDeleteTextures(2, postProcessBloomBlurBufferColourTex);
	glDeleteTextures(2, postProcessBloomBrightBufferColourTex);
	glDeleteTextures(1, &postProcessDepthTex);
	glDeleteFramebuffers(1, &postProcessFBO); 
	glDeleteFramebuffers(1, &gammaPostProcessFBO);
	glDeleteFramebuffers(1, &bloomPostProcessFBO);
	glDeleteFramebuffers(1, &bloomBlurFBO);
	glDeleteFramebuffers(1, &bloomBrightFBO);
	glDeleteFramebuffers(1, &sceneFBO);

	delete deferRenderQuad;
	glDeleteFramebuffers(1, &derferRenderFBO);
	glDeleteTextures(1, &deferRenderBufferColorTex);
	glDeleteTextures(1, &deferRenderBufferNormalTex);
	glDeleteTextures(1, &deferRenderBufferDepthTex);
	glDeleteFramebuffers(1, &deferRenderLightsFBO);
	glDeleteTextures(1, &deferRenderLightsDepthTex);
	glDeleteTextures(1, &deferRenderLightsDiffuseTex);
	glDeleteTextures(1, &deferRenderLightsSpecularTex);
	glDeleteTextures(1, &deferRenderLightsSkyTex);
	glDeleteTextures(1, &deferRenderLightsParticleTex);
}

void Renderer::SwitchCameraAutoMove() {
	isCameraAutoMove = !isCameraAutoMove;
}

void Renderer::SwitchSunLightAutoMove() {
	isSunLightAutoMove = !isSunLightAutoMove;
}

void Renderer::UpdateScene(float dt, bool useSpotRenderer) {
	if (useSpotRenderer == true) {
		spotCamera->UpdateCamera(dt);
		viewMatrix = spotCamera->BuildViewMatrix();
	}
	else {
		sceneTime += dt;
		waterRotate += dt * 2.0f;
		waterCycle += dt * 0.5f;

		frameTimeForAnim -= dt * 2.0f;
		while (frameTimeForAnim < 0.0f) {
			currentFrameForAnim = (currentFrameForAnim + 1) % skinEagleAnim->GetFrameCount();
			frameTimeForAnim = 1.0 / skinEagleAnim->GetFrameRate();
		}

		if (isSunLightAutoMove) {
			SunlightAutoMove(sceneTime);
		}
		else {
			Vector3 groundSize = ground->GetHeightMapSize();
			sunLight->SetPosition(groundSize * Vector3(0.5f, 1.5f, 0.5f));
		}

		if (isCameraAutoMove) {
			CameraAutoMove(sceneTime);
		}

		mainCamera->UpdateCamera(dt);
		flameParticleEmitter->Update(dt);

		viewMatrix = mainCamera->BuildViewMatrix();
		projMatrix = defaultProjMatrix;
		frameFrustum.FromMatrix(projMatrix * viewMatrix);

		root->Update(dt);
	}
}

void Renderer::RenderScene(bool useSpotRenderer)	{

	if (useSpotRenderer == true) {
		DrawSpotLightScene();
	}
	else {
		BuildNodeLists(root);
		SortNodeLists();

		// init post process frame buffer;
		InitRenderToFrameBuffer();

		if (useSecondCamera) {
			glViewport(0, 0, 960, 540);
		}

		DrawShadowDepthCubeMap();

		DrawScenes();

		DrawLights();

		DrawSkyBox();

		DrawParticle();

		CombineDeferRenderBuffers();

		ClearNodeLists();

		DrawPostProcess();

		DrawPresentScene();

		if (useSecondCamera) {
			glViewport(960, 540, 1920, 1080);
			glClearColor(1.0f, 0.0f, 1.0f, 1);
			postProcessQuad->Draw();
		}
	}
}

void Renderer::DrawSpotLightScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(spotShader);

	glUniform1i(glGetUniformLocation(spotShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spotHeightMapTexture);

	glUniform3fv(glGetUniformLocation(spotShader->GetProgram(), "cameraPos"), 1, (float*)& spotCamera->GetPosition());
	viewMatrix = spotCamera->BuildViewMatrix();
	modelMatrix.ToIdentity();
	projMatrix = defaultProjMatrix;
	UpdateShaderMatrices();
	Vector3 spotHeightMapSize = spotHeightMap->GetHeightMapSize();

	SpotLight* sl1 = new SpotLight(spotHeightMapSize * Vector3(0.5f, 1.5f, 0.5f), Vector3(0.0f, -1.0f, 0.0f), Vector4(1, 1, 1, 1), 0.9f, 0.7f);
	SetShaderSpotLight(*sl1);
	spotHeightMap->Draw();

	SpotLight*  sl2 = new SpotLight(spotHeightMapSize * Vector3(0.8f, 1.5f, 0.8f), Vector3(0.0f, -1.0f, 0.0f), Vector4(1, 1, 1, 1), 0.9f, 0.7f);
	SetShaderSpotLight(*sl2);
	spotHeightMap->Draw();
}

void Renderer::SunlightAutoMove(float sceneTime) {
	Vector3 groundSize = ground->GetHeightMapSize();
	sunLight->SetPosition(Vector3(groundSize.x * 0.5 + cos(sceneTime) * (0.5 * groundSize.x + 100),
		groundSize.y * 0.5 + sin(sceneTime) * (0.5 * groundSize.z + 100),
		groundSize.z * 0.5
	));
}

void Renderer::CameraAutoMove(float sceneTime) {
	Vector3 groundSize = ground->GetHeightMapSize();
	mainCamera->SetPosition(Vector3(groundSize.x * 0.5 + cos(sceneTime) * 0.25 * groundSize.x,
		groundSize.y * 5.0,
		groundSize.z * 0.5 + sin(sceneTime) * 0.25 * groundSize.z
	));
	mainCamera->SetYaw(cos(sceneTime) * 90.0f);
	mainCamera->SetPitch(sin(sceneTime) * 45.0f -45.0f);
}

void Renderer::BuildNodeLists(SceneNode* from) {
	if (frameFrustum.InsideFrustum(*from)) {
		Vector3 dir = from->GetWorldTransform().GetPositionVector() - mainCamera->GetPosition();
		from->SetCameraDistance(Vector3::Dot(dir, dir));

		if (from->GetColour().w < 1.0f) {
			transparentNodeList.push_back(from);
		}
		else {
			nodeList.push_back(from);
		}
	}

	for (vector<SceneNode*>::const_iterator i = from->GetChildIteratorStart();
		i != from->GetChildIteratorEnd();
		i++
		) {
		BuildNodeLists((*i));
	}
}

void Renderer::SortNodeLists() {
	std::sort(transparentNodeList.rbegin(), transparentNodeList.rend(), SceneNode::CompareByCameraDistance);
	std::sort(nodeList.begin(), nodeList.end(), SceneNode::CompareByCameraDistance);
}

void Renderer::DrawScenes() {
	std::string t = "scene";
	DrawSceneNodes(t);
}

void Renderer::DrawNodes(std::string& type) {
	for (const auto& i : nodeList) {
		DrawNode(i, type);
	}
	for (const auto& i : transparentNodeList) {
		DrawNode(i, type);
	}
}

void Renderer::DrawNode(SceneNode* n, std::string& type) {
	// if type == "shadow"
	// render to shadow cube map
	if (type == "shadow") {
		DrawObjectShadow(n);
	}
	if (type == "scene") {
		// todo
		/*if (n->GetName() == "obj") {
			DrawOBJ(n);
		}*/
		if (n->GetName() == "ground") {
			DrawGround(n);
		}
		if (n->GetName() == "water") {
			//DrawWater(n);
			DrawBetterWater(n);
		}
		if (n->GetName() == "sunSphere") {
			DrawSunSphere(n);
		}
		if (n->GetName() == "shadowCube") {
			DrawShadowCube(n);
		}
		if (n->GetName() == "grasses") {
			DrawGrasses(n);
		}
		if (n->GetName() == "trees") {
			DrawTrees(n);
		}
		if (n->GetName() == "eagle") {
			DrawSkinEagle(n);
		}
	}
}

void Renderer::ClearNodeLists() {
	transparentNodeList.clear();
	nodeList.clear();
}

void Renderer::DrawSkyBox() {

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferRenderLightsFBO);
	//glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDepthMask(GL_FALSE);
	projMatrix = defaultProjMatrix;
	viewMatrix = mainCamera->BuildViewMatrix();

	BindShader(skyShader);

	glUniform1i(glGetUniformLocation(skyShader->GetProgram(), "cubeTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	glUniform1i(glGetUniformLocation(skyShader->GetProgram(), "depthTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferRenderBufferDepthTex);

	UpdateShaderMatrices();

	skybox->Draw();
	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawSunSphere(SceneNode* n) {
	glBindFramebuffer(GL_FRAMEBUFFER, derferRenderFBO);
	//glDepthMask(GL_FALSE);

	BindShader(sunSphereShader);
	projMatrix = defaultProjMatrix;
	modelMatrix = GenerateModelMatrix(n);

	glUniform1i(glGetUniformLocation(sunSphereShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	UpdateShaderMatrices();

	n->Draw(*this);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawShadowCube(SceneNode* n) {
	glBindFramebuffer(GL_FRAMEBUFFER, derferRenderFBO);

	BindShader(shadowCubeShader);
	glUniform1i(glGetUniformLocation(shadowCubeShader->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	modelMatrix = GenerateModelMatrix(n);
	projMatrix = defaultProjMatrix;

	UpdateShaderMatrices();
	n->Draw(*this);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawTrees(SceneNode* n) {
	glBindFramebuffer(GL_FRAMEBUFFER, derferRenderFBO);

	int noTrees = 1;
	Vector3 cameraPosition = mainCamera->GetPosition();
	Vector3 wind = Vector3(2, 0.5, 2);
	for (int i = 0; i < noTrees; i++) {

		Vector3 treeStartPosition = Trees[i]->GetStartPosition();
		int treeLodLevel = Tree::SelectTreeLod(cameraPosition, treeStartPosition);

		BindShader(treeShader);
		modelMatrix.ToIdentity();
		viewMatrix = mainCamera->BuildViewMatrix();
		projMatrix = defaultProjMatrix;

		glUniform1i(glGetUniformLocation(treeShader->GetProgram(), "treeTrunkTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, treeTrunkTex);

		glUniform3f(glGetUniformLocation(treeShader->GetProgram(), "cameraPos"), cameraPosition.x, cameraPosition.y, cameraPosition.z);
		glUniform3f(glGetUniformLocation(treeShader->GetProgram(), "wind"), wind.x, wind.y, wind.z);
		glUniform1f(glGetUniformLocation(treeShader->GetProgram(), "time"), cos(sceneTime));
		glUniform1f(glGetUniformLocation(treeShader->GetProgram(), "circlesPerBranch"), 6);

		UpdateShaderMatrices();
		Trees[i]->DrawTree(treeLodLevel);

		BindShader(leafShader);
		glUniform1i(glGetUniformLocation(leafShader->GetProgram(), "treeLeafTex"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, treeLeafTex);

		glUniform3f(glGetUniformLocation(leafShader->GetProgram(), "wind"), wind.x, wind.y, wind.z);
		glUniform1f(glGetUniformLocation(leafShader->GetProgram(), "time"), cos(sceneTime));
		glUniform1f(glGetUniformLocation(leafShader->GetProgram(), "circlesPerBranch"), 6);
		glUniform1f(glGetUniformLocation(leafShader->GetProgram(), "branchLevels"), 5);

		UpdateShaderMatrices();
		Trees[i]->DrawLeaves(treeLodLevel);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawGrasses(SceneNode* n) {
	glBindFramebuffer(GL_FRAMEBUFFER, derferRenderFBO);

	BindShader(grassShader);
	glUniform3fv(glGetUniformLocation(grassShader->GetProgram(), "cameraPos"), 0, (float*)&mainCamera->GetPosition());
	glUniform3fv(glGetUniformLocation(grassShader->GetProgram(), "wind"), 0, (float*)&Vector3(5, 1, 5));
	glUniform1f(glGetUniformLocation(grassShader->GetProgram(), "time"), cos(sceneTime * 5));
	float fRandX = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) / 20;
	float fRandY = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) / 20;
	float fRandZ = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX))) / 20;
	glUniform3fv(glGetUniformLocation(grassShader->GetProgram(), "random"), 0, (float*)&Vector3(fRandX, fRandY, fRandZ));

	modelMatrix.ToIdentity();
	projMatrix = defaultProjMatrix;

	int noGrassZones = Grasses.size();
	for (size_t i = 0; i < noGrassZones; i++) {
		int grassLodLevel = Grass::SelectGrassLod(mainCamera->GetPosition(), Grasses[i]->GetStartPosition(), 1000);
		UpdateShaderMatrices();
		if (Grasses[i]->GetStartPosition().y >= 0.6 * ground->GetHeightMapSize().y) {
			Grasses[i]->DrawGrass(grassLodLevel);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//void Renderer::DrawOBJ(SceneNode* n) {
//	glBindFramebuffer(GL_FRAMEBUFFER, derferRenderFBO);
//	BindShader(basicShader);
//	glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "grassDiffuseTex"), 0);
//	glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "grassBumpTex"), 1);
//	glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "grassGlossTex"), 2);
//	glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "sandDiffuseTex"), 3);
//	glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "sandBumpTex"), 4);
//	glUniform1i(glGetUniformLocation(basicShader->GetProgram(), "sandGlossTex"), 5);
//
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, groundGrassTexture);
//
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, groundGrassBumpTexture);
//
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_2D, groundGrassGlossTexture);
//
//	glActiveTexture(GL_TEXTURE3);
//	glBindTexture(GL_TEXTURE_2D, groundSandTexture);
//
//	glActiveTexture(GL_TEXTURE4);
//	glBindTexture(GL_TEXTURE_2D, groundSandBumpTexture);
//
//	glActiveTexture(GL_TEXTURE5);
//	glBindTexture(GL_TEXTURE_2D, groundSandGlossTexture);
//
//
//	modelMatrix = Matrix4::Scale(Vector3(10, 10, 10));
//	projMatrix = defaultProjMatrix;
//
//	UpdateShaderMatrices();
//	n->Draw(*this);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}

void Renderer::DrawSkinEagle(SceneNode* n) {
	glBindFramebuffer(GL_FRAMEBUFFER, derferRenderFBO);

	BindShader(skinObjShader);
	glUniform1i(glGetUniformLocation(skinObjShader->GetProgram(), "diffuseTex"), 0);

	modelMatrix = GenerateModelMatrix(n);
	projMatrix = defaultProjMatrix;

	UpdateShaderMatrices();

	vector<Matrix4> frameMatrices;
	const Matrix4* inBindPose = n->GetMesh()->GetInverseBindPose();
	const Matrix4* frameData = skinEagleAnim->GetJointData(currentFrameForAnim);

	for (unsigned int i = 0; i < n->GetMesh()->GetJointCount(); i++) {
		frameMatrices.emplace_back(frameData[i] * inBindPose[i]);
	}

	int j = glGetUniformLocation(skinObjShader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

	if (n->GetMesh()->GetJointCount()) {
		for (int i = 0; i < n->GetMesh()->GetSubMeshCount(); i++) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, skinEagleMatTextures[i]);
			n->GetMesh()->DrawSubMesh(i);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawGround(SceneNode* n) {

	glBindFramebuffer(GL_FRAMEBUFFER, derferRenderFBO);

	BindShader(groundShader);
	glUniform1i(glGetUniformLocation(groundShader->GetProgram(), "grassDiffuseTex"), 0);
	glUniform1i(glGetUniformLocation(groundShader->GetProgram(), "grassBumpTex"), 1);
	glUniform1i(glGetUniformLocation(groundShader->GetProgram(), "grassGlossTex"), 2);
	glUniform1i(glGetUniformLocation(groundShader->GetProgram(), "sandDiffuseTex"), 3);
	glUniform1i(glGetUniformLocation(groundShader->GetProgram(), "sandBumpTex"), 4);
	glUniform1i(glGetUniformLocation(groundShader->GetProgram(), "sandGlossTex"), 5);
	glUniform1f(glGetUniformLocation(groundShader->GetProgram(), "blendHeight"), 255);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundGrassTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, groundGrassBumpTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, groundGrassGlossTexture);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, groundSandTexture);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, groundSandBumpTexture);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, groundSandGlossTexture);


	modelMatrix.ToIdentity();
	projMatrix = defaultProjMatrix;

	UpdateShaderMatrices();
	n->Draw(*this);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetShaderParticleSize(float f) {
	glUniform1f(glGetUniformLocation(particleShader->GetProgram(), "particleSize"), f);
}

void Renderer::DrawParticle() {
	glBindFramebuffer(GL_FRAMEBUFFER, deferRenderLightsFBO);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	BindShader(particleShader);

	glUniform1i(glGetUniformLocation(particleShader->GetProgram(), "diffuseTex"), 0);
	flameParticleEmitter->SetParticleSize(1.0f);
	SetShaderParticleSize(flameParticleEmitter->GetParticleSize());

	projMatrix = defaultProjMatrix;
	Vector3 groundSize = ground->GetHeightMapSize();
	modelMatrix = Matrix4::Translation(groundSize * Vector3(0.5f, 0.8f, 0.5f))*Matrix4::Scale(Vector3(5,5,5));

	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	flameParticleEmitter->Draw();
	glDepthMask(GL_TRUE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawBetterWater(SceneNode* n) {
	glBindFramebuffer(GL_FRAMEBUFFER, derferRenderFBO);

	BindShader(betterWaterShader);
	glUniform1i(glGetUniformLocation(betterWaterShader->GetProgram(), "TexWater"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, betterWaterTexture);
	glUniform1i(glGetUniformLocation(betterWaterShader->GetProgram(), "skybox"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glUniform3fv(glGetUniformLocation(betterWaterShader->GetProgram(), "cameraPos"), 0, (float*)&mainCamera->GetPosition());
	glUniform1f(glGetUniformLocation(betterWaterShader->GetProgram(), "time"), sceneTime);
	glUniform1f(glGetUniformLocation(betterWaterShader->GetProgram(), "speed"), 2.0);
	glUniform1f(glGetUniformLocation(betterWaterShader->GetProgram(), "amount"), 0.01);
	glUniform1f(glGetUniformLocation(betterWaterShader->GetProgram(), "height"), 10.0);

	projMatrix = defaultProjMatrix;
	viewMatrix = mainCamera->BuildViewMatrix();
	modelMatrix = GenerateModelMatrix(n);
	UpdateShaderMatrices();
	n->Draw(*this);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//void Renderer::DrawWater(SceneNode* n) {
//	glBindFramebuffer(GL_FRAMEBUFFER, derferRenderFBO);
//	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//
//	BindShader(waterShader);
//	glUniform3fv(glGetUniformLocation(waterShader->GetProgram(), "cameraPos"), 0, (float*)&mainCamera->GetPosition());
//
//	glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "diffuseTex"), 0);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, waterTexture);
//	glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "bumpTex"), 1);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, waterBumpTexture);
//	glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "cubeTex"), 2);
//	glActiveTexture(GL_TEXTURE2);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
//
//	Vector3 hSize = ground->GetHeightMapSize();
//	//modelMatrix = Matrix4::Translation(hSize * 0.5f) * Matrix4::Scale(hSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));
//	modelMatrix = GenerateModelMatrix(n);
//	projMatrix = defaultProjMatrix;
//	textureMatrix = Matrix4::Translation(Vector3(waterCycle, 0.0f, waterCycle)) *
//		Matrix4::Scale(Vector3(10, 10, 10)) *
//		Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));
//	UpdateShaderMatrices();
//	n->Draw(*this);
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}

void Renderer::GenerateScreenTexture(GLuint& into, bool depth) {
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0,
		format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::GenerateDerferRenderPrepared() {
	deferRenderQuad = Mesh::GenerateQuad();
	glGenFramebuffers(1, &derferRenderFBO);
	glGenFramebuffers(1, &deferRenderLightsFBO);

	GLenum buffers[2] = {
		GL_COLOR_ATTACHMENT0 ,
		GL_COLOR_ATTACHMENT1
	};
	// Generate our scene depth texture...
	GenerateScreenTexture(deferRenderBufferDepthTex, true);
	GenerateScreenTexture(deferRenderBufferColorTex);
	GenerateScreenTexture(deferRenderBufferNormalTex);
	GenerateScreenTexture(deferRenderLightsDepthTex, true);
	GenerateScreenTexture(deferRenderLightsDiffuseTex);
	GenerateScreenTexture(deferRenderLightsSpecularTex);
	GenerateScreenTexture(deferRenderLightsSkyTex);
	GenerateScreenTexture(deferRenderLightsParticleTex);
	// And now attach them to our FBOs
	glBindFramebuffer(GL_FRAMEBUFFER, derferRenderFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, deferRenderBufferColorTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, deferRenderBufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, deferRenderBufferDepthTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE)
	{
		return;
	}

	GLenum buffers4[4] = {
		GL_COLOR_ATTACHMENT0 ,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
	};
	glBindFramebuffer(GL_FRAMEBUFFER, deferRenderLightsFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, deferRenderLightsDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
		GL_TEXTURE_2D, deferRenderLightsSpecularTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
		GL_TEXTURE_2D, deferRenderLightsSkyTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3,
		GL_TEXTURE_2D, deferRenderLightsParticleTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, deferRenderLightsDepthTex, 0);
	glDrawBuffers(4, buffers4);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) !=
		GL_FRAMEBUFFER_COMPLETE)
	{
		return;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawLights() {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, derferRenderFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferRenderLightsFBO);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, deferRenderLightsFBO);
	BindShader(lightsShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(lightsShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deferRenderBufferDepthTex);

	glUniform1i(glGetUniformLocation(lightsShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferRenderBufferNormalTex);

	glUniform3fv(glGetUniformLocation(lightsShader->GetProgram(), "cameraPos"), 1, (float*)&mainCamera->GetPosition());
	glUniform2f(glGetUniformLocation(lightsShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4  invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(lightsShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	// cube shadow mapping;
	glUniform1i(glGetUniformLocation(lightsShader->GetProgram(), "depthCubeMapTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowDepthCubeMapTex);

	glUniform1f(glGetUniformLocation(lightsShader->GetProgram(), "far_plane"), farPlane);

	UpdateShaderMatrices();
	SetShaderLight(*sunLight);
	sunSphere->Draw();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_TRUE);
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::CombineDeferRenderBuffers() {

	glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	BindShader(deferRenderCombineShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(deferRenderCombineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, deferRenderBufferColorTex);

	glUniform1i(glGetUniformLocation(deferRenderCombineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, deferRenderLightsDiffuseTex);

	glUniform1i(glGetUniformLocation(deferRenderCombineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, deferRenderLightsSpecularTex);

	glUniform1i(glGetUniformLocation(deferRenderCombineShader->GetProgram(), "skyTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, deferRenderLightsSkyTex);

	glUniform1i(glGetUniformLocation(deferRenderCombineShader->GetProgram(), "particleTex"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, deferRenderLightsParticleTex);

	glUniform1i(glGetUniformLocation(deferRenderCombineShader->GetProgram(), "depthTex"), 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, deferRenderLightsDepthTex);

	deferRenderQuad->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::InitRenderToFrameBuffer() {
	if (POST_PROCESS_TYPE[current_post_process_type] == "gamma") {
		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, postProcessDepthTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessGammaBufferColourTex[0], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ||
			!postProcessDepthTex ||
			!postProcessGammaBufferColourTex[0]
			) {
			return;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	}
	else if (POST_PROCESS_TYPE[current_post_process_type] == "bloom") {
		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, postProcessDepthTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBrightBufferColourTex[0], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ||
			!postProcessDepthTex ||
			!postProcessBloomBrightBufferColourTex[0]
			) {
			return;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, postProcessDepthTex, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBufferColourTex[0], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ||
			!postProcessDepthTex ||
			!postProcessBufferColourTex[0]
			) {
			return;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Renderer::GeneratePostProcessPrepared() {
	postProcessQuad = Mesh::GenerateQuad();
	glGenTextures(1, &postProcessDepthTex);
	glBindTexture(GL_TEXTURE_2D, postProcessDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

	for (int i = 0; i < 2; i++) {
		glGenTextures(1, &postProcessBufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, postProcessBufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glGenTextures(1, &postProcessGammaBufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, postProcessGammaBufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glGenTextures(1, &postProcessBloomBlurBufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, postProcessBloomBlurBufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

		glGenTextures(1, &postProcessBloomBrightBufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, postProcessBloomBrightBufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glGenTextures(2, postProcessBloomBufferColourTex);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, postProcessBloomBufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, postProcessBloomBufferColourTex[i], 0);
	}
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);


	glGenFramebuffers(1, &sceneFBO);
	glGenFramebuffers(1, &postProcessFBO);
	glGenFramebuffers(1, &gammaPostProcessFBO);
	glGenFramebuffers(1, &bloomPostProcessFBO);
	glGenFramebuffers(1, &bloomBlurFBO);
	glGenFramebuffers(1, &bloomBrightFBO);
}

void Renderer::ChooseFrameTexture() {
	glActiveTexture(GL_TEXTURE0);
	if (POST_PROCESS_TYPE[current_post_process_type] == "gamma") {
		glBindTexture(GL_TEXTURE_2D, postProcessGammaBufferColourTex[0]);
	}
	else if (POST_PROCESS_TYPE[current_post_process_type] == "bloom") {
		glBindTexture(GL_TEXTURE_2D, postProcessBloomBufferColourTex[0]);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, postProcessBufferColourTex[0]);
	}
}

void Renderer::DrawPostProcess() {
	if (POST_PROCESS_TYPE[current_post_process_type] == "none") {
		DrawNonePostProcess();
	}
	if (POST_PROCESS_TYPE[current_post_process_type] == "blur") {
		DrawBlurPostProcess();
	}
	if (POST_PROCESS_TYPE[current_post_process_type] == "gamma") {
		DrawGammaPostProcess();
	}
	if (POST_PROCESS_TYPE[current_post_process_type] == "bloom") {
		DrawBloomPostProcess();
	}
	if (POST_PROCESS_TYPE[current_post_process_type] == "dof") {
		DrawDofPostProcess();
	}
}

void Renderer::DrawDofPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(dofShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(dofShader->GetProgram(), "sceneTex"), 0);
	glBindTexture(GL_TEXTURE_2D, postProcessBufferColourTex[0]);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(dofShader->GetProgram(), "depthTex"), 1);
	glBindTexture(GL_TEXTURE_2D, deferRenderBufferDepthTex);
	glUniform1i(glGetUniformLocation(dofShader->GetProgram(), "reverse"), 1);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBufferColourTex[1], 0);
	postProcessQuad->Draw();

	// swap colour buffer and do the  second  blur  pass
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(dofShader->GetProgram(), "sceneTex"), 0);
	glBindTexture(GL_TEXTURE_2D, postProcessBufferColourTex[1]);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(dofShader->GetProgram(), "depthTex"), 1);
	glBindTexture(GL_TEXTURE_2D, deferRenderBufferDepthTex);
	glUniform1i(glGetUniformLocation(dofShader->GetProgram(), "reverse"), 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBufferColourTex[0], 0);
	postProcessQuad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawPresentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(presentSceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	ChooseFrameTexture();
	glUniform1i(glGetUniformLocation(presentSceneShader->GetProgram(), "diffuseTex"), 0);
	postProcessQuad->Draw();
}

void Renderer::DrawNonePostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(nonePostProcessShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(nonePostProcessShader->GetProgram(), "sceneTex"), 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBufferColourTex[1], 0);
	glBindTexture(GL_TEXTURE_2D, postProcessBufferColourTex[0]);
	postProcessQuad->Draw();

	// swap colour buffer and do the  second  blur  pass
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBufferColourTex[0], 0);
	glBindTexture(GL_TEXTURE_2D, postProcessBufferColourTex[1]);
	postProcessQuad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawBlurPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(blurPostProcessShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(blurPostProcessShader->GetProgram(), "sceneTex"), 0);
	for (int i = 0; i < 10; i++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(blurPostProcessShader->GetProgram(), "isVertical"), 0);
		glBindTexture(GL_TEXTURE_2D, postProcessBufferColourTex[0]);
		postProcessQuad->Draw();

		// swap colour buffer and do the  second  blur  pass
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBufferColourTex[0], 0);
		glUniform1i(glGetUniformLocation(blurPostProcessShader->GetProgram(), "isVertical"), 1);
		glBindTexture(GL_TEXTURE_2D, postProcessBufferColourTex[1]);
		postProcessQuad->Draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawGammaPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, gammaPostProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessGammaBufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(gammaCorrectionPostProcessShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(gammaCorrectionPostProcessShader->GetProgram(), "sceneTex"), 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, postProcessGammaBufferColourTex[1], 0);
	glBindTexture(GL_TEXTURE_2D, postProcessGammaBufferColourTex[0]);
	postProcessQuad->Draw();

	// swap colour buffer and do the  second  blur  pass
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, postProcessGammaBufferColourTex[0], 0);
	glBindTexture(GL_TEXTURE_2D, postProcessGammaBufferColourTex[1]);
	postProcessQuad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawBloomPostProcess() {
	// draw origin to bloomFinalFBO
	glBindFramebuffer(GL_FRAMEBUFFER, bloomPostProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(nonePostProcessShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(nonePostProcessShader->GetProgram(), "sceneTex"), 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBufferColourTex[1], 0);
	glBindTexture(GL_TEXTURE_2D, postProcessBloomBrightBufferColourTex[0]);
	postProcessQuad->Draw();

	// swap colour buffer and do the  second  blur  pass
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBufferColourTex[0], 0);
	glBindTexture(GL_TEXTURE_2D, postProcessBloomBrightBufferColourTex[0]);
	postProcessQuad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);

	// draw bright 
	glBindFramebuffer(GL_FRAMEBUFFER, bloomBrightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBrightBufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(bloomBrightShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(bloomBrightShader->GetProgram(), "sceneTex"), 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBrightBufferColourTex[1], 0);
	glBindTexture(GL_TEXTURE_2D, postProcessBloomBrightBufferColourTex[0]);
	postProcessQuad->Draw();

	// swap colour buffer and do the  second  blur  pass
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBrightBufferColourTex[0], 0);
	glBindTexture(GL_TEXTURE_2D, postProcessBloomBrightBufferColourTex[1]);
	postProcessQuad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);

	// transfer data ====================================
	glBindFramebuffer(GL_FRAMEBUFFER, bloomBlurFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBlurBufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(nonePostProcessShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(nonePostProcessShader->GetProgram(), "sceneTex"), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBlurBufferColourTex[1], 0);
	glUniform1i(glGetUniformLocation(nonePostProcessShader->GetProgram(), "reverse"), 1);
	glBindTexture(GL_TEXTURE_2D, postProcessBloomBrightBufferColourTex[1]);
	postProcessQuad->Draw();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBlurBufferColourTex[0], 0);
	glUniform1i(glGetUniformLocation(nonePostProcessShader->GetProgram(), "reverse"), 0);
	glBindTexture(GL_TEXTURE_2D, postProcessBloomBrightBufferColourTex[0]);
	postProcessQuad->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);

	// draw bright blur
	glBindFramebuffer(GL_FRAMEBUFFER, bloomBlurFBO);
	bool horizontal = true;
	BindShader(bloomBlurShader);
	for (unsigned int i = 0; i < 10; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBlurBufferColourTex[horizontal], 0);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(bloomBlurShader->GetProgram(), "image"), 0);
		glBindTexture(GL_TEXTURE_2D, postProcessBloomBlurBufferColourTex[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
		glUniform1i(glGetUniformLocation(bloomBlurShader->GetProgram(), "horizontal"), horizontal);
		postProcessQuad->Draw();
		horizontal = !horizontal;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// final combine
	glBindFramebuffer(GL_FRAMEBUFFER, bloomPostProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBufferColourTex[1], 0);
	BindShader(bloomFinalShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	textureMatrix.ToIdentity();
	UpdateShaderMatrices();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBufferColourTex[1], 0);
	glUniform1i(glGetUniformLocation(bloomFinalShader->GetProgram(), "scene"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postProcessBloomBufferColourTex[0]);
	glUniform1i(glGetUniformLocation(bloomFinalShader->GetProgram(), "bloomBlur"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, postProcessBloomBlurBufferColourTex[0]);
	glUniform1f(glGetUniformLocation(bloomFinalShader->GetProgram(), "exposure"), 1.0f);
	glUniform1i(glGetUniformLocation(bloomFinalShader->GetProgram(), "reverse"), 0);
	postProcessQuad->Draw();

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessBloomBufferColourTex[0], 0);
	glUniform1i(glGetUniformLocation(bloomFinalShader->GetProgram(), "scene"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, postProcessBloomBufferColourTex[1]);
	glUniform1i(glGetUniformLocation(bloomFinalShader->GetProgram(), "bloomBlur"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, postProcessBloomBlurBufferColourTex[0]);
	glUniform1f(glGetUniformLocation(bloomFinalShader->GetProgram(), "exposure"), 1.0f);
	glUniform1i(glGetUniformLocation(bloomFinalShader->GetProgram(), "reverse"), 1);
	postProcessQuad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GenerateShadowCubeMapPrepared() {
	glGenFramebuffers(1, &shadowDepthCubeMapFBO);
	glGenTextures(1, &shadowDepthCubeMapTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, shadowDepthCubeMapTex);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < 6; ++i) {
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
			0, 
			GL_DEPTH_COMPONENT, 
			SHADOW_MAP_WIDTH,
			SHADOW_MAP_HEIGHT,
			0, 
			GL_DEPTH_COMPONENT, 
			GL_FLOAT, 
			NULL
		);
	}
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthCubeMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthCubeMapTex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		return;
	}
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::DrawShadowDepthCubeMap() {
	std::string t = "shadow";
	DrawSceneNodes(t);
}

void Renderer::DrawSceneNodes(std::string& type) {
	if (type == "shadow") {
		glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthCubeMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		DrawNodes(type);
		glViewport(0, 0, width, height);
		glDisable(GL_CULL_FACE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else if (type == "scene") {
		glBindFramebuffer(GL_FRAMEBUFFER, derferRenderFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		DrawNodes(type);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Renderer::DrawObjectShadow(SceneNode* n) {
	if (n->GetIsHaveShadow() == false) {
		return;
	}

	Vector3 lightPos = sunLight->GetPosition();
	Vector3 groundSize = ground->GetHeightMapSize();

	if (lightPos.y < groundSize.y) {
		return;
	}

	std::vector<Matrix4> shadowTransforms = GenerateShadowTransformsMatrix(lightPos, farPlane);

	//glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthCubeMapFBO);
	//glClear(GL_DEPTH_BUFFER_BIT);
	BindShader(shadowCubeMapShader);
	for (unsigned int i = 0; i < 6; ++i) {
		glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapShader->GetProgram(), ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, false, shadowTransforms[i].values);
	}
	glUniform1f(glGetUniformLocation(shadowCubeMapShader->GetProgram(), "far_plane"), farPlane);
	glUniform3fv(glGetUniformLocation(shadowCubeMapShader->GetProgram(), "lightPos"), 1, (float*)&lightPos);

	Matrix4 model = GenerateModelMatrix(n);
	glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapShader->GetProgram(), "model"), 1, false, model.values);

	if (n->GetName() == "trees") {
		int noTrees = 1;
		Vector3 pos = sunLight->GetPosition();
		for (int i = 0; i < noTrees; i++) {
			Vector3 treeStartPosition = Trees[i]->GetStartPosition();
			int treeLodLevel = Tree::SelectTreeLod(pos, treeStartPosition);
			Trees[i]->DrawTree(treeLodLevel);
			Trees[i]->DrawLeaves(treeLodLevel);
		}
	}
	/*else if (n->GetName() == "grasses") {
		int noGrassZones = Grasses.size();
		for (size_t i = 0; i < noGrassZones; i++) {
			int grassLodLevel = Grass::SelectGrassLod(mainCamera->GetPosition(), Grasses[i]->GetStartPosition(), 1000);
			UpdateShaderMatrices();
			if (Grasses[i]->GetStartPosition().y >= 0.6 * ground->GetHeightMapSize().y) {
				Grasses[i]->DrawGrass(grassLodLevel);
			}
		}
	}*/
	else if (n->GetName() == "eagle") {
		glUniform1i(glGetUniformLocation(shadowCubeMapShader->GetProgram(), "type"), 1);
		glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapShader->GetProgram(), "projMatrix"), 1, false, defaultProjMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(shadowCubeMapShader->GetProgram(), "viewMatrix"), 1, false, mainCamera->BuildViewMatrix().values);
		vector<Matrix4> frameMatrices;
		const Matrix4* inBindPose = n->GetMesh()->GetInverseBindPose();
		const Matrix4* frameData = skinEagleAnim->GetJointData(currentFrameForAnim);
		for (unsigned int i = 0; i < n->GetMesh()->GetJointCount(); i++) {
			frameMatrices.emplace_back(frameData[i] * inBindPose[i]);
		}
		int j = glGetUniformLocation(shadowCubeMapShader->GetProgram(), "joints");
		glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
		if (n->GetMesh()->GetJointCount()) {
			for (int i = 0; i < n->GetMesh()->GetSubMeshCount(); i++) {
				n->GetMesh()->DrawSubMesh(i);
			}
		}
	}
	else {
		glUniform1i(glGetUniformLocation(shadowCubeMapShader->GetProgram(), "type"), 0);
		n->Draw(*this);
	}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Matrix4 Renderer::GenerateModelMatrix(SceneNode* n) {
	Vector3 groundSize = ground->GetHeightMapSize();
	if (n->GetName() == "ground") {

	}
	if (n->GetName() == "water") {
		return Matrix4::Translation(Vector3(ground->GetHeightMapSize() * Vector3(0.5, 0.4, 0.5))) * Matrix4::Scale(Vector3(32, 1.5, 32));
		//return Matrix4::Translation(Vector3(groundSize.x * 0.5f, groundSize.y * 0.4f, groundSize.z * 0.5f)) * Matrix4::Scale(groundSize * 0.5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));
	}
	if (n->GetName() == "sunSphere") {
		if (isSunLightAutoMove) {
			return Matrix4::Translation(Vector3(groundSize.x * 0.5 + cos(sceneTime) * (0.5 * groundSize.x + 100),
				groundSize.y * 0.5 + sin(sceneTime) * ( 0.5 * groundSize.z + 100),
				groundSize.z * 0.5
			)) * Matrix4::Scale(Vector3(100, 100, 100));
		}
		else {
			return Matrix4::Translation(groundSize * Vector3(0.5f, 1.5f, 0.5f)) * Matrix4::Scale(Vector3(100, 100, 100));
		}
	}
	if (n->GetName() == "shadowCube") {
		return Matrix4::Translation(groundSize * Vector3(0.5f, 1.0f, 0.5f) + Vector3(0, 0, 200.0f)) * Matrix4::Scale(Vector3(50, 100, 50));
	}
	if (n->GetName() == "trees") {
		modelMatrix.ToIdentity();
		return modelMatrix;
	}
	/*if (n->GetName() == "grasses") {
		modelMatrix.ToIdentity();
		return modelMatrix;
	}*/
	if (n->GetName() == "eagle") {
		return Matrix4::Translation(groundSize * Vector3(0.5f, 0.5f, 0.5f) + Vector3(200.0f, 0, 100.0f)) * Matrix4::Scale(Vector3(20, 20, 20));
	}
}

std::vector<Matrix4> Renderer::GenerateShadowTransformsMatrix(Vector3 lightPos, float farPlane) {
	float near_plane = 1.0f;
	float far_plane = farPlane;
	Matrix4 shadowProj = Matrix4::Perspective(1.0f, farPlane, (float)SHADOW_MAP_WIDTH / (float)SHADOW_MAP_HEIGHT, 90.0f);
	std::vector<Matrix4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * Matrix4::BuildViewMatrix(lightPos, lightPos + Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f)));
	return shadowTransforms;
}