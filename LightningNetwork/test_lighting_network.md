#!/bin/bash

wget https://dl.google.com/go/go1.13.linux-amd64.tar.gz
sha256sum go1.13.linux-amd64.tar.gz | awk -F " " '{ print $1 }'

sudo tar -C /usr/local -xzf go1.13.linux-amd64.tar.gz

export PATH=$PATH:/usr/local/go/bin
export GOPATH=~/gocode
export PATH=$PATH:$GOPATH/bin

git clone https://github.com/lightningnetwork/lnd
cd lnd
git checkout v0.10.4-beta
sudo make install

lnd --bitcoin.active --bitcoin.testnet --debuglevel=debug --bitcoin.node=neutrino --neutrino.connect=faucet.lightning.community

###############
#make btcd
#btcd --testnet --rpcuser=REPLACEME --rpcpass=REPLACEME


### running command 
lncli create

source /opt/openenclave/share/openenclave/openenclaveric

sudo echo "source /opt/openenclave/share/openenclave/openenclaveric" | sudo tee -a ~/.bashrc
sudo echo "source /opt/openenclave/share/openenclave/openenclaveric" | sudo tee -a ~/.bashrc
sudo echo "source /opt/openenclave/share/openenclave/openenclaveric" | sudo tee -a ~/.bashrc


[https://gist.github.com/bretton/0b22a0503a9eba09df86a23f3d625c13](https://gist.github.com/bretton/0b22a0503a9eba09df86a23f3d625c13)

lnd --bitcoin.active --bitcoin.testnet --debuglevel=debug --bitcoin.node=neutrino --neutrino.connect=btcd-testnet.ion.radar.tech

lncli create

lncli unlock

lncli --network=testnet newaddress np2wkh

lncli --network=testnet addinvoice --amt=1

[https://testnet-faucet.mempool.co/](https://testnet-faucet.mempool.co/)