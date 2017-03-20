#include "dos.h"
#include "dos_scene.h"

using namespace dos;

int main(int argc, char** argv)
{
	auto& log = Logger::LogOut::get();
	log.logToConsole = true;
	log.logToFile = true;

	/*Scene scene;

	auto root = scene.getRoot();

	auto chassis = addTransform(scene, "chassis");
	auto road = addTransform(scene, "road");

	auto under = addTransform(scene, chassis, "under");
	auto wheel = addTransform(scene, under, "wheel");

	auto signs = addTransform(scene, road, "signs");

	auto interior = addTransform(scene, chassis, "interior");*/

	Scene scene;

	auto N1 = scene.addTransform();
	auto N2 = scene.addTransform();
	auto N3 = scene.addTransform(N1);
	auto N4 = scene.addTransform(N1);
	auto N5 = scene.addTransform();
	auto N6 = scene.addTransform(N3);
	auto N7 = scene.addTransform(N5);
	auto N8 = scene.addTransform(N2);

	return 0;
}