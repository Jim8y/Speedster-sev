pragma solidity ^0.5.6;


contract Monopoly {

    // Standard Token variables
    /* Public variables of the token */
    string public name;
    string public symbol;
    uint8 public decimals;
    uint256 public totalSupply;

    address public bankManager;
    uint256 public bankBalance = 0;
    /* This creates an array with all balances */
    mapping(address => uint256) public balanceOf;
    mapping(address => uint8) public property;
    // Monopoly Spesific
    uint256 public finesBalance = 0;

    struct FineClaim {
        address to;
        bool approved;
        uint expiretime;
    }

    struct Property {
        uint id;
        uint price;
        string name;
        int rent;
        int pos;
    }

    struct Player {
        uint balance;
        Property[30] property;
    }

    FineClaim public activeFineClaim;

    string[] public charms = [
    "Battleship",
    "Boot",
    "Scottie",
    "Iron",
    "Racecar",
    "Hat",
    "Thimble",
    "Wheelbarrow"
    ];

    mapping(string => bool) _charms;
    mapping(string => address) charmToOwner;
    mapping(address => string) public ownerToCharm;

    /* This generates a public event on the blockchain that will notify clients */
    event Transfer(string from, address indexed to, uint256 value);
    event Paid(string from, string to, uint256 value);
    event ClaimFines(string who);

    constructor(
        uint256 initialSupply,
        string memory tokenName,
        uint8 decimalUnits,
        string memory tokenSymbol,
        address ownerNew
    ) public {
        bankBalance = initialSupply;
        // Update total supply
        totalSupply = initialSupply;
        // Update total supply
        name = tokenName;
        // Set the name for display purposes
        symbol = tokenSymbol;
        // Set the symbol for display purposes
        decimals = decimalUnits;
        // Amount of decimals for display purposes

        // msg.sender in this context is not the same as in the MonopolyGame context,
        // since this contract gets executed from the MonopolyGame address, the sender is 
        // MonopolyGame.
        transferOwnership(ownerNew);
        newBankManager(ownerNew);

        // This looks stupid, but it is way cheaper than
        // doing this every time you need to check a charm name
        for (uint i = 0; i < charms.length; i++) {
            _charms[charms[i]] = true;
        }
    }

    function resetGame() public onlyOwner {
        for (uint i = 0; i < charms.length; i++) {

            address toDelete = charmToOwner[charms[i]];
            ownerToCharm[toDelete] = "Deleted";
            charmToOwner[charms[i]] = address(0x0);
            balanceOf[toDelete] = 0;
        }

        bankBalance = totalSupply;
        finesBalance = 0;
        newBankManager(owner);
    }

    // The owner must be the bank manager to do this
    // true after a reset
    function assignPlayer(string memory charm, address to) public onlyOwner {
        require(_validCharm(charm));
        ownerToCharm[to] = charm;
        charmToOwner[charm] = to;
        bankToCharm(charm, 1500);
    }

    modifier onlyPlayers {
        require(_validCharm(ownerToCharm[msg.sender]));
        _;
    }

    function _validCharm(string memory charmName) view internal returns (bool isValid) {
        isValid = _charms[charmName];
        return isValid;
    }


    /* Internal transfer, only can be called by this contract */
    function _transfer(address _from, string memory _fromCharm, address _to, string memory _toCharm, uint _value) internal {
        require(balanceOf[_from] > _value);
        // Check if the sender has enough
        require(balanceOf[_to] + _value > balanceOf[_to]);
        // Check for overflows
        balanceOf[_from] -= _value;
        // Subtract from the sender
        balanceOf[_to] += _value;
        // Add the same to the recipient
        emit Transfer(_fromCharm, _to, _value);
        emit    Paid(_fromCharm, _toCharm, _value);
    }


    // Players only paying
    // ============================================
    function payToCharm(string memory toCharmName, uint256 value) public onlyPlayers {
        require(_validCharm(toCharmName));
        _transfer(msg.sender, ownerToCharm[msg.sender], charmToOwner[toCharmName], toCharmName, value);
    }

    function payToBank(uint256 value) public onlyPlayers {

        require(balanceOf[msg.sender] > value);
        // Check if the sender has enough
        require(bankBalance + value > bankBalance);
        // Check for overflows
        balanceOf[msg.sender] -= value;
        // Subtract from the sender
        bankBalance += value;
        // Add the same to the recipient
        emit Paid(ownerToCharm[msg.sender], "Bank", value);
    }

    function payToFines(uint256 value) public onlyPlayers {

        require(balanceOf[msg.sender] > value);
        // Check if the sender has enough
        require(finesBalance + value > finesBalance);
        // Check for overflows
        balanceOf[msg.sender] -= value;
        // Subtract from the sender
        finesBalance += value;
        // Add the same to the recipient
        //        emit Paid(ownerToCharm[msg.sender], "Fines", value);
    }

    // Bank paying
    // ============================================
    function bankToCharm(string memory toCharmName, uint256 value) public onlyBankManager {
        require(_validCharm(toCharmName));

        address to = charmToOwner[toCharmName];

        require(bankBalance > value);
        // Check if the sender has enough
        require(balanceOf[to] + value > balanceOf[to]);
        // Check for overflows
        bankBalance -= value;
        // Subtract from the sender
        balanceOf[to] += value;
        // Add the same to the recipient
        //        emit Transfer("Bank", to, value);
        //        emit Paid("Bank", toCharmName, value);
    }

    // Fines
    //  ============================================
    function claimFine() public onlyPlayers {
        // No active claim
        require(activeFineClaim.approved == true || now > activeFineClaim.expiretime);

        activeFineClaim = FineClaim({
            to : msg.sender,
            approved : false,
            expiretime : now + 5 minutes
            });

        emit    ClaimFines(ownerToCharm[msg.sender]);
    }

    function releaseFine() public onlyPlayers {

        require(activeFineClaim.approved != true || now < activeFineClaim.expiretime);
        require(activeFineClaim.to != msg.sender);


        string storage to = ownerToCharm[activeFineClaim.to];
        uint256 value = finesBalance;

        require(balanceOf[activeFineClaim.to] + finesBalance > balanceOf[activeFineClaim.to]);
        // Check for overflows
        balanceOf[activeFineClaim.to] += finesBalance;
        // Add the same to the recipient
        finesBalance = 0;
        // Subtract from the sender

        activeFineClaim.approved = true;

        //        emit Transfer("Fines", activeFineClaim.to, value);
        //        emit Paid("Fines", to, value);
    }

}