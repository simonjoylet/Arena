/* admin_tool_main.cpp

 Generated by phxrpc_pb2tool from admin.proto

*/

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>

#include "phxrpc_admin_tool.h"
#include "admin_tool_impl.h"

#include "admin_client.h"

#include "phxrpc/file.h"


using namespace phxrpc;


void ShowUsage(const char *program) {
    printf("\nUsage: %s [-c <config>] [-f <func>] [-v]\n", program);

    AdminTool::Name2Func_t *name2func = AdminTool::GetName2Func();

    for (int i{0}; ; ++i) {
        AdminTool::Name2Func_t *iter = &(name2func[i]);

        if (nullptr == iter->name) break;

        printf("    -f %s %s\n", iter->name, iter->usage);
    }
    printf("\n");
    exit(0);
}

int main(int argc, char **argv) {
    const char *func{nullptr};
    const char *config{nullptr};

    for (int i{1}; argc - 1 > i; ++i) {
        if (0 == strcmp(argv[i], "-c")) {
            config = argv[++i];
        }
        if (0 == strcmp(argv[i], "-f")) {
            func = argv[++i];
        }
        if (0 == strcmp(argv[i], "-v")) {
            ShowUsage(argv[0]);
        }
    }

    if (nullptr == func) ShowUsage(argv[0]);

    if (nullptr != config) AdminClient::Init(config);

    AdminTool::Name2Func_t *target{nullptr};

    AdminTool::Name2Func_t *name2func{AdminTool::GetName2Func()};

    for (int i{0}; 100 > i; ++i) {
        AdminTool::Name2Func_t *iter = &(name2func[i]);

        if (nullptr == iter->name) break;

        if (0 == strcasecmp(func, iter->name)) {
            target = iter;
            break;
        }
    }

    if (nullptr == target) ShowUsage(argv[0]);

    OptMap opt_map(target->opt_string);

    if (!opt_map.Parse(argc, argv)) ShowUsage(argv[0]);

    AdminTool::ToolFunc_t targefunc = target->func;

    AdminToolImpl tool;

    if (0 != (tool.*targefunc)(opt_map)) ShowUsage(argv[0]);

    return 0;
}

