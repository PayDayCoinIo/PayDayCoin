# Installing 
How to install PayDay on debian-based system

1. install curl    
``` sudo apt-get -y install curl ```
1. run automated install 
``` curl -L https://raw.githubusercontent.com/PayDayCoinIo/PayDayCoin/master/autobuild_latest?nocache=`date +%s%N` | bash - ```
1. basic configure daemon
``` ./config_basic ```
1. Masternode configure
``` ./config_masternode ```

# PayDay Coin Specification

Coin Name: PayDay Coin

Ticker: PDX

Coin Type: POS

Maximum Block Size: 3 MB

Block Reward: 280

Block Time:	120 seconds (720 blocks/day)

First PoS Block: 7000 Block

Minimum Staking Age: 24 Hr

Coin Maturity: 63

Max Supply: 294,336,000 Coins

Premined: 11,773,440

Masternode Stake: 20000

RPC Port: 7215

Network Port: 7214
