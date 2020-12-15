solc --evm-version homestead --combined-json bin,hashes --pretty-json --optimize $1.sol | head -n -3 | awk '{if(NR>4)print}' >./json/$1.json
