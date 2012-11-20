// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PPAPI_HOST_RESOURCE_HOST_H_
#define PPAPI_HOST_RESOURCE_HOST_H_

#include <vector>

#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/host/ppapi_host_export.h"
#include "ppapi/host/resource_message_handler.h"
#include "ppapi/shared_impl/host_resource.h"

namespace IPC {
class Message;
}

namespace ppapi {
namespace host {

struct HostMessageContext;
class PpapiHost;
class ResourceMessageFilter;

// Some (but not all) resources have a corresponding object in the host side
// that is kept alive as long as the resource in the plugin is alive. This is
// the base class for such objects.
class PPAPI_HOST_EXPORT ResourceHost : public ResourceMessageHandler {
 public:
  ResourceHost(PpapiHost* host, PP_Instance instance, PP_Resource resource);
  virtual ~ResourceHost();

  PpapiHost* host() { return host_; }
  PP_Instance pp_instance() const { return pp_instance_; }
  PP_Resource pp_resource() const { return pp_resource_; }

  // This runs any message filters in |message_filters_|. If the message is not
  // handled by these filters then the host's own message handler is run. True
  // is always returned (the message will always be handled in some way).
  virtual bool HandleMessage(const IPC::Message& msg,
                             HostMessageContext* context) OVERRIDE;

  virtual void SendReply(const ReplyMessageContext& context,
                         const IPC::Message& msg) OVERRIDE;

 protected:
  // Adds a ResourceMessageFilter to handle resource messages. Incoming
  // messages will be passed to the handlers of these filters before being
  // handled by the resource host's own message handler. This allows
  // ResourceHosts to easily handle messages on other threads.
  void AddFilter(scoped_refptr<ResourceMessageFilter> filter);

 private:
  // The host that owns this object.
  PpapiHost* host_;

  PP_Instance pp_instance_;
  PP_Resource pp_resource_;

  // A vector of message filters which the host will forward incoming resource
  // messages to.
  std::vector<scoped_refptr<ResourceMessageFilter> > message_filters_;

  DISALLOW_COPY_AND_ASSIGN(ResourceHost);
};

}  // namespace host
}  // namespace ppapi

#endif  // PPAPI_HOST_RESOURCE_HOST_H_
