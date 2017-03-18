#include "dos.h"
#include "dos_scene.h"

using namespace dos;

int main(int argc, char** argv)
{
	auto& log = Logger::LogOut::get();
	log.logToConsole = true;
	log.logToFile = true;

	hmm_vec2 a{ 0.5f, 0.5f };
	hmm_vec2 b{ 0.5f, 0.5f };

	auto c = a + b;

	Scene scene;

	auto root = addEntity(scene);

	auto node11 = addEntity(scene, root);
	auto node12 = addEntity(scene, root);

	auto node21 = addEntity(scene, node11);
	auto node31 = addEntity(scene, node21);

	auto node13 = addEntity(scene, root);

	return 0;
}