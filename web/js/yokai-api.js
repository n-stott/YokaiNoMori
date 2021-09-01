searchBestMove = Module.cwrap('searchBestMove', 'number', ['string', 'string', 'string', 'number', 'number'])
get_board = Module.cwrap('board', 'string', [])
get_reserve0 = Module.cwrap('reserve0', 'string', [])
get_reserve1 = Module.cwrap('reserve1', 'string', [])

validAction = Module.cwrap('validAction', 'number', ['string', 'string', 'string', 'number', 'string', 'string', 'number', 'number'])
playAction = Module.cwrap('playAction', 'number', ['string', 'string', 'string', 'number', 'string', 'string', 'number', 'number'])

class Configuration {

    constructor(board, reserve0, reserve1, player) {
        this.board = board;
        this.reserve0 = reserve0;
        this.reserve1 = reserve1;
        this.currentPlayer = player;
    }

    static Default() {
        return new Configuration('TKB.P..p.bkt', '', '', 0);
    }

    update() {
        this.board = get_board()
        this.reserve0 = get_reserve0()
        this.reserve1 = get_reserve1()
        this.currentPlayer = 1-this.currentPlayer
    }

    autoPlay(depth) {
        console.log("before AI : ", this.toString());
        searchBestMove(
            this.board,
            this.reserve0,
            this.reserve1,
            this.currentPlayer,
            depth
        )
        this.update()
        console.log("after AI : ", this.toString());
    }

    play(action, piece, start, end) {
        let valid = validAction(
            this.board,
            this.reserve0,
            this.reserve1,
            this.currentPlayer,
            action,
            piece,
            start,
            end
        )
        if(valid) {
            console.log("before HU : ", this.toString());
            playAction(
                this.board,
                this.reserve0,
                this.reserve1,
                this.currentPlayer,
                action,
                piece,
                start,
                end
            )
            this.update()
            console.log("after HU : ", this.toString());
        }
    }

    winner() {
        if(this.reserve0.length > 0 && this.reserve0[this.reserve0.length-1] == 'k') return 0;
        if(this.reserve1.length > 0 && this.reserve1[this.reserve1.length-1] == 'K') return 1;
        return -1;
    }

    toString() {
        return this.board + '|' + this.reserve0 + '|' + this.reserve1;
    }
    
}

class GameHistory {
    constructor() {
        this.history = [];
    }
}