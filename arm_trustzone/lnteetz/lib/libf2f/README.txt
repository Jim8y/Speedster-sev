libf2f is a (work-in-progress) C++ boost/asio library for building p2p networks

## Originally developed by [RJ/libf2f](https://github.com/RJ/libf2f) 10 years ago
## Updated by Jinghui Liao <jinghui@wayne.edu>
## This version is specific for my own project, might not suitable for others and please do not replace this lib with its original code.

Provides:
* network servent/router
* connection handling and abstraction
* message class that is marshalled to/from the network
* very simple test app (only supports ping+pong)

Does not provide:
* high level protocol, you have to design implement this yourself
  (ie, extend the Protocol class)

Will hopefully provide in future:
* UPnP control capability to set up port-fwds
* Flow control strategies to manage bandwidth utilisation
* A better, simple demo app

