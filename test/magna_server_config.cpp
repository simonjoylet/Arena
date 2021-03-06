/* magna_server_config.cpp

 Generated by phxrpc_pb2server from magna.proto

*/

#include "magna_server_config.h"

#include "magna.pb.h"


MagnaServerConfig::MagnaServerConfig() {
}

MagnaServerConfig::~MagnaServerConfig() {
}

bool MagnaServerConfig::Read(const char *config_file) {
    bool ret{ep_server_config_.Read(config_file)};

    if (0 == strlen(ep_server_config_.GetPackageName())) {
        ep_server_config_.SetPackageName(
magna::RegisterRequest::default_instance().GetDescriptor()->file()->package().c_str());
    }

    return ret;
}

phxrpc::HshaServerConfig &MagnaServerConfig::GetHshaServerConfig() {
    return ep_server_config_;
}

