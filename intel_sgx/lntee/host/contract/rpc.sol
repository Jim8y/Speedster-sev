pragma solidity 0.5.10;

contract rpsAdvanced
{
    mapping(string => mapping(string => int)) payoffMatrix;
    address player1;
    address player2;
    string public player1Choice;
    string public player2Choice;
    uint game_winner;

    constructor () public
    {
        // constructor (spoiler alert: rename this if you rename the contract!)
        payoffMatrix["rock"]["rock"] = 0;
        payoffMatrix["rock"]["paper"] = 2;
        payoffMatrix["rock"]["scissors"] = 1;
        payoffMatrix["paper"]["rock"] = 1;
        payoffMatrix["paper"]["paper"] = 0;
        payoffMatrix["paper"]["scissors"] = 2;
        payoffMatrix["scissors"]["rock"] = 2;
        payoffMatrix["scissors"]["paper"] = 1;
        payoffMatrix["scissors"]["scissors"] = 0;
    }

    function register(uint isPlater1) public
    {
        if (isPlater1 == 0)
            player1 = msg.sender;
        else if (isPlater1 == 1)
            player2 = msg.sender;
    }

    function play(string memory choice) public
    {
        if (msg.sender == player1)
            player1Choice = choice;
        else if (msg.sender == player2)
            player2Choice = choice;
    }

    function checkWinner() public
    {
        if (bytes(player1Choice).length != 0 && bytes(player2Choice).length != 0)
        {
            // if both revealed, obtain winner in usual way
            int winner = payoffMatrix[player1Choice][player2Choice];

            if (winner == 1)
                game_winner = 1;

            else if (winner == 2)
            {
                game_winner = 2;
            }

            // unregister players and choices
            player1Choice = "";
            player2Choice = "";
        }
        else {}

    }
}
