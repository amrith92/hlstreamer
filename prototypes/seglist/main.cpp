#include <boost/mpi.hpp>
#include <iostream>
#include <sstream>
#include <boost/serialization/string.hpp>

namespace mpi = boost::mpi;

int main(int argc, char **argv)
{
	mpi::environment env(argc, argv);
	mpi::communicator world;

	std::ostringstream oss;
	std::string v;
	if (world.rank() == 0) {
		oss << "Boo!";
	} else {
		oss << "ACK " << world.rank();
	}
	v = oss.str();

	broadcast(world, v, 0);

	std::cout << "\nProcess #" << world.rank() << " says " << v << std::endl;

	return 0;
}
