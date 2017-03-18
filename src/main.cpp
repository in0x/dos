#include "dos.h"

int main(int argc, char** argv)
{
	auto& log = Logger::LogOut::get();
	log.logToConsole = true;
	log.logToFile = true;

	Logger::Log("Test");

	hmm_vec2 a{ 0.5f, 0.5f };
	hmm_vec2 b{ 0.5f, 0.5f };

	auto c = a + b;

	return 0;
}