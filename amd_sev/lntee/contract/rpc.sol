pragma solidity ^0.5.8;

contract rpsAdvanced
{
    int chose = 2;
    constructor () public
    {
    }



    function play() public pure returns (bool)
    {
        return 1 > 2;
        //        if (msg.sender == player1)
        //            player1Choice = choice;
        //        else if (msg.sender == player2)
        //            player2Choice = "rock";
        //        checkWinner();
    }
}
