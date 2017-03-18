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

	auto root = scene.getRoot();

	auto chassis = addTransform(scene, "chassis");
	auto road = addTransform(scene, "road");

	auto under = addTransform(scene, chassis, "under");
	auto wheel = addTransform(scene, under, "wheel");

	auto signs = addTransform(scene, road, "signs");

	auto interior = addTransform(scene, chassis, "interior");

	return 0;
}