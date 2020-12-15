pragma solidity ^0.5.7;


contract Sync {
    address _owner;

    struct Position {
        uint256 x;
        uint used;
    }

    mapping(address => Position) position;


    //   event Register(address indexed _from, address indexed _to, uint256 _value);
    //   event Approval(address indexed _owner, address indexed _spender, uint256 _value);

    constructor() public
    {
        _owner = msg.sender;
        position[_owner] = Position({x : 0, used : 1});
    }

    function register(address addr) public returns (bool){

        if (position[addr].used == 1) {
            return false;
        }

        position[addr] = Position({x : 0, used : 1});
        return true;
    }

    function test(address addr) public view returns (bool){
        return position[addr].used == 1;
    }

    function update(address target,
        uint256 x) public returns (bool){
        if (!(position[target].used == 1))
            return false;
        position[target] = Position({x : x, used : 1});
        return true;
    }

    function test2(uint256 x, uint256 y) public pure returns (bool){


        return true;
    }
}