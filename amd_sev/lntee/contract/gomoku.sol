pragma solidity ^0.5.6;

contract Gomoku
{

    // Change this and Rome will crumble
    int _boardDim = 18;
    uint8[19][19] _board;

    address _lastWinner;

    bool _gameInProgress = false;

    uint8 _winner;

    constructor() public
    {}

    function MakeMove(uint256 _playerTurn, uint256 Row, uint256 Column) public
    returns (uint256)
    {
        _board[uint8(Row) - 1][uint8(Column) - 1] = uint8(_playerTurn);
        //
        LookForWinner();
        //
        if (_winner != 0)
        {
            return uint256(0);
        }
        return uint256(0);
    }


    function LookForWinner() internal view
    returns (uint8)
    {
        uint8 r;
        uint8 c;

        for (r = 0; r < _boardDim; r++)
        {
            for (c = 0; c < _boardDim; c++)
            {
                uint8 player = _board[r][c];
                if (player != 0)
                {
                    if (r < _boardDim - 4)
                        if (CheckIfFive(r, c, 1, 0)) return player;

                    if (c < _boardDim - 4)
                    {
                        if (CheckIfFive(r, c, 0, 1)) return player;

                        if (r < _boardDim - 4)
                            if (CheckIfFive(r, c, 1, 1)) return player;
                    }

                    if (c > 3 && r < _boardDim - 4)
                        if (CheckIfFive(r, c, 1, - 1)) return player;
                }
            }
        }
    }

    function CheckIfFive(uint row, uint col, int rowDir, int colDir) internal view
    returns (bool)
    {
        uint8 player = _board[row][col];

        for (int i = 1; i < 5; i++)
        {
            if (_board
            [uint(int(row) + rowDir * i)]
            [uint(int(col) + colDir * i)] != player)
                return false;
        }

        return true;
    }

    function GetLastWinner() public view
    returns (uint8)
    {
        return _winner;
    }


    function GameState() public view
    returns (string memory)
    {
        if (_winner > 0)
            return ("Check Last Winner GetLastWinnerAddress, GG WP");

        bytes[19] memory rows;

        for (uint8 i = 0; i < 19; i++)
        {
            rows[i] = new bytes(19);
        }

        byte[] memory signs = new byte[](3);
        signs[0] = "-";
        signs[1] = "X";
        signs[2] = "O";

        for (uint r = 0; r < 19; r++)
        {
            for (uint i = 0; i < 19; i++)
            {
                bytes(rows[r])[i] = signs[_board[r][i]];
            }
        }

        return strConcat(rows);
    }

    function strConcat(bytes[19] memory arg) internal pure
    returns (string memory)
    {
        string memory str = new string(19 * 19);
        bytes memory supa = bytes(str);

        uint k = 0;
        for (uint i = 0; i < 19; i++)
        {
            for (uint m = 0; m < 19; m++)
            {
                supa[k++] = arg[i][m];
            }
        }

        return string(supa);
    }
}
