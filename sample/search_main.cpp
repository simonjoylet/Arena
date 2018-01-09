/* search_main.cpp

 Generated by phxrpc_pb2server from search.proto

*/

#include <iostream>
#include <memory>
#include <unistd.h>
#include <signal.h>

#include "phxrpc_search_dispatcher.h"
#include "search_service_impl.h"
#include "search_server_config.h"

#include "phxrpc/rpc.h"
#include "phxrpc/msg.h"
#include "phxrpc/file.h"


using namespace std;


void Dispatch(const phxrpc::BaseRequest *request,
              phxrpc::BaseResponse *response,
              phxrpc::DispatcherArgs_t *args) {
    ServiceArgs_t *service_args = (ServiceArgs_t *)(args->service_args);

    SearchServiceImpl service(*service_args);
    SearchDispatcher dispatcher(service, args);

    phxrpc::BaseDispatcher<SearchDispatcher> base_dispatcher(
            dispatcher, SearchDispatcher::GetMqttFuncMap(),
            SearchDispatcher::GetURIFuncMap());
    if (!base_dispatcher.Dispatch(request, response)) {
        response->DispatchErr();
    }
}

void ShowUsage(const char *program) {
    printf("\n");
    printf("Usage: %s [-c <config>] [-d] [-l <log level>] [-v]\n", program);
    printf("\n");

    exit(0);
}

int main(int argc, char **argv) {
    const char *config_file{nullptr};
    bool daemonize{false};
    int log_level{-1};
    extern char *optarg;
    int c;
    while (EOF != (c = getopt(argc, argv, "c:vl:d"))) {
        switch (c) {
            case 'c' : config_file = optarg; break;
            case 'd' : daemonize = true; break;
            case 'l' : log_level = atoi(optarg); break;

            case 'v' :
            default: ShowUsage(argv[0]); break;
        }
    }

    if (daemonize) phxrpc::ServerUtils::Daemonize();

    assert(signal(SIGPIPE, SIG_IGN) != SIG_ERR);

    //set customize log/monitor
    //phxrpc::setlog(openlog, closelog, vlog);
    //phxrpc::MonitorFactory::SetFactory(new YourSelfsMonitorFactory());

    if (nullptr == config_file) ShowUsage(argv[0]);

    SearchServerConfig config;
    if (!config.Read(config_file)) ShowUsage(argv[0]);

    if (log_level > 0) config.GetHshaServerConfig().SetLogLevel(log_level);

    phxrpc::openlog(argv[0], config.GetHshaServerConfig().GetLogDir(),
            config.GetHshaServerConfig().GetLogLevel());

    ServiceArgs_t service_args;
    service_args.config = &config;

    phxrpc::HshaServer server(config.GetHshaServerConfig(), Dispatch, &service_args);
    server.RunForever();

    phxrpc::closelog();

    return 0;
}

