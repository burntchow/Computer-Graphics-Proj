/**
This application renders a textured mesh that was loaded with Assimp.
*/

#include <iostream>
#include <memory>
#include <glad/glad.h>

#include "Mesh3D.h"
#include "Object3D.h"
#include "AssimpImport.h"
#include "Animator.h"
#include "ShaderProgram.h"
#include <unordered_set>
#include <glm/gtx/string_cast.hpp>
#include <SFML/Audio.hpp> 

#include "RotationAnimation.h"
#include "TranslationAnimation.h"
#include "BezierTranslationAnimation.h"
#include "PauseAnimation.h"

#define _USE_MATH_DEFINES 
#include <math.h>  

void printPosition(const glm::vec3& position) {
	// Manually format the glm::vec3 object for printing
	std::cout << "Position: (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void printBirdVelocity(const Object3D& bird) {
	// Get the bird's velocity
	glm::vec3 velocity = bird.getVelocity();

	// Print the bird's velocity
	std::cout << "Bird's velocity: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")" << std::endl;
}

bool isBirdInMotion(const glm::vec3& velocity, float threshold = 0.3f) {
	// Check if the length of the bird's velocity is greater than the threshold
	return glm::length(velocity) > threshold;
}

/**
 * @brief Defines a collection of objects that should be rendered with a specific shader program.
 */
struct Scene {
	ShaderProgram defaultShader;
	std::vector<Object3D> objects;
	std::vector<Animator> animators;
};

/**
 * @brief Constructs a shader program that renders textured meshes in the Phong reflection model.
 * The shaders used here are incomplete; see their source codes.
 * @return 
 */
ShaderProgram phongLighting() {
	ShaderProgram program;
	try {
		program.load("shaders/light_perspective.vert", "shaders/lighting.frag");
	}
	catch (std::runtime_error& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		exit(1);
	}
	return program;
}

/**
 * @brief Constructs a shader program that renders textured meshes without lighting.
 */
ShaderProgram textureMapping() {
	ShaderProgram program;
	try {
		program.load("shaders/texture_perspective.vert", "shaders/texturing.frag");
	}
	catch (std::runtime_error& e) {
		std::cout << "ERROR: " << e.what() << std::endl;
		exit(1);
	}
	return program;
}

/**
 * @brief Loads an image from the given path into an OpenGL texture.
 */
Texture loadTexture(const std::filesystem::path& path, const std::string& samplerName = "baseTexture") {
	sf::Image i;
	i.loadFromFile(path.string());
	return Texture::loadImage(i, samplerName);
}

/**
 * @brief Loading SKIPPER FROM PENGUINS OF MADAGASCAR
 */
Scene skipper() { //works lmao 
	auto skipper = assimpLoad("models/Skipper/Skipper.obj", true);
	skipper.move(glm::vec3(0.2, -1, 0));

	return Scene{
		phongLighting(),
		{skipper}
	};
}



/**
 * @brief Square for the grass ground floor thing 
 */
Scene Ground() { // works ! 
	// load in texture
	auto grass_texture = loadTexture("textures/grass-texture-seamless/153_artificial green grass texture-seamless.jpg");
	std::vector<Texture> textures; 
	textures.push_back(grass_texture); // into vector for square

	//auto grass_mesh = Mesh3D::triangle(grass_texture);
	auto grass_mesh = Mesh3D::square(textures);

	std::vector<Mesh3D> meshes; 
	meshes.push_back(grass_mesh);

	auto ground = Object3D(std::move(meshes));
	ground.rotate(glm::vec3(M_PI/2,0, 0));
	ground.move(glm::vec3(0,-1,0));
	ground.grow(glm::vec3(10,10,10));

	return Scene{
		phongLighting(),
		{ground}
	};
}



/**
 * @brief Wood pallet for fort 
 */
Scene WoodPallet() { // works!
	auto pallet = assimpLoad("models/wood_pallet/scene.gltf", true);
	pallet.move(glm::vec3(0.2, -2, -10));
	pallet.rotate(glm::vec3(0, M_PI, 0)); // rotate 180 degrees -> pi

	return Scene{
		phongLighting(),
		{pallet}
	};
}



/**
 * @brief Slingshot
 */
Scene Slingshot() { // works!
	auto slingshot = assimpLoad("models/scout_slingshot_from_secret_neighbor/scene.gltf", true);
	slingshot.move(glm::vec3(0.2, -0.4, -3));
	slingshot.rotate(glm::vec3(0, M_PI, 0)); // rotate 180 degrees -> pi

	return Scene{
		phongLighting(),
		{slingshot}
	};
}



/**
 * @brief PIG
 */
Scene Pig() { // works ! 
	auto pig = assimpLoad("models/hiberworld_minion_pig/scene.gltf", true);
	pig.move(glm::vec3(0.2, -0.4, -10));
	pig.rotate(glm::vec3(0, M_PI, 0)); // rotate 180 degrees -> pi

	return Scene{
		phongLighting(),
		{pig}
	};
}


/**
 * @brief RED BIRD 
 */
Scene RedBird() { // works ! 
	auto redBird = assimpLoad("models/angry_bird_red/scene.gltf", true);
	redBird.move(glm::vec3(0.4, -0.4, -20));
	redBird.grow(glm::vec3(0.5,0.5,0.5));
	//pig.rotate(glm::vec3(0, M_PI, 0)); // rotate 180 degrees -> pi

	return Scene{
		phongLighting(),
		{redBird}
	};
}

// doesnt work yet :C
Scene Crate() { // test adding texture to object
	auto wood_texture = loadTexture("models/27-crates_obj/Texture_512/crate_Smallfullopen_LOD0_AlbedoTransparency.jpg");
	
	auto crate = assimpLoad("models/27-crates_obj/Crate.obj", true);
	crate.addTexture(wood_texture);

	crate.move(glm::vec3(0.2, -0.4, -20));

	return Scene{
		textureMapping(),
		{crate}
	};
}

Object3D touchGrass() {

	//auto grass_texture = loadTexture("textures/sky.png"); // does sky png work - yes 
	auto grass_texture = loadTexture("textures/grass-texture-seamless/153_artificial green grass texture-seamless.jpg");
	std::vector<Texture> textures;
	textures.push_back(grass_texture); // into vector for square

	auto grass_mesh = Mesh3D::square(textures);

	std::vector<Mesh3D> meshes;
	meshes.push_back(grass_mesh);

	auto ground = Object3D(std::move(meshes));

	return ground; 
}

Object3D iCanTouchTheSky() {
	auto sky_texture = loadTexture("textures/sky.png");
	std::vector<Texture> textures;
	textures.push_back(sky_texture); // into vector for rectangle 

	auto sky_mesh = Mesh3D::square(textures);

	std::vector<Mesh3D> meshes;
	meshes.push_back(sky_mesh);

	auto sky = Object3D(std::move(meshes));

	return sky; 
}

Scene testScene() {

	// Needs a background eventually *** 
	auto sky1 = iCanTouchTheSky();
	//sky.rotate();
	//sky.rotate(glm::vec3(M_PI / 2, 0, 0)); 
	sky1.move(glm::vec3(-40, 12, -9.5));
	sky1.grow(glm::vec3(60, 40, 40)); 

	auto sky2 = iCanTouchTheSky();
	sky2.move(glm::vec3(20, 12, -9.5));
	sky2.grow(glm::vec3(60, 40, 40));


	// grass ground  ===================================================================
 
	auto ground1 = touchGrass();
	ground1.rotate(glm::vec3(M_PI / 2, 0, 0));
	ground1.move(glm::vec3(0, -1, 0));
	ground1.grow(glm::vec3(20, 20, 20));

	auto ground2 = touchGrass(); 
	ground2.rotate(glm::vec3(M_PI / 2, 0, 0)); 
	ground2.move(glm::vec3(-20, -1, 0)); 
	ground2.grow(glm::vec3(20, 20, 20)); 
	 
	auto ground3 = touchGrass(); 
	ground3.rotate(glm::vec3(M_PI / 2, 0, 0)); 
	ground3.move(glm::vec3(-40, -1, 0)); 
	ground3.grow(glm::vec3(20, 20, 20)); 

	auto ground4 = touchGrass();
	ground4.rotate(glm::vec3(M_PI / 2, 0, 0));
	ground4.move(glm::vec3(20, -1, 0));
	ground4.grow(glm::vec3(20, 20, 20));

	auto ground5 = touchGrass();
	ground5.rotate(glm::vec3(M_PI / 2, 0, 0));
	ground5.move(glm::vec3(-60, -1, 0));
	ground5.grow(glm::vec3(20, 20, 20));

	auto ground6 = touchGrass();
	ground6.rotate(glm::vec3(M_PI / 2, 0, 0));
	ground6.move(glm::vec3(40, -1, 0));
	ground6.grow(glm::vec3(20, 20, 20));

	// wood pallets ====================================================================

	auto base_pallet_left = assimpLoad("models/wood_pallet/scene.gltf", true);
	base_pallet_left.move(glm::vec3(0.3, -0.9, -1));
	base_pallet_left.grow(glm::vec3(0.5, 0.5, 0.5));

	auto base_pallet_right = assimpLoad("models/wood_pallet/scene.gltf", true);
	base_pallet_right.move(glm::vec3(5.5, -0.9, -1));
	base_pallet_right.grow(glm::vec3(0.5, 0.5, 0.5));

	auto low_pallet_left = assimpLoad("models/wood_pallet/scene.gltf", true);
	low_pallet_left.move(glm::vec3(1, 2, -1)); 
	low_pallet_left.rotate(glm::vec3(0,0, M_PI/2));
	low_pallet_left.grow(glm::vec3(0.5, 0.5, 0.5));

	auto low_pallet_right = assimpLoad("models/wood_pallet/scene.gltf", true);
	low_pallet_right.move(glm::vec3(5.2, 2, -1));
	low_pallet_right.rotate(glm::vec3(0, 0, M_PI / 2));
	low_pallet_right.grow(glm::vec3(0.5, 0.5, 0.5));

	auto mid_pallet = assimpLoad("models/wood_pallet/scene.gltf", true);
	mid_pallet.move(glm::vec3(2.8, 4.32, -1));
	mid_pallet.grow(glm::vec3(0.5, 0.5, 0.5));

	auto up_pallet_left = assimpLoad("models/wood_pallet/scene.gltf", true); 
	up_pallet_left.move(glm::vec3(1.8, 7.2, -1));
	up_pallet_left.rotate(glm::vec3(0, 0, M_PI / 2));
	up_pallet_left.grow(glm::vec3(0.5, 0.5, 0.5));

	auto up_pallet_right = assimpLoad("models/wood_pallet/scene.gltf", true); 
	up_pallet_right.move(glm::vec3(4.5, 7.2, -1));
	up_pallet_right.rotate(glm::vec3(0, 0, M_PI / 2));
	up_pallet_right.grow(glm::vec3(0.5, 0.5, 0.5));

	auto up_mid_pallet = assimpLoad("models/wood_pallet/scene.gltf", true);
	up_mid_pallet.move(glm::vec3(2.8, 9.57, -1));
	up_mid_pallet.grow(glm::vec3(0.4, 0.4, 0.4));

	// PIG =========================================================================

	auto pig = assimpLoad("models/hiberworld_minion_pig/scene.gltf", true);
	pig.move(glm::vec3(2.9, 5.5, -1));
	pig.rotate(glm::vec3(0, M_PI, 0)); // rotate 180 degrees -> pi
	pig.grow(glm::vec3(0.3,0.3,0.3));

	// BIRDS =======================================================================
	auto bird3 = assimpLoad("models/angry_bird_red/scene.gltf", true); //leftmost
	bird3.move(glm::vec3(-30, 0.4, -1));
	bird3.rotate(glm::vec3(0, M_PI / 2, 0));
	bird3.grow(glm::vec3(0.07, 0.07, 0.07));

	auto bird2 = assimpLoad("models/angry_bird_red/scene.gltf", true); //mid
	bird2.move(glm::vec3(-33, 0.4, -1));
	bird2.rotate(glm::vec3(0, M_PI / 2, 0));
	bird2.grow(glm::vec3(0.07, 0.07, 0.07));

	auto bird1 = assimpLoad("models/angry_bird_red/scene.gltf", true); //rightmost 
	bird1.move(glm::vec3(-36, 0.4, -1));
	bird1.rotate(glm::vec3(0, M_PI / 2, 0));
	bird1.grow(glm::vec3(0.07, 0.07, 0.07));

	// Question about birds - How to apply simulation to one bird? 
	// --> Get bird individually in main? (if you wanted to make it fly)  


	// SLINGSHOT ===================================================================

	auto slingshot = assimpLoad("models/scout_slingshot_from_secret_neighbor/scene.gltf", true);
	slingshot.move(glm::vec3(-46, 0.3, -3));
	slingshot.rotate(glm::vec3(0, M_PI/2, 0));
	slingshot.grow(glm::vec3(15, 15, 15));

	// Question about slingshot - how to tilt back slingshot with bird in it? Heirarchical?
	// If make child of slingshot, how to release bird to go flying? addChild -> animation -> removeChild

	// EGG =========================================================================

	auto egg1 = assimpLoad("models/egg/scene.gltf", true);
	egg1.move(glm::vec3(26, 0.5, -3));
	egg1.grow(glm::vec3(0.3, 0.3, 0.3));

	auto egg2 = assimpLoad("models/egg/scene.gltf", true);
	egg2.move(glm::vec3(28, 0.5, -5));
	egg2.grow(glm::vec3(0.25, 0.25, 0.25));

	auto egg3 = assimpLoad("models/egg/scene.gltf", true);
	egg3.move(glm::vec3(24, 0.5, -5));
	egg3.grow(glm::vec3(0.25, 0.25, 0.25));


	// Objects =====================================================================

	std::vector<Object3D> objects;
	objects.push_back(std::move(slingshot));
	objects.push_back(std::move(bird1));
	objects.push_back(std::move(bird2));
	objects.push_back(std::move(bird3));
	objects.push_back(std::move(ground1));
	objects.push_back(std::move(ground2));
	objects.push_back(std::move(ground3));
	objects.push_back(std::move(ground4));
	objects.push_back(std::move(ground5));
	objects.push_back(std::move(ground6));
	objects.push_back(std::move(base_pallet_left));
	objects.push_back(std::move(base_pallet_right));
	objects.push_back(std::move(low_pallet_left)); 
	objects.push_back(std::move(low_pallet_right));
	objects.push_back(std::move(mid_pallet));
	objects.push_back(std::move(up_pallet_left));
	objects.push_back(std::move(up_pallet_right));
	objects.push_back(std::move(up_mid_pallet));
	objects.push_back(std::move(pig));
	objects.push_back(std::move(sky1));
	objects.push_back(std::move(sky2));
	objects.push_back(std::move(egg1));
	objects.push_back(std::move(egg2));
	objects.push_back(std::move(egg3));


	// Starting Animation ==========================================================

	Animator loadBird; 
	// bird1.move(glm::vec3(-30, 0.4, -1)); // bird1 location 
	auto slingLoad = glm::vec3(-46.5, 3.8, -3);
	auto jumpPoint = glm::vec3(-38, 8, -3);

	loadBird.addAnimation(std::make_unique<PauseAnimation>(objects[1], 2));
	loadBird.addAnimation(std::make_unique<RotationAnimation>(objects[1], 1, glm::vec3(0, M_PI, 0))); // 180 
	loadBird.addAnimation(std::make_unique<RotationAnimation>(objects[1], 0.5, glm::vec3(0, 0, -M_PI/4.6))); // tilt up 
	loadBird.addAnimation(std::make_unique<BezierTranslationAnimation>(objects[1], 1.5, objects[1].getPosition(), jumpPoint, slingLoad));
	loadBird.addAnimation(std::make_unique<RotationAnimation>(objects[1], 0.5, glm::vec3(0, 0, M_PI / 4))); // tilt back 
	loadBird.addAnimation(std::make_unique<RotationAnimation>(objects[1], 1, glm::vec3(0, M_PI, 0))); // 180 

	std::vector<Animator> animators;
	animators.push_back(std::move(loadBird));

	// Scene -----------------------------------------------------------------------
	return Scene{
		phongLighting(),
		std::move(objects),
		std::move(animators)
		/*{
			ground1, 
			ground2, 
			ground3,
			ground4,
			ground5,  
			base_pallet_left,
			base_pallet_right,
			low_pallet_left,
			low_pallet_right,
			mid_pallet,
			up_pallet_left,
			up_pallet_right,
			up_mid_pallet, 
			pig,
			bird1,
			bird2, 
			bird3, 
			slingshot
		}*/
	};
}

/**
 * @brief  Demonstrates loading a square, oriented as the "floor", with a manually-specified texture
 * that does not come from Assimp.
 * @return 
 */
Scene marbleSquare() {
	std::vector<Texture> textures = {
		loadTexture("models/White_marble_03/Textures_4K/white_marble_03_4k_baseColor.tga", "baseTexture"),
	};

	auto mesh = Mesh3D::square(textures);
	auto square = Object3D(std::vector<Mesh3D>{mesh});
	square.grow(glm::vec3(5, 5, 5));
	square.rotate(glm::vec3(-3.14159 / 4, 0, 0));
	return Scene{
		phongLighting(),
		{square}
	};
}

/**
 * @brief Constructs a scene of the textured Stanford bunny.
 */
Scene bunny() {
	auto bunny = assimpLoad("models/bunny_textured.obj", true);
	//bunny.grow(glm::vec3(12, 12, 12));
	//bunny.move(glm::vec3(0.2, 7, 3));

	bunny.move(glm::vec3(-2, 6, -10));
	bunny.grow(glm::vec3(9, 9, 9));
	

	//glm::vec3 gravity(0, -9.8, 0);
	//bunny.setVelocity(glm::vec3(1, 1, 1));
	//bunny.setMass(4);
	//bunny.addForceToList(gravity);

	std::vector<Object3D> objects;
	objects.push_back(std::move(bunny));

	// Test animation 
	Animator animBunny;
	 
	animBunny.addAnimation(std::make_unique<RotationAnimation>(objects[0], 5, glm::vec3(0, 6.28, 0)));
	animBunny.addAnimation(std::make_unique<RotationAnimation>(objects[0], 5, glm::vec3(6.28, 0, 0)));
	//animBunny.addAnimation(std::make_unique<BezierTranslationAnimation>(objects[0], 3, objects[0].getPosition(), glm::vec3(1, 0.5, -6), glm::vec3(2, -0.5, -6)));
	
	std::vector<Animator> animators;
	animators.push_back(std::move(animBunny));

	return Scene{
		phongLighting(),
		//{bunny},
		std::move(objects),
		std::move(animators) 
	};
} 

/**
 * @brief Constructs a scene of a tiger sitting in a boat, where the tiger is the child object
 * of the boat.
 * @return 
 */
Scene lifeOfPi() {
	// This scene is more complicated; it has child objects, as well as animators.
	auto boat = assimpLoad("models/boat/boat.fbx", true);
	boat.move(glm::vec3(0, -0.7, 0));
	boat.grow(glm::vec3(0.01, 0.01, 0.01));
	auto tiger = assimpLoad("models/tiger/scene.gltf", true);
	tiger.move(glm::vec3(0, -5, 10));
	boat.addChild(std::move(tiger));
	
	// Because boat and tiger are local variables, they will be destroyed when this
	// function terminates. To prevent that, we need to move them into a vector, and then
	// move that vector as part of the return value.
	std::vector<Object3D> objects;
	objects.push_back(std::move(boat));
	
	// We want these animations to referenced the *moved* objects, which are no longer
	// in the variables named "tiger" and "boat". "boat" is now in the "objects" list at
	// index 0, and "tiger" is the index-1 child of the boat.
	Animator animBoat;
	//animBoat.addAnimation([&objects[0]]() {
		//return std::make_unique<RotationAnimation>(objects[0], 10, glm::vec3(0, 6.28, 0));
	//});
	animBoat.addAnimation(std::make_unique<RotationAnimation>(objects[0], 10, glm::vec3(0, 6.28, 0)));
	Animator animTiger;
	//animTiger.addAnimation([&objects[0].getChild(1)]() {
		//return std::make_unique<RotationAnimation>(objects[0].getChild(1), 10, glm::vec3(0, 0, 6.28));
	//});
	animTiger.addAnimation(std::make_unique<RotationAnimation>(objects[0].getChild(1), 10, glm::vec3(0, 0, 6.28)));

	// The Animators will be destroyed when leaving this function, so we move them into
	// a list to be returned.
	std::vector<Animator> animators;
	animators.push_back(std::move(animBoat));
	animators.push_back(std::move(animTiger));

	// Transfer ownership of the objects and animators back to the main.
	return Scene {
		textureMapping(),
		std::move(objects),
		std::move(animators)
	};
}

int main() {

	// Initialize the window and OpenGL.
	sf::ContextSettings Settings;
	Settings.depthBits = 24; // Request a 24 bits depth buffer
	Settings.stencilBits = 8;  // Request a 8 bits stencil buffer
	Settings.antialiasingLevel = 2;  // Request 2 levels of antialiasing
	sf::RenderWindow window(sf::VideoMode{ 1300, 800 }, "SFML Demo", sf::Style::Resize | sf::Style::Close, Settings);
	gladLoadGL();
	glEnable(GL_DEPTH_TEST);

	// Initialize scene objects. 
	auto scene = testScene();
	
	/**/
	auto& slingshot = scene.objects[0];
	auto& bird1 = scene.objects[1];
	auto& bird2 = scene.objects[2];
	auto& bird3 = scene.objects[3];  
	auto& lower_left_pallet = scene.objects[11];

	auto& animators = scene.animators; 

	std::vector<std::reference_wrapper<Object3D>> birdQueue; 
	birdQueue.push_back(std::ref(bird1)); 
	birdQueue.push_back(std::ref(bird2)); 
	birdQueue.push_back(std::ref(bird3)); 

	// Start with the leftmost bird 
	auto currentBird = 0; 
	 
	// assign starting values to the birds 
	for (auto& birdReference : birdQueue) {
		Object3D& bird = birdReference.get();

		bird.setVelocity(glm::vec3(0, 0, 0));
		bird.setMass(4);
		bird.addForceToList(glm::vec3(0, -9.8, 0));
	}

	// wall / pallet dim
	float palletHeight = 10;
	float palletWidth = 3; 
	float palletDepth = 3;
	 
	// make an end screen https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Sprite.php
	sf::Texture endScreenTexture;
	endScreenTexture.loadFromFile("textures/end.jpg");
	sf::Sprite endScreen;
	endScreen.setTexture(endScreenTexture);
	const sf::Vector2u windowSize = window.getSize();
	endScreen.setScale(static_cast<float>(windowSize.x) / endScreenTexture.getSize().x,
					   static_cast<float>(windowSize.y) / endScreenTexture.getSize().y);
	  
	 
	bool gameEnd = false; 
	 
	auto cameraPosition = glm::vec3(-30, 10, 30);// 005 // -30, 10, 30  // bunny 0,10,30
	auto cameraFront = glm::vec3(0, 0, -1); // test
	auto cameraUp = glm::vec3(0, 1, 0); //test 
	auto camera = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); // added front 
	//auto camera = glm::lookAt(cameraPosition, glm::vec3(0, 5, -1), glm::vec3(0, 1, 0)); // original line 
	auto perspective = glm::perspective(glm::radians(45.0), static_cast<double>(window.getSize().x) / window.getSize().y, 0.1, 100.0);

	
	ShaderProgram& mainShader = scene.defaultShader;
	mainShader.activate();
	mainShader.setUniform("view", camera);
	mainShader.setUniform("projection", perspective);
	 
	// Lighting parameters
	glm::vec3 ambientColor(0.1, 0.1, 0.1); // white...
	glm::vec3 directionalLight(0,0,-1); 
	glm::vec3 directionalColor(1, 1, 1); // grey...

	// Set the lighting parameters as uniforms
	//ShaderProgram program = phongLighting(); 
	float ambient = 0.9;
	float diffuse = 0.5;
	float specular = 10; 
	float shininess = 32;
	 
	//program.activate(); 
	mainShader.setUniform("baseTexture", 0);
	mainShader.setUniform("material", glm::vec4(ambient, diffuse, specular, shininess));
	mainShader.setUniform("ambientColor", ambientColor);
	mainShader.setUniform("directionalLight", directionalLight);
	mainShader.setUniform("directionalColor", directionalColor);
	mainShader.setUniform("viewPos", camera);

	// do a spotlight for EGG https://learnopengl.com/Lighting/Light-casters
	glm::vec3 color(1, 0, 1);
	mainShader.setUniform("spotLightPos", glm::vec3(28, 4, -1)); // put over the first bird for now 
	mainShader.setUniform("spotLightDir", glm::vec3(0, -1, 0)); // face to floor 
	mainShader.setUniform("spotLightColor", color);
	mainShader.setUniform("spotLightAmbient", 1.0f);
	mainShader.setUniform("spotLightDiffuse", 0.8f);
	mainShader.setUniform("spotLightSpecular", 1.0f); 
	mainShader.setUniform("spotLightCutOff", glm::cos(glm::radians(12.5f)));
	mainShader.setUniform("spotLightOuterCutOff", glm::cos(glm::radians(17.5f)));

	// Physics?

	//glm::vec3 gravity(0, -9.8, 0); // set gravity later 
	glm::vec3 force0(0, 0, 0); 
	glm::vec3 velocity(1, 1, 1);
	auto mass = 5.0f;

	auto friction = force0;
	auto gravity = force0; 
	int userPower = 0;
	float velocityThreshold = 10.0f;
	// create a boolean for when the user hits the release button for the bird 
	bool hitRelease = false; 
	bool currentBirdCanChange = false; 
	bool flag1 = false; 
	bool flag2 = false; 
	
	// Ready, set, go!
	for (auto& animator : scene.animators) {
		animator.start();
	}
	bool running = true;
	sf::Clock c;
	auto last = c.getElapsedTime();

	// KEYBOARD INPUTS - inserting and erasing 
	std::unordered_set<sf::Keyboard::Key> keysPressed;

	//auto userPull = 0; 
	//auto isReadiedUp = false;

	while (running) {
		 
		auto now = c.getElapsedTime();
		auto diff = now - last;
		auto diffSeconds = diff.asSeconds();

		//bunny.tick(diffSeconds);
		birdQueue[currentBird].get().tick(diffSeconds);

		last = now;
		for (auto& animator : scene.animators) {
			animator.tick(diffSeconds);
		} 



		//printPosition(bunny.getPosition());
		printPosition(birdQueue[currentBird].get().getPosition());

		sf::Event ev;
		while (window.pollEvent(ev)) {
			if (ev.type == sf::Event::Closed) {
				running = false;
			}
			else if (ev.type == sf::Event::KeyPressed) {
				keysPressed.insert(ev.key.code);
			}
			else if (ev.type == sf::Event::KeyReleased) {
				keysPressed.erase(ev.key.code);
			//	hitRelease = true; 
			}
		} 

		if (gameEnd == true) {
			window.clear();
			window.draw(endScreen);
			window.display(); 
		}
		else {

			//Camera direction by key press 
			// Reference for camera movement : https://learnopengl.com/Getting-started/Camera 
			/*if (keysPressed.find(sf::Keyboard::Key::A) != keysPressed.end()) {
				cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * 0.05f;
			}
			else if (keysPressed.find(sf::Keyboard::Key::D) != keysPressed.end()) {
				cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * 0.05f;
			}*/
			if (keysPressed.find(sf::Keyboard::Key::B) != keysPressed.end()) {
				//bunny.setVelocity(glm::vec3(1,1,1));
				cameraPosition = birdQueue[currentBird].get().getPosition() + glm::vec3(-1, 0, 30); // Adjust the camera's offset from the bird
				//cameraPosition.z = birdQueue[currentBird].get().getPosition().z;

				// have hit only occur once (don't fly forever if key held) 
				//if (hitRelease == true) {
				birdQueue[currentBird].get().setVelocity(glm::vec3(15, 10, 0));
				//	hitRelease = false; 
				//}

				gravity = glm::vec3(0, -9.8, 0);
				//friction = -0.3f * bunny.getVelocity() * bunny.getMass();
				flag1 = true;
			}

			if (keysPressed.find(sf::Keyboard::Key::L) != keysPressed.end()) { // turn off directional light 
				float ambient = 0.1;
				float diffuse = 0.01;
				float specular = 0.5;
				float shininess = 3;

				mainShader.setUniform("baseTexture", 0);
				mainShader.setUniform("material", glm::vec4(ambient, diffuse, specular, shininess));
				mainShader.setUniform("ambientColor", ambientColor);
				mainShader.setUniform("directionalLight", directionalLight);
				mainShader.setUniform("directionalColor", directionalColor);
			}

			if (keysPressed.find(sf::Keyboard::Key::P) != keysPressed.end()) { // turn on directional light 
				float ambient = 0.9;
				float diffuse = 0.5;
				float specular = 10;
				float shininess = 32;

				mainShader.setUniform("baseTexture", 0);
				mainShader.setUniform("material", glm::vec4(ambient, diffuse, specular, shininess));
				mainShader.setUniform("ambientColor", ambientColor);
				mainShader.setUniform("directionalLight", directionalLight);
				mainShader.setUniform("directionalColor", directionalColor);
			}

			if (keysPressed.find(sf::Keyboard::Key::Escape) != keysPressed.end()) {
				gameEnd = true; 
			}
			 


			bool birdIsInMotion = isBirdInMotion(birdQueue[currentBird].get().getVelocity(), 0.09);
			printBirdVelocity(birdQueue[currentBird].get());
			// update camera with new position 
			if (birdIsInMotion) {
				cameraPosition = birdQueue[currentBird].get().getPosition() + glm::vec3(5, 5, 20);
				camera = glm::lookAt(cameraPosition, birdQueue[currentBird].get().getPosition(), cameraUp);

			}
			else {

				birdQueue[currentBird].get().setVelocity(glm::vec3(0, 0, 0));
				gravity = force0;
				friction = force0;

				std::cout << "bird not in motion" << std::endl;

				if (keysPressed.find(sf::Keyboard::Key::A) != keysPressed.end()) {
					cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * 0.05f;
				}
				else if (keysPressed.find(sf::Keyboard::Key::D) != keysPressed.end()) {
					cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * 0.05f;
				}

				camera = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);




				// iterate bird queue
				//currentBird = currentBird++; 
				flag2 = true;
				if (flag1 && flag2) {
					currentBirdCanChange = true;
				}


				if (currentBirdCanChange) { // make a boolean for currentBirdCanChange . . . so that we dont get stuck in the loop 

					if (currentBird == 0) {
						currentBird = 1;

						std::cout << "Reached next bird (2)" << std::endl; // reached 

						Animator loadBird2;
						// bird1.move(glm::vec3(-30, 0.4, -1)); // bird1 location 
						auto slingLoad = glm::vec3(-46.5, 3.8, -3);
						auto jumpPoint = glm::vec3(-36, 9, -3);

						// Add the load up animation 
						loadBird2.addAnimation(std::make_unique<PauseAnimation>(birdQueue[currentBird].get(), 2));
						loadBird2.addAnimation(std::make_unique<RotationAnimation>(birdQueue[currentBird].get(), 1, glm::vec3(0, M_PI, 0))); // 180   
						loadBird2.addAnimation(std::make_unique<RotationAnimation>(birdQueue[currentBird].get(), 0.5, glm::vec3(0, 0, -M_PI / 4.6))); // tilt up   
						loadBird2.addAnimation(std::make_unique<BezierTranslationAnimation>(birdQueue[currentBird].get(), 1.5, birdQueue[currentBird].get().getPosition(), jumpPoint, slingLoad));
						loadBird2.addAnimation(std::make_unique<RotationAnimation>(birdQueue[currentBird].get(), 0.5, glm::vec3(0, 0, M_PI / 4))); // tilt back   
						loadBird2.addAnimation(std::make_unique<RotationAnimation>(birdQueue[currentBird].get(), 1, glm::vec3(0, M_PI, 0))); // 180   

						loadBird2.start();
						animators.push_back(std::move(loadBird2));


						currentBirdCanChange = false;
					}
					else if (currentBird == 1) {
						currentBird = 2;
						//birdQueue[currentBird].get() 

						Animator loadBird3;
						// bird1.move(glm::vec3(-30, 0.4, -1)); // bird1 location 
						auto slingLoad = glm::vec3(-46.5, 3.8, -3);
						auto jumpPoint = glm::vec3(-36, 9, -3);

						// Add the load up animation
						loadBird3.addAnimation(std::make_unique<PauseAnimation>(birdQueue[currentBird].get(), 2));
						loadBird3.addAnimation(std::make_unique<RotationAnimation>(birdQueue[currentBird].get(), 1, glm::vec3(0, M_PI, 0))); // 180   
						loadBird3.addAnimation(std::make_unique<RotationAnimation>(birdQueue[currentBird].get(), 0.5, glm::vec3(0, 0, -M_PI / 4.6))); // tilt up   
						loadBird3.addAnimation(std::make_unique<BezierTranslationAnimation>(birdQueue[currentBird].get(), 1.5, birdQueue[currentBird].get().getPosition(), jumpPoint, slingLoad));
						loadBird3.addAnimation(std::make_unique<RotationAnimation>(birdQueue[currentBird].get(), 0.5, glm::vec3(0, 0, M_PI / 4))); // tilt back   
						loadBird3.addAnimation(std::make_unique<RotationAnimation>(birdQueue[currentBird].get(), 1, glm::vec3(0, M_PI, 0))); // 180   

						loadBird3.start();
						animators.push_back(std::move(loadBird3));

						currentBirdCanChange = false;
					}
					else if (currentBird == 2) {
						// End 

						currentBirdCanChange = false;
					}
				}
				flag1 = false;
				flag2 = false;


			}
			//camera = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
			mainShader.setUniform("view", camera);

			//bunny.addForceToList(gravity);
			birdQueue[currentBird].get().addForceToList(gravity);

			// collide with the ground 
			if (birdQueue[currentBird].get().getPosition().y <= 0) { // say ground is at -1 
				auto velocity = birdQueue[currentBird].get().getVelocity();
				velocity.y = -velocity.y * 0.6; // reduce 
				birdQueue[currentBird].get().setVelocity(velocity);
				birdQueue[currentBird].get().setPosition(glm::vec3(birdQueue[currentBird].get().getPosition().x, 0, birdQueue[currentBird].get().getPosition().z));
			}
			// makes contact with the wall/pallet - center point located at (1,2,-1)
			// lower left pallet 
			if (birdQueue[currentBird].get().getPosition().x > -0.5
				&& birdQueue[currentBird].get().getPosition().x <= 0
				&& birdQueue[currentBird].get().getPosition().y < 5
				) {
				std::cout << "CONTACT - lower left pallet" << std::endl;

				auto velo = birdQueue[currentBird].get().getVelocity();
				velo.x = -velo.x * 0.8;
				birdQueue[currentBird].get().setVelocity(velo);
				birdQueue[currentBird].get().setPosition(glm::vec3(-0.5, birdQueue[currentBird].get().getPosition().y, birdQueue[currentBird].get().getPosition().z));

			}
			// makes contact with the higher wall/pallet - center point located at (1.8,7.2,-1)
			if (birdQueue[currentBird].get().getPosition().x > 0
				&& birdQueue[currentBird].get().getPosition().x <= 1
				&& birdQueue[currentBird].get().getPosition().y < 10.2
				&& birdQueue[currentBird].get().getPosition().y > 4.3
				) {
				std::cout << "CONTACT - upper left pallet" << std::endl;

				auto velo = birdQueue[currentBird].get().getVelocity();
				velo.x = -velo.x * 0.99;
				birdQueue[currentBird].get().setVelocity(velo);
				birdQueue[currentBird].get().setPosition(glm::vec3(0, birdQueue[currentBird].get().getPosition().y, birdQueue[currentBird].get().getPosition().z));
			}

			// makes contact with middle pallet 
			if (birdQueue[currentBird].get().getPosition().x > 1
				&& birdQueue[currentBird].get().getPosition().x <= 6
				&& birdQueue[currentBird].get().getPosition().y <= 4.5
				&& birdQueue[currentBird].get().getPosition().y > 4
				) { // say ground is at -1 -> 0 bc -1 too low 
				std::cout << "CONTACT - middle pallet" << std::endl;

				auto velocity = birdQueue[currentBird].get().getVelocity();
				velocity.y = -velocity.y * 0.75; // reduce 
				birdQueue[currentBird].get().setVelocity(velocity);
				birdQueue[currentBird].get().setPosition(glm::vec3(birdQueue[currentBird].get().getPosition().x, 4.5, birdQueue[currentBird].get().getPosition().z));
			}

			// makes contact with roof pallet 
			if (birdQueue[currentBird].get().getPosition().x > 1.5
				&& birdQueue[currentBird].get().getPosition().x <= 5
				&& birdQueue[currentBird].get().getPosition().y <= 11
				&& birdQueue[currentBird].get().getPosition().y > 9.7
				) { // say ground is at -1 -> 0 bc -1 too low 
				std::cout << "CONTACT - roof pallet" << std::endl;

				auto velocity = birdQueue[currentBird].get().getVelocity();
				velocity.y = -velocity.y * 0.7; // reduce 
				birdQueue[currentBird].get().setVelocity(velocity);
				//birdQueue[currentBird].get().setRotationalVelocity(glm::vec3(0,0,-3)); // works but needs to slow down 
				birdQueue[currentBird].get().setPosition(glm::vec3(birdQueue[currentBird].get().getPosition().x, 11, birdQueue[currentBird].get().getPosition().z));
			}


			// makes contact with the eggs 
			if (birdQueue[currentBird].get().getPosition().x > 23
				&& birdQueue[currentBird].get().getPosition().x < 30
				&& birdQueue[currentBird].get().getPosition().y <= 2
				&& birdQueue[currentBird].get().getPosition().y > 0) {

				std::cout << "CONTACT - eggs" << std::endl; 
				 

				gameEnd = true;
			}
			 

			// friction of the floor 
			// my mu is 0.3 and go opposite direction of velocity (?) :D ? 
			auto friction = -0.3f * birdQueue[currentBird].get().getVelocity() * birdQueue[currentBird].get().getMass();
			birdQueue[currentBird].get().addForceToList(friction);
 
			// Clear the OpenGL "context".
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Render each object in the scene.
			for (auto& o : scene.objects) {
				o.render(window, mainShader);
			}


			window.display();
		}
	}


	return 0;
}


