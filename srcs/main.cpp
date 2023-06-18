#include "../includes/Server.hpp"

int	main(int argc, char **argv){
	if (argc == 3){
        irc_server object;
        try{
            object.check_port(argv);
            object.init_sockets();
            object.ReusableSocket();
            object.non_blocking();
            object.bind_sockets();
            object.listenToIncomingconnection();
            object.multipleconnection();
        }
        catch(std::exception &e){
            std::cerr << e.what() << std::endl;
        }
	}
}